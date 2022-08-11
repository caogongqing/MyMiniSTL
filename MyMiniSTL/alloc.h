#ifndef __MYMINISTL_ALLOC_H__
#define __MYMINISTL_ALLOC_H__

#include <new>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include "util.h"
/*
分配内存过程：

　　1）如果申请的内存空间大于128bytes, 则交由第一个分配器处理

　　2）分配器首先将申请内存的大小上调至8的倍数n，并根据n找出其对应的空闲链表地址__my_free_list

　　3）如果该空闲链表中有可用的空闲块，则将此空闲块返回并更新__my_free_list，否则转到4）

　　4）到这一步，说明__my_free_list中没有空闲块可用了，分配器会按照下面的步骤处理：

　　    a) 试着调用_s_chunk_alloc()申请大小为n*20的内存空间，注意的是，此时不一定能申请到n*20大小的内存空间

        b) 如果只申请到大小为n的内存空间，则返回给用户，否则到c)

        c) 将申请到的n*x(a中说了，不一定是n*20)内存块取出一个返回给用户，其余的内存块链到空闲链表__my_free_list中

_s_chunk_alloc()的具体过程为：

    1)如果_s_start_free和_s_end_free之间的空间足够分配n*20大小的内存空间，则从这个空间中取出n*20大小的内存空间，
    更新_s_start_free并返回申请到的内存空间的起始地址，否则转到2)

    2) 如果_s_start_free和_s_end_free之间的空间足够分配大于n的内存空间，则分配整数倍于n的内存空间，更新_s_start_free，
    由nobj返回这个整数，并返回申请到的内存空间的起始地址；否则转到3）

　　3) 到这一步，说明内存池中连一块大小为n的内存都没有了，此时如果内存池中还有一些内存（这些内存大小肯定小于n），则将这些
  内存插入到其对应大小的空闲分区链中

　　4) 调用malloc向运行时库申请大小为（2*n*20 + 附加量）的内存空间， 如果申请成功，更新_s_start_free, _s_end_free和_s_heap_size，
  并重新调用_s_chunk_alloc()，否则转到5)

 　 5) 到这一步，说明4)中调用malloc失败，这时分配器依次遍历16个空闲分区链，只要有一个空闲链，就释放该链中的一个节点，
   重新调用_s_chunk_alloc()

内存释放过程：
    内存的释放过程比较简单，它接受两个参数，一个是指向要释放的内存块的指针p，另外一个表示要释放的内存块的大小n。分配器首
先判断n，如果n>128bytes，则交由第一个分配器去处理；否则将该内存块加到相应的空闲链表中。

*/


namespace ministl
{

//---------------------一级空间配置器-------------------------------------
typedef void(*malloc_alloc_oom_handler)();
template <int inst>
class malloc_alloc_template {
private:
    // 以下都是用来处理内存不够的情况
    static void* oom_malloc(size_t) {
        void (*my_malloc_handler)();
        void* result;
        for (;;) {
            // 不断尝试释放、配置、再释放、再配置
            my_malloc_handler = m_malloc_alloc_oom_handler;
            if (0 == my_malloc_handler) { throw std::bad_alloc(); }
            my_malloc_handler();//调用处理例程、企图释放释放内存
            result = malloc(n);     //再次尝试配置内存
            if (result) 
                return result;
        }
    }
    static void* oom_realloc(void*, size_t) {
        void (*my_malloc_handler)();
        void* result;
        for (;;) { //不断尝试释放、配置、再释放、再配置。。。
            my_malloc_handler = m_malloc_alloc_oom_handler;
            if (0 == my_malloc_handler) { throw std::bad_alloc(); }
            my_malloc_handler(); //调用处理例程、企图释放释放内存
            result = realloc(p, n); //再次尝试配置内存
            if (result) 
                return(result);
        }
    }
    // 设定一个函数void (*f)()，由下面的参数来接收，当不能开辟相应大小的内存的时候，设定好的函数就会一直调用
    static malloc_alloc_oom_handler m_malloc_alloc_oom_handler;
public:
    static void* allocate(size_t n) {
        void* result = malloc(n); //第一级配置器直接使用malloc()
        if (0 == result) result = oom_malloc(n);
        return result;
    }

    static void deallocate(void* p, size_t) {
        free(p);
    }

    static void* reallocate(void* p,size_t new_sz) {
        void* result = realloc(p, new_sz); //第一级配置器直接使用 realloc()
        if (0 == result) result = oom_realloc(p, new_sz);
        return result;
    }


    /*
    *我们知道当operator new无法满足某一个内存分配的需求的时候，它会抛出一个异常，在这之前，它会先调用一个客
    *户制定的错误处理函数new_handler。为了指定这个来处理内存不足的函数，使用者需要调用标准库程序函数
    set_new_handler
    */
    malloc_alloc_oom_handler set_malloc_handler(void (*f)()) {
        malloc_alloc_oom_handler old = m_malloc_alloc_oom_handler;
        m_malloc_alloc_oom_handler = f;
        return old;
    }
    
};
malloc_alloc_oom_handler malloc_alloc_template<0>::m_malloc_alloc_oom_handler = nullptr;

// -------------------------------二级空间配置器----------------------------
//一个自由链表，用于维护管理内存碎片， 分配回收小内存
union FreeList
{
    union FreeList* next;  // 指向下一个区块
    char data[1];          // 储存本块内存的首地址
};

//不同的内存范围调整上界
enum {
    EAlign128 = 8,
    EAlign256 = 16,
    EAlign512 = 32,
    EAlign1024 = 64,
    EAlign2048 = 128,
    EAlign4096 = 256
};
//小对象的内存大小
enum {
    ESmallObjectMemory = 4096
};

//free-tree 个数
enum { 
    EfreetreeNumber = 56
};


//=====================================二级空间配置器=====================================================
//以下为2级空间配置器：
//当内存大于4096B时，直接调用1级空间配置器
//当内存小于4096B时，使用内存池的管理方式来管理这些空间
class alloc
{
private:
    static char* start_free;                      // 内存池起始位置
    static char* end_free;                        // 内存池结束位置
    static size_t heap_size;                       // 申请heap空间附加值大小

    static FreeList* free_list[EfreetreeNumber];  // 自由链表

public:
    static void* allocate(size_t n); //分配内存
    static void  deallocate(void* p, size_t n); // 销毁内存
    static void* reallocate(void* p, size_t old_size, size_t new_size); //增加内存

private:
    static size_t align(size_t bytes);//上调大小
    static size_t round_up(size_t bytes);//上调区间大小
    static size_t freelist_index(size_t bytes);//返回对应结点在free-list中拿内存的位置
    static void* refill(size_t n);//对于小额内存的申请
    static char* chunk_alloc(size_t size, size_t& nobj);//内存池，当free list内存使用完毕后从内存池中获取内存
};
char* alloc::start_free = nullptr;
char* alloc::end_free = nullptr;
size_t alloc::heap_size = 0;
FreeList* alloc::free_list[EfreetreeNumber] = {
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
};

inline
void* alloc::allocate(size_t n) {
    FreeList* m_free_list;
    FreeList* result;
    //如果大于最大对象的尺寸，就直接调用malloc（这里应该是调用一级空间配置器的，但是因为还没写好就不调用了）
    if (n > static_cast<size_t>(ESmallObjectMemory)) {
        return malloc(n);
    }
    // 获取当前对象应该从链表中获得的结点。
    m_free_list = free_list[freelist_index(n)];
    result = m_free_list;
    if (result == nullptr) {
        void* tmp = refill(freelist_index(n));
        return tmp;
    }
    m_free_list = result->next;
    return result;
}

inline
void alloc::deallocate(void* ptr, size_t n) {
    FreeList* q = (FreeList*)ptr;
    FreeList* m_free_list;

    //大于4096就调用一级空间配置器
    if (n > (size_t)ESmallObjectMemory) {
        malloc_alloc_template<0>::deallocate(ptr, n);
        return;
    }
    //寻找对应的free list 然后回收这片区域
    m_free_list = free_list[freelist_index(n)];
    q->next = m_free_list;
    m_free_list = q;
}

inline
void* alloc::reallocate(void* p, size_t old_size, size_t new_size) {
    FreeList* q = (FreeList*)p;
    FreeList* m_free_list;

    if (new_size > (size_t)ESmallObjectMemory && old_size > (size_t)ESmallObjectMemory) {
        return malloc_alloc_template<0>::reallocate(p, new_size);
    }
    if (old_size == new_size) {
        return p;
    }


    FreeList* result = (FreeList*)allocate(new_size);
    size_t copy_sz = new_size > old_size ? old_size : new_size;
    std::memcpy(result, p, copy_sz);
    deallocate(p, old_size);
    return result;


}

inline size_t alloc::align(size_t bytes) {
    if (bytes <= 512)
    {
        return bytes <= 256
            ? bytes <= 128 ? EAlign128 : EAlign256
            : EAlign512;
    }
    return bytes <= 2048
        ? bytes <= 1024 ? EAlign1024 : EAlign2048
        : EAlign4096;
}
inline size_t alloc::round_up(size_t bytes) {
    return ((bytes + EAlign128 - 1) & ~(EAlign128 - 1));
}
inline size_t alloc::freelist_index(size_t bytes) {
    if (bytes <= 512)
    {
        return bytes <= 256
            ? bytes <= 128
            ? ((bytes + EAlign128 - 1) / EAlign128 - 1)
            : (15 + (bytes + EAlign256 - 129) / EAlign256)
            : (23 + (bytes + EAlign512 - 257) / EAlign512);
    }
    return bytes <= 2048
        ? bytes <= 1024
        ? (31 + (bytes + EAlign1024 - 513) / EAlign1024)
        : (39 + (bytes + EAlign2048 - 1025) / EAlign2048)
        : (47 + (bytes + EAlign4096 - 2049) / EAlign4096);
}

inline void* alloc::refill(size_t n) {
    size_t nobjs = 20;
    //调用chunk_alloc()函数尝试获取nobjs个区块作为free-list的新结点
    //注意objs是以引用的方式传入的。
    char* chunk = chunk_alloc(n, nobjs);
    FreeList *m_free_list, *result, *current, *next;
    //如果只获得一个区块就分给调用者用，free list不添加新结点
    if (1 == nobjs) return chunk;
    //获得多于一个结点的时候调整free list，纳入新结点
    m_free_list = free_list[freelist_index(n)];
    result = reinterpret_cast<FreeList*>(chunk);
    //取自内存池的指针串到free_list上
    m_free_list = next = reinterpret_cast<FreeList*>(chunk + n);
    for (size_t i = 1;; ++i) {
        current = next;
        //将一个个的Freetree结点挂在free tree上去
        next = (FreeList*)((char*)next + n);
        //到达已经分配出去的那个结点之前后跳出 否则一直串结点
        if (nobjs - 1 == i) {
            current->next = nullptr;
            break;
        }
        else {
            current->next = next;
        }
    }
    return result;
}
//内存池
inline char* alloc::chunk_alloc(size_t size, size_t& nobjs) {
    char* result;
    // 需求量
    size_t total_b = size * nobjs;
    // 内存池剩余
    size_t pool_c = end_free - start_free; 
    // 剩余完全满足需求
    if (pool_c >= total_b) {
        result = start_free;
        //内存池缩小
        start_free += total_b;
        return result;
    }
    //不能完全满足需求但是够1以上20以下的大小
    else if (pool_c >= size){
        //实际能开辟的块数 在我们refill中需要以此来判断我们获得块数并将其串联到free list上
        nobjs = pool_c / size;
        total_b = size * nobjs;
        result = start_free;
        //更新内存池中的容量 其实这种情况下内存池当前容量也就为空了
        start_free += total_b;
        return result;
    }
    else {
        //此时内存池完全没办法了 一个区块都无法提供了
        size_t linger_bytes = 2 * total_b + round_up(heap_size >> 4);
        //看看是否还存在零碎的内存可以使用
        if (pool_c > 0) {
            //那就是还有零头 此时不管内存池啥情况了 先给申请端把内存分了再说
            FreeList* m_free_list = free_list[freelist_index(pool_c)];
            //调整free list，将内存池中仅剩的一点点内存给了先
            ((FreeList*)start_free)->next = m_free_list;
            m_free_list = (FreeList*)start_free;
        }
        
        //然后我们对内存池进行扩充
        start_free = (char*)malloc(linger_bytes);
        if (start_free == 0) {
            //堆空间都用完了
            FreeList* m_free_list, * p;
            //寻找看看还有没有Free list有剩余的空间，且这片空间大小也足够
            //i就是我们需要的空间大小，每次循环后跳一个调整后的空间大小
            for (size_t i = size; i <= ESmallObjectMemory; i += align(i))
            {
                m_free_list = free_list[freelist_index(i)];
                p = m_free_list;
                if (p)
                {
                    m_free_list = p->next;
                    start_free = (char*)p;
                    end_free = start_free + i;
                    return chunk_alloc(size, nobjs);
                }
            } 
            //此时已经没办法了 只能看看一级空间配置器是否还能起到作用了 还不行的话只能抛出异常
            end_free = nullptr;
            start_free = (char*)malloc_alloc_template<0>::allocate(linger_bytes);
        }

        //接下来就是我们能开辟足够的空间
        heap_size += linger_bytes;
        end_free = start_free + linger_bytes;
        return chunk_alloc(size, nobjs);
    }
}

// ======================================封装好的供外界使用的接口=================================
template<class T, class Alloc>
class simple_alloc {
public:
    static T* allocate(size_t n) {
        return 0 == n ? 0 : static_cast<T*>(Alloc::allocate(n * sizeof(T)));
    }
    static T* allocate(void) {
        return static_cast<T*>(Alloc::allocate(sizeof(T)));
    }
    static void deallocate(T* p, size_t n) {
        if (0 != n) Alloc::deallocate(p, n * sizeof(T));
    }
    static void deallocate(T* p) {
        Alloc::deallocate(p, sizeof(T));
    }
};

}
#endif

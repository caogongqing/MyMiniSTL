#ifndef __MYMINISTL_ALLOC_H__
#define __MYMINISTL_ALLOC_H__

#include <new>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include "util.h"
/*
�����ڴ���̣�

����1�����������ڴ�ռ����128bytes, ���ɵ�һ������������

����2�����������Ƚ������ڴ�Ĵ�С�ϵ���8�ı���n��������n�ҳ����Ӧ�Ŀ��������ַ__my_free_list

����3������ÿ����������п��õĿ��п飬�򽫴˿��п鷵�ز�����__my_free_list������ת��4��

����4������һ����˵��__my_free_list��û�п��п�����ˣ��������ᰴ������Ĳ��账��

����    a) ���ŵ���_s_chunk_alloc()�����СΪn*20���ڴ�ռ䣬ע����ǣ���ʱ��һ�������뵽n*20��С���ڴ�ռ�

        b) ���ֻ���뵽��СΪn���ڴ�ռ䣬�򷵻ظ��û�������c)

        c) �����뵽��n*x(a��˵�ˣ���һ����n*20)�ڴ��ȡ��һ�����ظ��û���������ڴ��������������__my_free_list��

_s_chunk_alloc()�ľ������Ϊ��

    1)���_s_start_free��_s_end_free֮��Ŀռ��㹻����n*20��С���ڴ�ռ䣬�������ռ���ȡ��n*20��С���ڴ�ռ䣬
    ����_s_start_free���������뵽���ڴ�ռ����ʼ��ַ������ת��2)

    2) ���_s_start_free��_s_end_free֮��Ŀռ��㹻�������n���ڴ�ռ䣬�������������n���ڴ�ռ䣬����_s_start_free��
    ��nobj����������������������뵽���ڴ�ռ����ʼ��ַ������ת��3��

����3) ����һ����˵���ڴ������һ���СΪn���ڴ涼û���ˣ���ʱ����ڴ���л���һЩ�ڴ棨��Щ�ڴ��С�϶�С��n��������Щ
  �ڴ���뵽���Ӧ��С�Ŀ��з�������

����4) ����malloc������ʱ�������СΪ��2*n*20 + �����������ڴ�ռ䣬 �������ɹ�������_s_start_free, _s_end_free��_s_heap_size��
  �����µ���_s_chunk_alloc()������ת��5)

 �� 5) ����һ����˵��4)�е���mallocʧ�ܣ���ʱ���������α���16�����з�������ֻҪ��һ�������������ͷŸ����е�һ���ڵ㣬
   ���µ���_s_chunk_alloc()

�ڴ��ͷŹ��̣�
    �ڴ���ͷŹ��̱Ƚϼ򵥣�����������������һ����ָ��Ҫ�ͷŵ��ڴ���ָ��p������һ����ʾҪ�ͷŵ��ڴ��Ĵ�Сn����������
���ж�n�����n>128bytes�����ɵ�һ��������ȥ�������򽫸��ڴ��ӵ���Ӧ�Ŀ��������С�

*/


namespace ministl
{

//---------------------һ���ռ�������-------------------------------------
typedef void(*malloc_alloc_oom_handler)();
template <int inst>
class malloc_alloc_template {
private:
    // ���¶������������ڴ治�������
    static void* oom_malloc(size_t) {
        void (*my_malloc_handler)();
        void* result;
        for (;;) {
            // ���ϳ����ͷš����á����ͷš�������
            my_malloc_handler = m_malloc_alloc_oom_handler;
            if (0 == my_malloc_handler) { throw std::bad_alloc(); }
            my_malloc_handler();//���ô������̡���ͼ�ͷ��ͷ��ڴ�
            result = malloc(n);     //�ٴγ��������ڴ�
            if (result) 
                return result;
        }
    }
    static void* oom_realloc(void*, size_t) {
        void (*my_malloc_handler)();
        void* result;
        for (;;) { //���ϳ����ͷš����á����ͷš������á�����
            my_malloc_handler = m_malloc_alloc_oom_handler;
            if (0 == my_malloc_handler) { throw std::bad_alloc(); }
            my_malloc_handler(); //���ô������̡���ͼ�ͷ��ͷ��ڴ�
            result = realloc(p, n); //�ٴγ��������ڴ�
            if (result) 
                return(result);
        }
    }
    // �趨һ������void (*f)()��������Ĳ��������գ������ܿ�����Ӧ��С���ڴ��ʱ���趨�õĺ����ͻ�һֱ����
    static malloc_alloc_oom_handler m_malloc_alloc_oom_handler;
public:
    static void* allocate(size_t n) {
        void* result = malloc(n); //��һ��������ֱ��ʹ��malloc()
        if (0 == result) result = oom_malloc(n);
        return result;
    }

    static void deallocate(void* p, size_t) {
        free(p);
    }

    static void* reallocate(void* p,size_t new_sz) {
        void* result = realloc(p, new_sz); //��һ��������ֱ��ʹ�� realloc()
        if (0 == result) result = oom_realloc(p, new_sz);
        return result;
    }


    /*
    *����֪����operator new�޷�����ĳһ���ڴ����������ʱ�������׳�һ���쳣������֮ǰ�������ȵ���һ����
    *���ƶ��Ĵ�������new_handler��Ϊ��ָ������������ڴ治��ĺ�����ʹ������Ҫ���ñ�׼�������
    set_new_handler
    */
    malloc_alloc_oom_handler set_malloc_handler(void (*f)()) {
        malloc_alloc_oom_handler old = m_malloc_alloc_oom_handler;
        m_malloc_alloc_oom_handler = f;
        return old;
    }
    
};
malloc_alloc_oom_handler malloc_alloc_template<0>::m_malloc_alloc_oom_handler = nullptr;

// -------------------------------�����ռ�������----------------------------
//һ��������������ά�������ڴ���Ƭ�� �������С�ڴ�
union FreeList
{
    union FreeList* next;  // ָ����һ������
    char data[1];          // ���汾���ڴ���׵�ַ
};

//��ͬ���ڴ淶Χ�����Ͻ�
enum {
    EAlign128 = 8,
    EAlign256 = 16,
    EAlign512 = 32,
    EAlign1024 = 64,
    EAlign2048 = 128,
    EAlign4096 = 256
};
//С������ڴ��С
enum {
    ESmallObjectMemory = 4096
};

//free-tree ����
enum { 
    EfreetreeNumber = 56
};


//=====================================�����ռ�������=====================================================
//����Ϊ2���ռ���������
//���ڴ����4096Bʱ��ֱ�ӵ���1���ռ�������
//���ڴ�С��4096Bʱ��ʹ���ڴ�صĹ���ʽ��������Щ�ռ�
class alloc
{
private:
    static char* start_free;                      // �ڴ����ʼλ��
    static char* end_free;                        // �ڴ�ؽ���λ��
    static size_t heap_size;                       // ����heap�ռ丽��ֵ��С

    static FreeList* free_list[EfreetreeNumber];  // ��������

public:
    static void* allocate(size_t n); //�����ڴ�
    static void  deallocate(void* p, size_t n); // �����ڴ�
    static void* reallocate(void* p, size_t old_size, size_t new_size); //�����ڴ�

private:
    static size_t align(size_t bytes);//�ϵ���С
    static size_t round_up(size_t bytes);//�ϵ������С
    static size_t freelist_index(size_t bytes);//���ض�Ӧ�����free-list�����ڴ��λ��
    static void* refill(size_t n);//����С���ڴ������
    static char* chunk_alloc(size_t size, size_t& nobj);//�ڴ�أ���free list�ڴ�ʹ����Ϻ���ڴ���л�ȡ�ڴ�
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
    //�������������ĳߴ磬��ֱ�ӵ���malloc������Ӧ���ǵ���һ���ռ��������ģ�������Ϊ��ûд�þͲ������ˣ�
    if (n > static_cast<size_t>(ESmallObjectMemory)) {
        return malloc(n);
    }
    // ��ȡ��ǰ����Ӧ�ô������л�õĽ�㡣
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

    //����4096�͵���һ���ռ�������
    if (n > (size_t)ESmallObjectMemory) {
        malloc_alloc_template<0>::deallocate(ptr, n);
        return;
    }
    //Ѱ�Ҷ�Ӧ��free list Ȼ�������Ƭ����
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
    //����chunk_alloc()�������Ի�ȡnobjs��������Ϊfree-list���½��
    //ע��objs�������õķ�ʽ����ġ�
    char* chunk = chunk_alloc(n, nobjs);
    FreeList *m_free_list, *result, *current, *next;
    //���ֻ���һ������ͷָ��������ã�free list������½��
    if (1 == nobjs) return chunk;
    //��ö���һ������ʱ�����free list�������½��
    m_free_list = free_list[freelist_index(n)];
    result = reinterpret_cast<FreeList*>(chunk);
    //ȡ���ڴ�ص�ָ�봮��free_list��
    m_free_list = next = reinterpret_cast<FreeList*>(chunk + n);
    for (size_t i = 1;; ++i) {
        current = next;
        //��һ������Freetree������free tree��ȥ
        next = (FreeList*)((char*)next + n);
        //�����Ѿ������ȥ���Ǹ����֮ǰ������ ����һֱ�����
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
//�ڴ��
inline char* alloc::chunk_alloc(size_t size, size_t& nobjs) {
    char* result;
    // ������
    size_t total_b = size * nobjs;
    // �ڴ��ʣ��
    size_t pool_c = end_free - start_free; 
    // ʣ����ȫ��������
    if (pool_c >= total_b) {
        result = start_free;
        //�ڴ����С
        start_free += total_b;
        return result;
    }
    //������ȫ���������ǹ�1����20���µĴ�С
    else if (pool_c >= size){
        //ʵ���ܿ��ٵĿ��� ������refill����Ҫ�Դ����ж����ǻ�ÿ��������䴮����free list��
        nobjs = pool_c / size;
        total_b = size * nobjs;
        result = start_free;
        //�����ڴ���е����� ��ʵ����������ڴ�ص�ǰ����Ҳ��Ϊ����
        start_free += total_b;
        return result;
    }
    else {
        //��ʱ�ڴ����ȫû�취�� һ�����鶼�޷��ṩ��
        size_t linger_bytes = 2 * total_b + round_up(heap_size >> 4);
        //�����Ƿ񻹴���������ڴ����ʹ��
        if (pool_c > 0) {
            //�Ǿ��ǻ�����ͷ ��ʱ�����ڴ��ɶ����� �ȸ�����˰��ڴ������˵
            FreeList* m_free_list = free_list[freelist_index(pool_c)];
            //����free list�����ڴ���н�ʣ��һ����ڴ������
            ((FreeList*)start_free)->next = m_free_list;
            m_free_list = (FreeList*)start_free;
        }
        
        //Ȼ�����Ƕ��ڴ�ؽ�������
        start_free = (char*)malloc(linger_bytes);
        if (start_free == 0) {
            //�ѿռ䶼������
            FreeList* m_free_list, * p;
            //Ѱ�ҿ�������û��Free list��ʣ��Ŀռ䣬����Ƭ�ռ��СҲ�㹻
            //i����������Ҫ�Ŀռ��С��ÿ��ѭ������һ��������Ŀռ��С
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
            //��ʱ�Ѿ�û�취�� ֻ�ܿ���һ���ռ��������Ƿ����������� �����еĻ�ֻ���׳��쳣
            end_free = nullptr;
            start_free = (char*)malloc_alloc_template<0>::allocate(linger_bytes);
        }

        //���������������ܿ����㹻�Ŀռ�
        heap_size += linger_bytes;
        end_free = start_free + linger_bytes;
        return chunk_alloc(size, nobjs);
    }
}

// ======================================��װ�õĹ����ʹ�õĽӿ�=================================
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

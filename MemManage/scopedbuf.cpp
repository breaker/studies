// scopedbuf.cpp
//
// scoped �ͻ�����
//

#include <exception>

////////////////////////////////////////////////////////////////////////////////
// CRT �ڴ������ class Malloc
////////////////////////////////////////////////////////////////////////////////

// CRT malloc() �ķ�װ��

template <class Type, bool ThrowExcept = true>
class Malloc
{
public:
    typedef Type*   TypePtr;
    typedef Type&   TypeRef;

    Malloc() {}

    // sz �� Type ����, ������ byte ��
    TypePtr allocate(size_t sz)
    {
        TypePtr p = (TypePtr) ::malloc(sz * sizeof(Type));
        if (ThrowExcept && p == NULL)
            throw std::bad_alloc();
        return p;
    }

    TypePtr reallocate(TypePtr ptr, size_t sz)
    {
        TypePtr p = (TypePtr) ::realloc(ptr, sz * sizeof(Type));
        if (ThrowExcept && p == NULL && sz != 0)
            throw std::bad_alloc();
        return p;
    }

    void deallocate(TypePtr ptr)
    {
        ::free(ptr);
    }

    // �� ptr ������ val ֵ, ����ʽ new
    void construct(TypePtr ptr, const Type& val)
    {
        new(ptr) Type(val);
    }

    // ���� Type ����, �����ͷŶ�����Ĵ洢
    void destroy(TypePtr ptr)
    {
        ptr->~Type();
    }
};

////////////////////////////////////////////////////////////////////////////////
// scoped �ͻ����� class ScopedBuf
////////////////////////////////////////////////////////////////////////////////

// scoped �ͻ����� class ScopedBuf (monopolize ����Ȩ)

template <class Type, class Alloc = Malloc<Type, true> >
class ScopedBuf
{
private:
    Type*   m_pbuf;
    size_t  m_size;
    Alloc   m_alloc;

private:
    // forbid copy
    ScopedBuf(const ScopedBuf&);
    ScopedBuf& operator=(const ScopedBuf&);

public:
    ScopedBuf(Alloc ac = Alloc()) : m_pbuf(NULL), m_size(0), m_alloc(ac) {}

    // sz �� Type ����, ������ byte ��
    ScopedBuf(size_t sz, Alloc ac = Alloc()) : m_alloc(ac)
    {
        allocate(sz);
    }

    ScopedBuf(size_t sz, const Type& val, Alloc ac = Alloc()) : m_alloc(ac)
    {
        allocate(sz);
        for (size_t i = 0; i < sz; ++i)
            ac.construct(m_pbuf + i, val);
    }

    ~ScopedBuf()
    {
        for (size_t i = 0; i < m_size; ++i)
            m_alloc.destroy(m_pbuf + i);
        deallocate();
    }

public:
    // �����ڴ�
    template <bool Check>
    void allocate(size_t sz)
    {
        if (Check && m_pbuf != NULL)
            deallocate();
        m_size = sz;
        m_pbuf = m_alloc.allocate(sz);
    }

    void allocate(size_t sz)
    {
        return allocate<false>(sz);
    }

    void allocate_check(size_t sz)
    {
        return allocate<true>(sz);
    }

    // ���·����ڴ�
    void reallocate(size_t sz)
    {
        m_size = sz;
        m_pbuf = m_alloc.reallocate(m_pbuf, sz);
    }

    // ��չ�ڴ�
    void expand(double ratio)
    {
        return reallocate(m_size + (size_t) (m_size * ratio));
    }
    void expand()
    {
        return reallocate(m_size + (m_size >> 1));
    }

    // �ͷ��ڴ�
    void deallocate()
    {
        m_alloc.deallocate(m_pbuf);
        m_pbuf = NULL;
        m_size = 0;
    }

    size_t size() const
    {
        return m_size;
    }

    Type* buf()
    {
        return m_pbuf;
    }

    // �±����
    Type& operator[](size_t i)
    {
        return m_pbuf[i];
    }

    Type& at(size_t i)
    {
        if (m_pbuf == NULL)
            throw std::exception("null pointer");
        if (i >= m_size)
            throw std::exception("error range");
        return m_pbuf[i];
    }

    Type* release()
    {
        Type* p = m_pbuf;
        m_pbuf = NULL;
        m_size = 0;
        return p;
    }

    // buf �����Ƿ����� Alloc ����Ĵ洢
    void reset(Type* buf, size_t sz)
    {
        if (m_pbuf != NULL)
            m_alloc.deallocate(m_pbuf);
        m_pbuf = buf;
        m_size = sz;
    }
};

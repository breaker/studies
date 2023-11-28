// autoptr.cpp
//
// 可指定 deleter 的 auto 型智能指针: class AutoPtr, AutoArr
// 默认 deleter: StdDel, StdDelArr 来自 deleter.cpp
//

////////////////////////////////////////////////////////////////////////////////
// auto 型智能指针 class AutoPtr
////////////////////////////////////////////////////////////////////////////////

// 可指定 deleter 的 auto 型智能指针 (transfer 所有权)

template <class Type, class Del = StdDel<FALSE, FALSE> >
class AutoPtr
{
private:
    Type*   m_ptr;
    Del     m_del;

public:
    // 禁止隐式类型转换 Type* => AutoPtr
    explicit AutoPtr(Type* ptr = NULL, Del del = Del()) : m_ptr(ptr), m_del(del) {}

    // transfer 所有权的 copy ctor
    AutoPtr(AutoPtr& right) : m_ptr(right.release()) {}

    // 可转换类型 (继承层次结构) 间的 copy ctor
    template <class TR>
    AutoPtr(AutoPtr<TR>& right) : m_ptr(right.release()) {}

    ~AutoPtr()
    {
        m_del(m_ptr);
    }

    // transfer 所有权的 assign
    AutoPtr& operator=(AutoPtr& right)
    {
        if (&right != this) {
            m_del(m_ptr);
            m_ptr = right.release();
        }
        return *this;
    }

    // 可转换类型 (继承层次结构) 间的 assign
    template <class TR>
    AutoPtr& operator=(AutoPtr<TR>& right)
    {
        if (right.get() != m_ptr) {
            m_del(m_ptr);
            m_ptr = right.release();
        }
        return *this;
    }

    Type* get() const
    {
        return m_ptr;
    }

    Type* operator->() const
    {
        return m_ptr;
    }

    Type& operator*() const
    {
        return *m_ptr;
    }

    // 释放管理, 返回之前的管理对象
    Type* release()
    {
        Type* p = m_ptr;
        m_ptr = NULL;
        return p;
    }

    // 重置管理对象
    void reset(Type* ptr = NULL)
    {
        m_del(m_ptr);
        m_ptr = ptr;
    }
};

////////////////////////////////////////////////////////////////////////////////
// auto 型智能指针数组版 class AutoArr
////////////////////////////////////////////////////////////////////////////////

template <class Type, class Del = StdDelArr<FALSE, FALSE> >
class AutoArr : public AutoPtr<Type, Del>
{
public:
    explicit AutoArr(Type* ptr = NULL, Del del = Del()) : AutoPtr(ptr, del) {}

    // 不检查下标和托管对象的有效性, 以保证效率
    Type& operator[](size_t i)
    {
        return get()[i];
    }
};

// autoptr.cpp
//
// ��ָ�� deleter �� auto ������ָ��: class AutoPtr, AutoArr
// Ĭ�� deleter: StdDel, StdDelArr ���� deleter.cpp
//

////////////////////////////////////////////////////////////////////////////////
// auto ������ָ�� class AutoPtr
////////////////////////////////////////////////////////////////////////////////

// ��ָ�� deleter �� auto ������ָ�� (transfer ����Ȩ)

template <class Type, class Del = StdDel<FALSE, FALSE> >
class AutoPtr
{
private:
    Type*   m_ptr;
    Del     m_del;

public:
    // ��ֹ��ʽ����ת�� Type* => AutoPtr
    explicit AutoPtr(Type* ptr = NULL, Del del = Del()) : m_ptr(ptr), m_del(del) {}

    // transfer ����Ȩ�� copy ctor
    AutoPtr(AutoPtr& right) : m_ptr(right.release()) {}

    // ��ת������ (�̳в�νṹ) ��� copy ctor
    template <class TR>
    AutoPtr(AutoPtr<TR>& right) : m_ptr(right.release()) {}

    ~AutoPtr()
    {
        m_del(m_ptr);
    }

    // transfer ����Ȩ�� assign
    AutoPtr& operator=(AutoPtr& right)
    {
        if (&right != this) {
            m_del(m_ptr);
            m_ptr = right.release();
        }
        return *this;
    }

    // ��ת������ (�̳в�νṹ) ��� assign
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

    // �ͷŹ���, ����֮ǰ�Ĺ������
    Type* release()
    {
        Type* p = m_ptr;
        m_ptr = NULL;
        return p;
    }

    // ���ù������
    void reset(Type* ptr = NULL)
    {
        m_del(m_ptr);
        m_ptr = ptr;
    }
};

////////////////////////////////////////////////////////////////////////////////
// auto ������ָ������� class AutoArr
////////////////////////////////////////////////////////////////////////////////

template <class Type, class Del = StdDelArr<FALSE, FALSE> >
class AutoArr : public AutoPtr<Type, Del>
{
public:
    explicit AutoArr(Type* ptr = NULL, Del del = Del()) : AutoPtr(ptr, del) {}

    // ������±���йܶ������Ч��, �Ա�֤Ч��
    Type& operator[](size_t i)
    {
        return get()[i];
    }
};

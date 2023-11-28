// refcount.cpp
//
// reference counting + COW (copy on write) 简单示例
//

#include <string>
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// class Widget 的内部表示 class WidgetRep (Representation)
////////////////////////////////////////////////////////////////////////////////

class WidgetRep
{
    friend class Widget;

private:
    int         m_num;
    string      m_name;
    unsigned    m_refs;

private:
    WidgetRep(int n = 0, const char* name = _T("")) : m_num(n), m_name(name), m_refs(1) {}
};

////////////////////////////////////////////////////////////////////////////////
// class Widget
////////////////////////////////////////////////////////////////////////////////

class Widget
{
public:
    Widget(int no = 0, const char* name = _T("")) : m_prep(new WidgetRep(no, name)) {}

    Widget(const Widget& r) : m_prep(r.m_prep)
    {
        ++m_prep->m_refs;
    }

    ~Widget()
    {
        if (--m_prep->m_refs == 0)
            delete m_prep;
    }

    void init(int no, const char* name)
    {
        if (--m_prep->m_refs == 0)
            delete m_prep;
        m_prep = new WidgetRep(no, name);   // COW
    }

    Widget& operator=(const Widget& r)
    {
        if (this == &r || m_prep == r.m_prep)
            return *this;

        if (--m_prep->m_refs == 0)
            delete m_prep;
        m_prep = r.m_prep;
        ++m_prep->m_refs;
        return *this;
    }

    ostream& output(ostream& os) const
    {
        os << "{num: " << m_prep->m_num << ", name: " << m_prep->m_name << "}";
        return os;
    }

private:
    WidgetRep*  m_prep;
};

////////////////////////////////////////////////////////////////////////////////
// class Widget 上的操作
////////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const Widget& person)
{
    return person.output(os);
}

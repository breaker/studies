// string.cpp
//
// 自定义字符串类示例
// 改自 "C++ Programming Language, Special Ed" 11.12 A String Class
// 使用的技术: pimpl, reference counting, COW (copy on write), proxy class
//

namespace my
{

////////////////////////////////////////////////////////////////////////////////
// class String 的内部表示 class StringRep (Representation)
////////////////////////////////////////////////////////////////////////////////

class StringRep
{
    friend class String;

private:
    unsigned    m_refs;
    char*       m_pstr;
    size_t      m_size; // 字符个数, 不包括结尾 0

private:
    // forbid copy
    StringRep(const StringRep&);
    StringRep& operator=(const StringRep&);

private:
    struct Attach {};
    static const Attach Attached;

    StringRep(size_t sz, char* str, const Attach& attach) : m_refs(1), m_size(sz), m_pstr(str) {}

    StringRep(size_t sz, const char* str) : m_refs(1), m_size(sz)
    {
        m_pstr = new char[sz + 1];
        strcpy_s(m_pstr, sz + 1, str);
    }

    ~StringRep()
    {
        delete[] m_pstr;
    }

    // 意思是 get own OR copy, 根据 m_refs == 1, 得到本体或其拷贝
    StringRep* get_own_copy()
    {
        if (m_refs == 1)
            return this;
        --m_refs;
        return new StringRep(m_size, m_pstr);
    }

    void assign(size_t sz, const char* str)
    {
        if (sz == m_size) {  // sz == m_size 时, 复用缓冲区 m_pstr
            strcpy_s(m_pstr, sz + 1, str);
            return;
        }
        m_pstr = alloc<TRUE, TRUE>(sz + 1);
        m_size = sz;
    }

    void append(size_t sz, const char* str)
    {
        if (sz == 0)
            return;
        size_t newsz = m_size + sz;
        m_pstr = alloc<TRUE, TRUE>(newsz + 1);
        strcpy_s(m_pstr + m_size, sz + 1, str);
        m_size = newsz;
    }

    StringRep* copy_append(size_t sz, const char* str)
    {
        if (sz == 0)
            return this;
        size_t newsz = m_size + sz;
        char* newstr = alloc<TRUE, FALSE>(newsz + 1);
        strcpy_s(newstr + m_size, sz + 1, str);
        return new StringRep(newsz, newstr, Attached);
    }

    template <BOOL CopyOld, BOOL DelOld>
    char* alloc(size_t sz)
    {
        char* newstr = new char[sz];
        if (CopyOld)
            strcpy_s(newstr, sz, m_pstr);
        if (DelOld)
            delete[] m_pstr;
        return newstr;
    }
};

const StringRep::Attach StringRep::Attached;

////////////////////////////////////////////////////////////////////////////////
// char& 的 proxy class CharRef
////////////////////////////////////////////////////////////////////////////////

// class CharRef 的用途:
// 用以区分 String::operator[] 的读 右值使用 (r-value usage) 和写 左值使用 (l-value usage)

class CharRef
{
    friend class String;

private:
    String& m_str;
    size_t  m_idx;

private:
    CharRef(String& str, size_t i) : m_str(str), m_idx(i) {}

public:
    // 读字符, 右值使用 r-value usage
    operator char() const
    {
        return m_pstr.read(m_idx);
    }

    // 写字符, 左值使用 l-value usage
    void operator=(char c)
    {
        m_pstr.write(m_idx, c);
    }
};

////////////////////////////////////////////////////////////////////////////////
// 字符串类 String
////////////////////////////////////////////////////////////////////////////////

class String
{
private:
    StringRep* m_prep;

public:
    String() : m_prep(new StringRep(0, "")) {}

    String(const char* str) : m_prep(new StringRep(strlen(str), str)) {}

    String(const char* str1, const char* str2)
    {
        size_t sz1 = strlen(str1);
        size_t sz2 = strlen(str2);
        char* newstr = new char[sz1 + sz2 + 1];
        strcpy_s(newstr, sz1 + 1, str1);
        strcpy_s(newstr + sz1, sz2 + 1, str2);
        m_prep = new StringRep(sz1 + sz2, newstr, StringRep::Attached);
    }

    String(const String& r) : m_prep(r.m_prep)
    {
        r.m_prep->m_refs++;
    }

    ~String()
    {
        if (--m_prep->m_refs == 0)
            delete m_prep;
    }

    String& operator=(const char* str)
    {
        if (m_prep->m_refs == 1)
            m_prep->assign(strlen(str), str);         // 重用 StringRep
        else {
            m_prep->m_refs--;
            m_prep = new StringRep(strlen(str), str);    // 拷贝 StringRep
        }
        return *this;
    }

    String& operator=(const String& r)
    {
        r.m_prep->m_refs++;  // 先增加计数, 防止自赋值引起下句释放 StringRep
        if (--m_prep->m_refs == 0)
            delete m_prep;
        m_prep = r.m_prep;
        return *this;
    }

    char read(size_t i) const
    {
        return m_prep->m_pstr[i];
    }

    void write(size_t i, char c)
    {
        m_prep = m_prep->get_own_copy();
        m_prep->m_pstr[i] = c;
    }

    // 左值使用的 [], 通过 CharRef::operator= 作用
    // 右值使用的 [], 通过 CharRef::operator char 作用
    CharRef operator[](size_t i)
    {
        return CharRef(*this, i);
    }

    // 右值使用的 [], 本体 const
    char operator[](size_t i) const
    {
        return m_prep->m_pstr[i];
    }

    // 左值和右值使用的 at, 带范围检查
    CharRef at(size_t i)
    {
        check(i);
        return CharRef(*this, i);
    }

    // 右值使用的 at, 带范围检查, 本体 const
    char at(size_t i) const
    {
        check(i);
        return m_prep->m_pstr[i];
    }

    size_t size() const
    {
        return m_prep->m_size;
    }

    void check(size_t i) const
    {
        if (i >= m_prep->m_size)
            throw std::out_of_range("index in String is out of range");
    }

    std::ostream& output(std::ostream& os) const
    {
        os << m_prep->m_pstr;
        return os;
    }

    const char* c_str() const
    {
        return m_prep->m_pstr;
    }

    String& operator+=(const char* str)
    {
        if (m_prep->m_refs == 1)
            m_prep->append(strlen(str), str);             // 重用 StringRep
        else {
            m_prep->m_refs--;
            m_prep = m_prep->copy_append(strlen(str), str); // 拷贝 StringRep
        }
        return *this;
    }

    String& operator+=(const String& r)
    {
        return ((*this) += r.m_prep->m_pstr);
    }
};

////////////////////////////////////////////////////////////////////////////////
// class String 上的操作
////////////////////////////////////////////////////////////////////////////////

inline std::ostream& operator<<(std::ostream& os, const String& str)
{
    return str.output(os);
}

inline BOOL operator==(const String& l, const String& r)
{
    return !strcmp(l.c_str(), r.c_str());
}

inline BOOL operator!=(const String& l, const String& r)
{
    return strcmp(l.c_str(), r.c_str());
}

inline const String operator+(const String& l, const String& r)
{
    return String(l.c_str(), r.c_str());    // RVO
}

}   // namespace my

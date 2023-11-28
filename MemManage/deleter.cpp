// deleter.cpp
//
// 一组删除器 deleter
// 可配置 Check 检查是已释放 和 Zero 释放后置零
// 释放操作包括 delete, delete[], free, Release (COM), CloseHandle, fclose
//

////////////////////////////////////////////////////////////////////////////////
// 删除器 deleter
////////////////////////////////////////////////////////////////////////////////

// 标准 delete 释放内存
template <bool Check = false, bool Zero = false>
struct StdDel {
    template <class Type>
    void operator()(Type*& ptr)
    {
        if (!Check || ptr != NULL) {
            ::delete ptr;
            if (Zero)
                ptr = NULL;
        }
    }
};

const StdDel<false, false>  STDDEL;
const StdDel<false, true>   STDDEL_Z;
const StdDel<true, false>   STDDEL_C;
const StdDel<true, true>    STDDEL_CZ;

// 标准 delete[] 释放内存
template <bool Check = false, bool Zero = false>
struct StdDelArr {
    template <class Type>
    void operator()(Type*& ptr)
    {
        if (!Check || ptr != NULL) {
            ::delete[] ptr;
            if (Zero)
                ptr = NULL;
        }
    }
};

const StdDelArr<false, false>   STDDELARR;
const StdDelArr<false, true>    STDDELARR_Z;
const StdDelArr<true, false>    STDDELARR_C;
const StdDelArr<true, true>     STDDELARR_CZ;

// 标准 free 释放内存
template <bool Check = false, bool Zero = false>
struct StdFree {
    template <class Type>
    void operator()(Type*& ptr)
    {
        if (!Check || ptr != NULL) {
            ::free(ptr);
            if (Zero)
                ptr = NULL;
        }
    }
};

const StdFree<false, false> STDFREE;
const StdFree<false, true>  STDFREE_Z;
const StdFree<true, false>  STDFREE_C;
const StdFree<true, true>   STDFREE_CZ;

// 释放 COM 对象
template <bool Check = false, bool Zero = false>
struct COMRel {
    template <class Type>
    void operator()(Type*& ptr)
    {
        if (!Check || ptr != NULL) {
            ptr->Release();
            if (Zero)
                ptr = NULL;
        }
    }
};

const COMRel<false, false>  COMREL;
const COMRel<false, true>   COMREL_Z;
const COMRel<true, false>   COMREL_C;
const COMRel<true, true>    COMREL_CZ;

// 关闭 Windows 句柄
template <bool Check = false, bool Zero = false>
struct HClose {
    template <class Type>
    void operator()(Type& h)
    {
        if (!Check || h != NULL) {
            ::CloseHandle(h);
            if (Zero)
                h = NULL;
        }
    }
};

const HClose<false, false>  HCLOSE;
const HClose<false, true>   HCLOSE_Z;
const HClose<true, false>   HCLOSE_C;
const HClose<true, true>    HCLOSE_CZ;

// 关闭 FILE 指针
template <bool Check = false, bool Zero = false>
struct FClose {
    void operator()(FILE*& fp)
    {
        if (!Check || fp != NULL) {
            ::fclose(fp);
            if (Zero)
                fp = NULL;
        }
    }
};

const FClose<false, false>  FCLOSE;
const FClose<false, true>   FCLOSE_Z;
const FClose<true, false>   FCLOSE_C;
const FClose<true, true>    FCLOSE_CZ;

// 删除器函数
// 使用示例: Delete(obj, STDDEL_CZ)
template <class Type, class Del>
inline void Delete(Type& obj, Del del)
{
    del(obj);
}

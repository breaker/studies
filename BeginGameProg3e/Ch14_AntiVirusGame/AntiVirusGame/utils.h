// utils.h
//

// C Runtime Library (CRT)
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <tchar.h>
#include <locale.h>

// C++ Standard Library (STL)
#include <new>
#include <string>

namespace bzu {

////////////////////////////////////////////////////////////////////////////////
// MBCS/Unicode String Converters
////////////////////////////////////////////////////////////////////////////////

// CRT 函数 mbstowcs_s 的封装
// 将 MBCS 字符串 mbstr 转换为 wchar_t (UTF-16) 字符串 wcstr
//
// 参数:
// mbstr        需要转换的 MBCS 字符串
// wcstr [out]  存储转换为 wchar_t (UTF-16) 的字符串的输出缓冲区
// sizeWcstr    输出缓冲区 wcstr 的大小, 以 wchar_t 为单位
// loc          指定和 mbstr 的 MBCS 字符编码对应的 locale, 当 loc = 0 时, 使用全局 locale
// numConverted [out]   当 wcstr != 0 时, 表示转换了多少字符 (含 \0), 以 wchar_t 为单位
//                      当 wcstr = 0 且 sizeWcstr = 0 时, 表示转换整个 mbstr 时 (忽略 num),
//                      需要的输出缓冲区 wcstr 的大小 sizeWcstr, 以 wchar_t 为单位 (含 \0)
// num  mbstr 中需要转换的最大字符个数 (不含 \0), 以 MBCS 字符为单位
//      指定 _TRUNCATE 表示尝试转换整个 mbstr, 直到 mbstr 的
//      \0 结尾, 或者因为输出缓冲区 wcstr 太小, mbstr 中字符串被截断
//
// 返回值:
// 0        成功
// EINVAL   无效的参数错误
// EILSEQ   字符映射错误: 当前 locale 下, mbstr 中含有无效的 MBCS 字符, 无法转换为 wchar_t 字符
// STRUNCATE    输出缓冲区 wcstr 太小, 字符串 mbstr 被截断, 部分被转换
//
// 注意:
// 当 loc = 0 时, 需要设置合适的全局 locale, 才能转换出正确的 wchar_t 字符, 例如:
// 字符串 mbstr = "abc汉字", 如果全局 locale 为程序启动时的默认 C locale, 则只
// 有 ASCII 字符 "abc" 被正确转换, 而 "汉字" 转换为乱码
// 此时, 调用前应设置简体中文 locale: setlocale("chs"), 或中文系统默认 locale: setlocale("")
inline
errno_t MbsToWcs(
    const char* mbstr,
    _Out_ wchar_t* wcstr,
    size_t sizeWcstr,
    _locale_t loc = NULL,
    _Out_ size_t* numConverted = NULL,
    size_t num = _TRUNCATE)
{
    if (mbstr == NULL)
        return EINVAL;

    errno_t err;
    if (loc == NULL)
        err = mbstowcs_s(numConverted, wcstr, sizeWcstr, mbstr, num);
    else
        err = _mbstowcs_s_l(numConverted, wcstr, sizeWcstr, mbstr, num, loc);

    // 字符映射错误: 当前 locale 下, 无法将 MBCS 字符转换为 wchar_t (UTF-16) 字符
    if (numConverted != NULL && *numConverted == (size_t) -1)
        return EILSEQ;

    return err;
}

// 用 new 分配 wcstr 存储空间
// 调用 MbsToWcs, 将 MBCS 字符串 mbstr 转换为 wchar_t (UTF-16) 字符串 wcstr
// 注意:
// 用户负责释放 (delete) 用完的 wcstr 内存
inline
errno_t MbsToWcsNew(
    const char* mbstr,
    _Out_ wchar_t*& wcstr,
    _locale_t loc = NULL,
    _Out_ size_t* numConverted = NULL,
    size_t num = _TRUNCATE)
{
    // NOTE:
    // 1. mbstowcs_s 输出缓冲区参数 wcstr = 0 时, 缓冲区大小参数 sizeWcstr 必需为 0
    //    否则会报参数错误 Assert
    // 2. mbstowcs_s 得到的输出缓冲区大小是能装下整个 mbstr 转换后字符串的大小 (含 \0)
    //    以 wchar_t 为单位, 无论 num 为多少

    // 得到需要的输出缓冲区的大小
    size_t sizeWcstr;
    errno_t err = MbsToWcs(mbstr, NULL, 0, loc, &sizeWcstr);
    if (err != 0)
        return err;

    // 因为 NOTE 2 的原因, 在需要的缓冲区大小 (以 wchar_t 为单位) 和 num (需要转换的字符数) 之间选择较小的值
    // 作为分配 wcstr 缓冲区的大小, 以节省内存
    if (num != _TRUNCATE)
        sizeWcstr = min(sizeWcstr, num + 1);

    wcstr = new(std::nothrow) wchar_t[sizeWcstr];
    if (wcstr == NULL)
        return ENOMEM;

    // 实际转换 mbstr 到 wcstr
    err = MbsToWcs(mbstr, wcstr, sizeWcstr, loc, numConverted, num);
    return err;
}

// CRT 函数 wcstombs_s 的封装
// 将 wchar_t (UTF-16) 字符串 wcstr 转换为 MBCS 字符串 mbstr
// 参考 MbsToWcs 说明
//
// 参数:
// wcstr        需要转换的 wchar_t (UTF-16) 字符串
// mbstr [out]  存储转换为 MBCS 的字符串的输出缓冲区
// sizeMbstr    输出缓冲区 mbstr 的大小, 以 byte 为单位
// loc          指定转换为 MBCS 字符使用的字符集编码对应的 locale 当 loc = 0 时, 使用全局 locale
// numConverted [out]   当 mbstr != 0 时, 表示转换为 MBCS 后, 占用输出缓冲区 mbstr 多少 byte (含 \0)
//                      当 mbstr = 0 且 sizeMbstr = 0 时, 表示转换整个 wcstr 时 (忽略 num),
//                      需要的输出缓冲区 mbstr 的大小 sizeMbstr, 以 byte 为单位 (含 \0)
// num  转换后, 输出缓冲区 mbstr 中需要存储的最多 byte 数
//      指定 _TRUNCATE 表示尝试转换整个 wcstr, 直到 wcstr 的 \0 结尾
//
// 注意:
// 当 num = _TRUNCATE 时, 有两种截断含义:
// 1. 因为输出缓冲区 mbstr 太小, 只有部分 wcstr 被转换, 但已转换的字符都保持完整,
//    此时 WcsToMbs 返回 0, 即不报告这种截断, 需要用户自行判断
// 2. 当只有部分 wcstr 被转换时 (1 类型截断), 有可能导致单个非 ASCII 字符的截
//    断, 此时 WcsToMbs 返回 STRUNCATE
//    例如 UTF-16 字符串 "abc汉字" 转换 MBCS 字符串
//    如果 sizeMbstr = 5, num = _TRUNCATE, 且 locale 对应 GBK 字符集,
//    则输出缓冲区 mbstr 中将包含 {0x61, 0x62, 0x63, 0xba, 0x00}
//    其中 0xba 是 GBK 编码的 "汉" (0xbaba) 的截断字节
//
inline
errno_t WcsToMbs(
    const wchar_t* wcstr,
    _Out_ char* mbstr,
    size_t sizeMbstr,
    _locale_t loc = NULL,
    _Out_ size_t* numConverted = NULL,
    size_t num = _TRUNCATE)
{
    if (wcstr == NULL)
        return EINVAL;

    errno_t err;
    if (loc == NULL)
        err = wcstombs_s(numConverted, mbstr, sizeMbstr, wcstr, num);
    else
        err = _wcstombs_s_l(numConverted, mbstr, sizeMbstr, wcstr, num, loc);

    // 字符映射错误: 当前 locale 下, 无法将 wchar_t (UTF-16) 字符转换为 MBCS 字符
    // wcstombs_s 实际返回值和 VC 2005 MSDN 参考有出入, 映射错误会返回 EILSEQ
    if ((numConverted != NULL && *numConverted == (size_t) -1) || err == EILSEQ)
        return EILSEQ;

    return err;
}

// 用 new 分配 mbstr 存储空间
// 调用 WcsToMbs, 将 wchar_t (UTF-16) 字符串 wcstr 转换为 MBCS 字符串 mbstr
// 注意:
// 用户负责释放 (delete) 用完的 mbstr 内存
inline
errno_t WcsToMbsNew(
    const wchar_t* wcstr,
    _Out_ char*& mbstr,
    _locale_t loc = NULL,
    _Out_ size_t* numConverted = NULL,
    size_t num = _TRUNCATE)
{
    // 得到需要的输出缓冲区的大小, 以 byte 为单位
    size_t sizeMbstr;
    errno_t err = WcsToMbs(wcstr, NULL, 0, loc, &sizeMbstr);
    if (err != 0)
        return err;

    if (num != _TRUNCATE)
        sizeMbstr = min(sizeMbstr, num + 1);

    mbstr = new(std::nothrow) char[sizeMbstr];
    if (mbstr == NULL)
        return ENOMEM;

    // 实际转换 wcstr 到 mbstr
    err = WcsToMbs(wcstr, mbstr, sizeMbstr, loc, numConverted, num);
    return err;
}

////////////////////////////////////////////////////////////////////////////////
// Convert to _TCHAR Helpers
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 将 char* 或 basic_string<char> 转换为 _TCHAR*
//
// 这些为 inline 函数, include 到用户工程后, 根据字符集设置即 _UNICODE 定义与否,
// 使用不同的代码:
// 1. 定义 _UNICODE 时, _TCHAR=wchar_t
//    调用 MbsToWcs 将 mbstr 转换为 wchar_t, 存储到输出缓冲区 tchar 中
//    尝试转换整个 mbstr, 直到 \0 结尾, 或者因为缓冲区 tchar 太小, mbstr 被截断
//    返回值: 返回输出缓冲区 tchar
//    a. 转换成功, 或者 mbstr 被截断, tchar 中存储已转换的字符串
//    b. 转换失败, tchar 为空字符串
// 2. 未定义 _UNICODE 时, _TCHAR=char
//    返回值: 直接返回 char 字符串 mbstr
////////////////////////////////////////////////////////////////////////////////

// 输入字符串类型为 const char*
inline
const _TCHAR* ToTchar(
    const char* mbstr,
    _Out_ _TCHAR* tchar,
    size_t sizeTchar,
    _locale_t loc = NULL)
{
#ifdef _UNICODE
    errno_t err;
    err = MbsToWcs(mbstr, tchar, sizeTchar, loc);
    if (err != 0 && err != STRUNCATE)
        *tchar = 0;
    return tchar;
#else
    // _UNICODE not defined
    UNREFERENCED_PARAMETER(tchar);
    UNREFERENCED_PARAMETER(sizeTchar);
    return mbstr;
#endif
}

// 输入字符串类型为 const char*
// 模板自动推导输出缓冲区 tchar 大小
//
// 要求:
// tchar 是数组 _TCHAR[], 而非指针 _TCHAR*
template <size_t SizeTchar>
inline
const _TCHAR* ToTchar(
    const char* mbstr,
    _Out_ _TCHAR (&tchar)[SizeTchar],
    _locale_t loc = NULL)
{
    return ToTchar(mbstr, tchar, SizeTchar, loc);
}

// 输入字符串类型为 basic_string<char>
inline
const _TCHAR* ToTchar(
    const std::string& mbstr,
    _Out_ _TCHAR* tchar,
    size_t sizeTchar,
    _locale_t loc = NULL)
{
    return ToTchar(mbstr.c_str(), tchar, sizeTchar, loc);
}

// 输入字符串类型为 basic_string<char>
// 模板自动推导输出缓冲区 tchar 大小
//
// 要求:
// tchar 是数组 _TCHAR[], 而非指针 _TCHAR*
template <size_t SizeTchar>
inline
const _TCHAR* ToTchar(
    const std::string& mbstr,
    _Out_ _TCHAR (&tchar)[SizeTchar],
    _locale_t loc = NULL)
{
    return ToTchar(mbstr.c_str(), tchar, SizeTchar, loc);
}

////////////////////////////////////////////////////////////////////////////////
// 将 wchar_t* 或 basic_string<wchar_t> 转换为 _TCHAR*
//
// 当使用 SBCS/MBCS 配置时, sizeTchar 为输出缓冲区 tchar 的 byte 数
//
// 当下列函数使用 WcsToMbs 从 wchar_t 字符串转换为 char 字符串时, 无法检测
// 出 WcsToMbs 的 1 类型截断 (参考 WcsToMbs 说明), 需要用户自行判断
////////////////////////////////////////////////////////////////////////////////

// 输入字符串类型为 const wchar_t*
inline
const _TCHAR* ToTchar(
    const wchar_t* wcstr,
    _Out_ _TCHAR* tchar,
    size_t sizeTchar,
    _locale_t loc = NULL)
{
#ifdef _UNICODE
    UNREFERENCED_PARAMETER(tchar);
    UNREFERENCED_PARAMETER(sizeTchar);
    return wcstr;
#else
    // _UNICODE not defined
    errno_t err;
    err = WcsToMbs(wcstr, tchar, sizeTchar, loc);
    if (err != 0 && err != STRUNCATE)
        *tchar = 0;
    return tchar;
#endif
}

// 输入字符串类型为 const wchar_t*
// 模板自动推导输出缓冲区 tchar 大小
//
// 要求:
// tchar 是数组 _TCHAR[], 而非指针 _TCHAR*
template <size_t SizeTchar>
inline
const _TCHAR* ToTchar(
    const wchar_t* wcstr,
    _Out_ _TCHAR (&tchar)[SizeTchar],
    _locale_t loc = NULL)
{
    return ToTchar(wcstr, tchar, SizeTchar, loc);
}

// 输入字符串类型为 basic_string<wchar_t>
inline
const _TCHAR* ToTchar(
    const std::wstring& wcstr,
    _Out_ _TCHAR* tchar,
    size_t sizeTchar,
    _locale_t loc = NULL)
{
    return ToTchar(wcstr.c_str(), tchar, sizeTchar, loc);
}

// 输入字符串类型为 basic_string<wchar_t>
// 模板自动推导输出缓冲区 tchar 大小
//
// 要求:
// tchar 是数组 _TCHAR[], 而非指针 _TCHAR*
template <size_t SizeTchar>
inline
const _TCHAR* ToTchar(
    const std::wstring& wcstr,
    _Out_ _TCHAR (&tchar)[SizeTchar],
    _locale_t loc = NULL)
{
    return ToTchar(wcstr.c_str(), tchar, SizeTchar, loc);
}

}   // namespace bzu

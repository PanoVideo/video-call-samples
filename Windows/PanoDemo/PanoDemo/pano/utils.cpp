

#include "utils.h"

#ifdef PANO_OS_WIN
# define WIN32_LEAN_AND_MEAN
# include <Windows.h>
#endif

PANO_UTILS_NS_BEGIN

#ifdef PANO_OS_WIN

std::string utf8_encode(const wchar_t *wstr, int len)
{
    if (!wstr || !len) return std::string();
    auto utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, len, NULL, 0, NULL, NULL);
    if (utf8_len <= 0) {
        return "";
    }
    std::string utf8_str(utf8_len, 0);
    utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, len, &utf8_str[0], utf8_len, NULL, NULL);
    if (utf8_len < 0) {
        return "";
    }
    return utf8_str;
}

std::string utf8_encode(const wchar_t *wstr)
{
    auto utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (utf8_len <= 1) { // include the terminating null character
        return "";
    }
    std::string utf8_str(utf8_len, 0);
    utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &utf8_str[0], utf8_len, NULL, NULL);
    --utf8_len; // exclude the terminating null character
    if (utf8_len < 0) {
        return "";
    }
    utf8_str.resize(utf8_len);
    return utf8_str;
}
// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

#endif


PANO_UTILS_NS_END // namespace pano::utils

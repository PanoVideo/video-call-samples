
#pragma once

#include "PanoConf.h"

#include <string>


#define PANO_UTILS_NS_BEGIN namespace pano{ namespace utils {
#define PANO_UTILS_NS_END }}
#define PANO_UTILS_NS_USING using namespace pano::utils;

PANO_UTILS_NS_BEGIN

#ifdef PANO_OS_WIN

std::string utf8_encode(const wchar_t *wstr, int len);
std::string utf8_encode(const wchar_t *wstr);
std::string utf8_encode(const std::wstring &wstr);
std::wstring utf8_decode(const std::string &str);

#endif

template <typename T>
struct DestroyableObjectDeleter
{
    void operator() (T *ptr)
    {
        if (ptr) ptr->destroy();
    }
};

template <typename T>
using DUniquePtr = std::unique_ptr<T, DestroyableObjectDeleter<T>>;

PANO_UTILS_NS_END // namespace pano::utils

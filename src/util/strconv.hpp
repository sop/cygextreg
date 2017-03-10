#ifndef __UTIL__STRCONV_HPP__
#define __UTIL__STRCONV_HPP__

#include <windows.h>
#include <string>

namespace cygscript {

std::string wide_to_mb(std::wstring str, UINT codepage = CP_THREAD_ACP);
std::wstring mb_to_wide(std::string str, UINT codepage = CP_THREAD_ACP);

}

#endif

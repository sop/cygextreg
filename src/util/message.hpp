#ifndef __UTIL__MESSAGE_HPP__
#define __UTIL__MESSAGE_HPP__

#include <windows.h>
#include <string>

namespace cygregext {

void show_message(const std::wstring& msg, UINT type = MB_OK);
void show_message(const std::string& msg, UINT type = MB_OK);

}

#endif

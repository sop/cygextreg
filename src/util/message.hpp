#ifndef __UTIL__MESSAGE_HPP__
#define __UTIL__MESSAGE_HPP__

#include <windows.h>
#include <iostream>

namespace cygscript {

void show_message(std::wstring msg, UINT type = MB_OK);
void show_message(std::string msg, UINT type = MB_OK);

}

#endif

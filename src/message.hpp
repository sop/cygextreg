#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <windows.h>

namespace cygscript {

void show_message(std::wstring msg, UINT type = MB_OK);
void show_message(std::string msg, UINT type = MB_OK);

}

#endif

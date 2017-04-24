#include "message.hpp"
#include <iostream>
#include <sstream>

namespace cygregext {

void show_message(const std::wstring& msg, UINT type) {
	auto str = msg.substr(0, msg.find_last_not_of(L" \t\r\n") + 1);
	std::wostream* s;
	s = type & MB_ICONERROR ? &std::wcerr : &std::wcout;
	(*s) << str.c_str() << std::endl;
	if ((*s).fail()) {
		std::wstring title(type & MB_ICONERROR ? L"Error" : L"Info");
		MessageBoxW(NULL, str.c_str(), title.c_str(), type);
	}
}

void show_message(const std::string& msg, UINT type) {
	auto str = msg.substr(0, msg.find_last_not_of(" \t\r\n") + 1);
	std::ostream* s;
	s = type & MB_ICONERROR ? &std::cerr : &std::cout;
	(*s) << str.c_str() << std::endl;
	if ((*s).fail()) {
		std::string title(type & MB_ICONERROR ? "Error" : "Info");
		MessageBoxA(NULL, str.c_str(), title.c_str(), type);
	}
}

}

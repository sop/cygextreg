#include <windows.h>
#include <iostream>

namespace cygscript {

void show_messagew(std::wstring msg, UINT type) {
	std::wostream* s;
	s = type & MB_ICONERROR ? &std::wcerr : &std::wcout;
	(*s) << msg.c_str() << std::endl;
	if ((*s).fail()) {
		std::wstring title(type & MB_ICONERROR ? L"Error" : L"Info");
		MessageBoxW(NULL, msg.c_str(), title.c_str(), type);
	}
}

void show_messagea(std::string msg, UINT type) {
	std::ostream* s;
	s = type & MB_ICONERROR ? &std::cerr : &std::cout;
	(*s) << msg.c_str() << std::endl;
	if ((*s).fail()) {
		std::string title(type & MB_ICONERROR ? "Error" : "Info");
		MessageBoxA(NULL, msg.c_str(), title.c_str(), type);
	}
}

}

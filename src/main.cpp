#include <iostream>
#include <sstream>
#include <stdexcept>
#include <windows.h>
#include "app.hpp"
#include "message.hpp"


int main(const int argc, char* const argv[]) {
	try {
		cygscript::App app(argc, argv);
		return app.run();
	} catch (const std::exception &e) {
		std::stringstream ss;
		ss << "ERROR: " << e.what() << std::endl;
		cygscript::show_message(ss.str(), MB_OK | MB_ICONERROR);
		return 1;
	}
	return 0;
}

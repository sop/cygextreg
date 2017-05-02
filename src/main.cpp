#include <windows.h>
#include <sstream>
#include <stdexcept>
#include "app.hpp"
#include "util/message.hpp"

int main(const int argc, char* const argv[]) {
	setlocale(LC_ALL, "C.UTF-8");
	try {
		cygextreg::App app(argc, argv);
		return app.run();
	} catch (const std::exception &e) {
		std::stringstream ss;
		ss << "ERROR: " << e.what() << std::endl;
		cygextreg::show_message(ss.str(), MB_OK | MB_ICONERROR);
		return 1;
	}
	return 0;
}

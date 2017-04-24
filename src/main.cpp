#include <windows.h>
#include <sstream>
#include <stdexcept>
#include "app.hpp"
#include "util/message.hpp"

int main(const int argc, char* const argv[]) {
	setlocale(LC_ALL, "C.UTF-8");
	try {
		cygregext::App app(argc, argv);
		return app.run();
	} catch (const std::exception &e) {
		std::stringstream ss;
		ss << "ERROR: " << e.what() << std::endl;
		cygregext::show_message(ss.str(), MB_OK | MB_ICONERROR);
		return 1;
	}
	return 0;
}

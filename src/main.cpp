#include <iostream>
#include <stdexcept>
#include "app.hpp"

int main(const int argc, char* const argv[]) {
	try {
		cygscript::App app(argc, argv);
		return app.run();
	} catch (const std::exception &e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

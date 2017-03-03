#include "app.hpp"
#include <iostream>
#include <stdexcept>
#include <memory>
#include <getopt.h>
#include "register.hpp"
#include "exec.hpp"
#include "elevated.hpp"

namespace cygscript {

App::App(const int argc, char* const argv[]) :
	_argv(argv),
	_cmd(Command::NONE),
	_regType(RegisterType::USER) {
	static const char *opts = "re";
	static const struct option longopts[] = {
		{ "register", no_argument, NULL, 'r' },
		{ "all", no_argument, NULL, 'a' },
		{ "exec", no_argument, NULL, 'e' },
		{ 0, 0, 0, 0 }
	};
	while (1) {
		int opt_index = 0;
		int c = 0;
		c = getopt_long(argc, argv, opts, longopts, &opt_index);
		if (-1 == c) {
			break;
		}
		switch (c) {
		case 'r':
			_cmd = Command::REGISTER;
			break;
		case 'a':
			_regType = RegisterType::EVERYONE;
			break;
		case 'e':
			_cmd = Command::EXEC;
			break;
		case '?':
		default:
			_printUsage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	if (Command::NONE == _cmd) {
		_printUsage(argv[0]);
		exit(EXIT_FAILURE);
	}
	for (int i = optind; i < argc; ++i) {
		_execArgs.push_back(std::string(argv[i]));
	}
}

int App::run() {
	std::unique_ptr<ICommand> cmd;
	switch (_cmd) {
	case Command::EXEC:
		cmd = std::unique_ptr<ExecCommand>(new ExecCommand(_execArgs));
		break;
	case Command::REGISTER:
		if (_regType == RegisterType::EVERYONE) {
			_checkElevated();
		}
		cmd = std::unique_ptr<RegisterCommand>(
			new RegisterCommand(_regType == RegisterType::EVERYONE));
		break;
	case Command::NONE:
		return 1;
	}
	return cmd->run();
}

void App::_printUsage(char *progname) {
	std::cerr << "Usage: " << progname << std::endl;
}

void App::_checkElevated() {
	ElevatedProcess proc;
	if (proc.isAdmin()) {
		return;
	}
	proc.startElevated(_argv);
	exit(EXIT_SUCCESS);
}

}

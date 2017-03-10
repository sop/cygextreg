#include "app.hpp"
#include <windows.h>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <getopt.h>
#include "cmd/register.hpp"
#include "cmd/exec.hpp"
#include "cmd/list.hpp"
#include "elevated.hpp"
#include "strconv.hpp"
#include "message.hpp"

namespace cygscript {

App::App(const int argc, char* const argv[]) :
	_argc(argc),
	_argv(argv),
	_cmd(Command::NONE),
	_regType(RegisterType::USER) {
	static const char *opts = "rualeh";
	static const struct option longopts[] = {
		{ "register", no_argument, NULL, 'r' },
		{ "unregister", no_argument, NULL, 'u' },
		{ "all", no_argument, NULL, 'a' },
		{ "list", no_argument, NULL, 'l' },
		{ "exec", no_argument, NULL, 'e' },
		{ "help", no_argument, NULL, 'h' },
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
		case 'u':
			_cmd = Command::UNREGISTER;
			break;
		case 'a':
			_regType = RegisterType::EVERYONE;
			break;
		case 'l':
			_cmd = Command::LIST;
			break;
		case 'e':
			_cmd = Command::EXEC;
			break;
		case '?':
		case 'h':
		default:
			_printUsage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	if (Command::NONE == _cmd) {
		_printUsage(argv[0]);
		exit(EXIT_FAILURE);
	}
}

int App::run() {
	std::unique_ptr<ICommand> cmd;
	switch (_cmd) {
	case Command::EXEC:
		cmd = std::unique_ptr<ExecCommand>(new ExecCommand(_wideArgs()));
		break;
	case Command::REGISTER:
		if (_regType == RegisterType::EVERYONE) {
			_checkElevated();
		}
		cmd = std::unique_ptr<RegisterCommand>(
			new RegisterCommand(RegisterCommand::Command::REGISTER,
			                    _regType == RegisterType::EVERYONE));
		break;
	case Command::UNREGISTER:
		if (_regType == RegisterType::EVERYONE) {
			_checkElevated();
		}
		cmd = std::unique_ptr<RegisterCommand>(
			new RegisterCommand(RegisterCommand::Command::UNREGISTER,
			                    _regType == RegisterType::EVERYONE));
		break;
	case Command::LIST:
		cmd = std::unique_ptr<ListCommand>(new ListCommand());
		break;
	case Command::NONE:
		return 1;
	}
	return cmd->run();
}

static char help[] =
	""
	"Options:\n"
	"  -r, --register     Add .sh filetype to Windows registry.\n"
	"  -u, --unregister   Remove .sh filetype from Windows registry.\n"
	"  -a, --all          Register or unregister filetype for all users, \n"
	"                       default to current user.\n"
	"  -l, --list         List registry status.\n"
	"  -h, --help         This help.\n";

void App::_printUsage(char *progname) {
	std::stringstream ss;
	ss << "Usage: " << progname << " [OPTION]..." << std::endl;
	ss << "Register .sh filetype to Windows explorer." << std::endl;
	ss << std::endl;
	ss << help;
	show_message(ss.str());
}

std::vector<std::wstring> App::_wideArgs() {
	std::vector<std::wstring> args;
	LPWSTR* argv;
	int argc;
	argv = CommandLineToArgvW(GetCommandLine(), &argc);
	/* if Windows command line has less arguments than what was
	   passed to the main(), then application was invoked from Cygwin shell. */
	if (argc < _argc) {
		for (int i = 0; i < _argc; ++i) {
			/* assume utf-8 */
			args.push_back(mb_to_wide(_argv[i], CP_UTF8));
		}
	} else {
		for (int i = 0; i < argc; ++i) {
			args.push_back(argv[i]);
		}
	}
	LocalFree(argv);
	return args;
}

void App::_checkElevated() {
	ElevatedProcess proc;
	if (proc.isAdmin()) {
		return;
	}
	proc.startElevated(_argc, _argv);
	exit(EXIT_SUCCESS);
}

}

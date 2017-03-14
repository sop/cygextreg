#include "app.hpp"
#include "config.h"
#include <windows.h>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <getopt.h>
#include "cmd/register.hpp"
#include "cmd/exec.hpp"
#include "cmd/list.hpp"
#include "util/elevated.hpp"
#include "util/strconv.hpp"
#include "util/message.hpp"

namespace cygscript {

App::App(const int argc, char* const argv[]) :
	_argc(argc),
	_argv(argv),
	_cmd(Command::NONE),
	_regType(RegisterType::USER),
	_extension(".sh"),
	_force(false) {
	static const char *opts = "ruaflhV";
	static const struct option longopts[] = {
		{ "register", no_argument, NULL, 'r' },
		{ "unregister", no_argument, NULL, 'u' },
		{ "ext", required_argument, NULL, 'e' },
		{ "all", no_argument, NULL, 'a' },
		{ "force", no_argument, NULL, 'f' },
		{ "list", no_argument, NULL, 'l' },
		{ "exec", no_argument, NULL, 'E' },
		{ "version", no_argument, NULL, 'V' },
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
		/* --register */
		case 'r':
			_cmd = Command::REGISTER;
			break;
		/* --unregister */
		case 'u':
			_cmd = Command::UNREGISTER;
			break;
		/* --ext */
		case 'e':
			_extension = optarg;
			break;
		/* --all */
		case 'a':
			_regType = RegisterType::EVERYONE;
			break;
		/* --force */
		case 'f':
			_force = true;
			break;
		/* --list */
		case 'l':
			_cmd = Command::LIST;
			break;
		/* --exec */
		case 'E':
			_cmd = Command::EXEC;
			break;
		/* --version */
		case 'V':
			_printVersion();
			exit(EXIT_SUCCESS);
		/* --help */
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
	if ('.' != _extension[0]) {
		_extension.insert(0, ".");
	}
}

int App::run() {
	std::unique_ptr<ICommand> cmd;
	switch (_cmd) {
	/* execute command */
	case Command::EXEC:
		cmd = std::unique_ptr<ExecCommand>(new ExecCommand(_wideArgs()));
		break;
	/* register extension */
	case Command::REGISTER:
		if (_regType == RegisterType::EVERYONE) {
			_checkElevated();
		}
		cmd = std::unique_ptr<RegisterCommand>(
			new RegisterCommand(
				RegisterCommand::Command::REGISTER, _extension,
				_regType == RegisterType::EVERYONE, _force));
		break;
	/* unregister extension */
	case Command::UNREGISTER:
		if (_regType == RegisterType::EVERYONE) {
			_checkElevated();
		}
		cmd = std::unique_ptr<RegisterCommand>(
			new RegisterCommand(
				RegisterCommand::Command::UNREGISTER, _extension,
				_regType == RegisterType::EVERYONE, _force));
		break;
	/* list registered extensions */
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
	"  -r, --register    Add filetype to Windows registry.\n"
	"  -u, --unregister  Remove filetype from Windows registry.\n"
	"      --ext=EXT     Register or unregister files of given extension,\n"
	"                      default to .sh\n"
	"  -a, --all         Register or unregister filetype for all users,\n"
	"                      default to current user.\n"
	"  -f, --force       Overwrite if already registered for another application.\n"
	"  -l, --list        List registry status.\n"
	"  -h, --help        Display this help and exit.\n"
	"  -V, --version     Print version and exit.\n";

void App::_printUsage(char *progname) {
	std::stringstream ss;
	ss << "Usage: " << progname << " [OPTION]..." << std::endl;
	ss << "Register .sh filetype to Windows explorer." << std::endl;
	ss << std::endl;
	ss << help;
	show_message(ss.str());
}

void App::_printVersion() {
	std::cout << "cygscript " << VERSION << std::endl;
	std::cout << "Copyright (C) 2017 Joni Eskelinen" << std::endl;
	std::cout << "License MIT: The MIT License" << std::endl;
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

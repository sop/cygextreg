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
#include "util/winerror.hpp"
#include "util/elevated.hpp"
#include "util/strconv.hpp"
#include "util/message.hpp"

namespace cygregext {

/* type of native WinApi GetCommandLineW */
typedef LPWSTR (__stdcall* native_GetCommandLineW)(void);

App::App(const int argc, char* const argv[]) :
	_argc(argc),
	_argv(argv),
	_cmd(Command::NONE),
	_regType(RegisterType::USER),
	_extension(".sh"),
	_iconPath(std::string()),
	_force(false) {
	static const char *opts = "ruaflhV";
	static const struct option longopts[] = {
		{ "register", no_argument, NULL, 'r' },
		{ "unregister", no_argument, NULL, 'u' },
		{ "ext", required_argument, NULL, 'e' },
		{ "icon", required_argument, NULL, 'i' },
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
		/* --icon */
		case 'i':
			_iconPath = optarg;
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
			new RegisterCommand(_extension, _iconPath,
			                    _regType == RegisterType::EVERYONE, _force));
		break;
	/* unregister extension */
	case Command::UNREGISTER:
		if (_regType == RegisterType::EVERYONE) {
			_checkElevated();
		}
		cmd = std::unique_ptr<UnregisterCommand>(
			new UnregisterCommand(_extension,
			                      _regType == RegisterType::EVERYONE));
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

WinPathW App::getPath() {
	wchar_t buf[MAX_PATH + 1];
	DWORD ret = GetModuleFileName(NULL, buf, sizeof(buf) / sizeof(buf[0]));
	if (0 == ret) {
		THROW_LAST_ERROR("Failed to get executable path.");
	}
	if (sizeof(buf) / sizeof(buf[0]) == ret &&
	    ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
		throw std::runtime_error("Failed to get executable path.");
	}
	return WinPathW(std::wstring(buf, ret));
}

static char help[] =
	""
	"Options:\n"
	"  -r, --register     Add a file type to the Windows registry.\n"
	"  -u, --unregister   Remove a file type from the Windows registry.\n"
	"      --ext=EXT      Register or unregister files of the given extension.\n"
	"                       Default to .sh.\n"
	"      --icon=PATH,N  Path and index of the icon to register for an extension.\n"
	"                       Default to the icon of this application.\n"
	"  -a, --all          Register or unregister extension for all users.\n"
	"                       Default to current user only.\n"
	"  -f, --force        Overwrite if already registered for another application.\n"
	"  -l, --list         List registered extensions.\n"
	"  -h, --help         Display this help and exit.\n"
	"  -V, --version      Print version and exit.\n";

void App::_printUsage(char *progname) {
	std::stringstream ss;
	ss << "Usage: " << progname << " -r|-u|-l [OPTION]..." << std::endl;
	ss << "Register a script file type (.sh) to Windows File Explorer."
	   << std::endl << std::endl;
	ss << help;
	show_message(ss.str());
}

void App::_printVersion() {
	std::cout << "cygregext " << VERSION << std::endl;
	std::cout << "Copyright (C) 2017 Joni Eskelinen" << std::endl;
	std::cout << "License MIT: The MIT License" << std::endl;
	std::cout << "Icons: Tango Desktop Project" << std::endl;
}

std::vector<std::wstring> App::_wideArgs() {
	std::vector<std::wstring> args;
	LPWSTR* argv;
	int argc;
	/* use native WinApi GetCommandLineW since Cygwin hooks it */
	native_GetCommandLineW fn = (native_GetCommandLineW)GetProcAddress(
		GetModuleHandle(L"kernel32.dll"), "GetCommandLineW");
	if (NULL == fn) {
		THROW_LAST_ERROR("Failed to import GetCommandLineW");
	}
	argv = CommandLineToArgvW(fn(), &argc);
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

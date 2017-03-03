#include "exec.hpp"
#include <unistd.h>
#include <iostream>
#include <sys/cygwin.h>
#include <sys/stat.h>
#include <windows.h>
#include <shlwapi.h>

namespace cygscript {

int ExecCommand::run() {
	char pathbuf[MAX_PATH + 1];
	std::vector<std::string> newargs;
	if (0 != cygwin_conv_path(
			CCP_POSIX_TO_WIN_A, "/bin/mintty.exe", pathbuf, sizeof(pathbuf))) {
		throw std::runtime_error(strerror(errno));
	}
	newargs.push_back(std::string(pathbuf));
	newargs.push_back("--exec");
	for (auto str : _args) {
		if (_isWinPath(str, true)) {
			if (0 != cygwin_conv_path(
					CCP_WIN_A_TO_POSIX, str.c_str(), pathbuf,
					sizeof(pathbuf))) {
				throw std::runtime_error(strerror(errno));
			}
			newargs.push_back(std::string(pathbuf));
		} else {
			newargs.push_back(str);
		}
	}
	return _execute(newargs);
}

int ExecCommand::_execute(std::vector<std::string> args) {
	char** argv;
	argv = (char**)malloc((args.size() + 1) * sizeof(char*));
	for (size_t i = 0; i < args.size(); ++i) {
		argv[i] = (char*)args[i].c_str();
	}
	argv[args.size()] = nullptr;
	if (-1 == execv(argv[0], argv)) {
		throw std::runtime_error(strerror(errno));
	}
	return 0;
}

bool ExecCommand::_isWinPath(std::string path, bool must_exist)
{
	/* if detected by Cygwin */
	if (cygwin_posix_path_list_p(path.c_str())) {
		return false;
	}
	/* if path must exist */
	if (must_exist && FALSE == PathFileExistsA(path.c_str())) {
		return false;
	}
	return true;
}

bool ExecCommand::_fileExists(std::string path)
{
	struct stat s;
	if (-1 == stat(path.c_str(), &s)) {
		return false;
	}
	if (!(S_ISREG(s.st_mode) || S_ISLNK(s.st_mode))) {
		errno = ENOENT;
		return false;
	}
	return true;
}

}

#include "exec.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/cygwin.h>
#include <sys/stat.h>
#include <windows.h>
#include <shlwapi.h>
#include "strconv.hpp"
#include "winerror.hpp"
#include "message.hpp"

namespace cygscript {

int ExecCommand::run() {
	wchar_t w_pathbuf[MAX_PATH + 1];
	std::vector<std::wstring> newargs;
	/* Windows path to mintty.exe */
	if (0 != cygwin_conv_path(
			CCP_POSIX_TO_WIN_W, "/bin/mintty.exe", w_pathbuf,
			sizeof(w_pathbuf))) {
		throw std::runtime_error(strerror(errno));
	}
	newargs.push_back(std::wstring(w_pathbuf));
	/* force MinTTY's charset to UTF-8 */
	newargs.push_back(L"-o");
	newargs.push_back(L"Charset=UTF-8");
	newargs.push_back(L"--exec");
	for (auto arg : _getExecArgs()) {
		/* convert Windows paths to cygwin form */
		if (_isWinPath(arg, true)) {
			newargs.push_back(_pathWinToPosix(arg));
		} else {
			newargs.push_back(arg);
		}
	}
	return _execute(newargs);
}

std::vector<std::wstring> ExecCommand::_getExecArgs() {
	std::vector<std::wstring> args;
	size_t i;
	for (i = 0; i < _args.size(); ++i) {
		if (_args[i] == L"--") {
			break;
		}
	}
	for (++i; i < _args.size(); ++i) {
		args.push_back(_args[i]);
	}
	return args;
}

int ExecCommand::_execute(std::vector<std::wstring> args) {
	STARTUPINFOA si = {};
	PROCESS_INFORMATION pi = {};
	std::wstringstream cmd_args;
	for(auto arg : args) {
		cmd_args << "\"" << _escapeArg(arg) << "\" ";
	}
	si.cb = sizeof(si);
	std::string cmd_argsa = wide_to_mb(cmd_args.str(), CP_UTF8);
	if (0 == CreateProcessA(
			NULL, (LPSTR)cmd_argsa.c_str(), NULL, NULL, FALSE,
			CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi)) {
		THROW_LAST_ERROR("Failed to start process");
	}
	return 0;
}

bool ExecCommand::_isWinPath(std::wstring path, bool must_exist) {
	/* if detected by Cygwin */
	if (cygwin_posix_path_list_p(wide_to_mb(path).c_str())) {
		return false;
	}
	/* if path must exist */
	if (must_exist && FALSE == PathFileExists(path.c_str())) {
		return false;
	}
	return true;
}

std::wstring ExecCommand::_pathWinToPosix(std::wstring winpath) {
	char buf[MAX_PATH + 1];
	if (0 != cygwin_conv_path(
			CCP_WIN_W_TO_POSIX, winpath.c_str(), buf, sizeof(buf))) {
		throw std::runtime_error(strerror(errno));
	}
	/* Cygwin represents paths in utf-8 */
	return mb_to_wide(buf, CP_UTF8);
}

std::wstring ExecCommand::_escapeArg(std::wstring arg) {
	_replaceAll(arg, L"\"", L"\\\"");
	return arg;
}

void ExecCommand::_replaceAll(std::wstring& str, const std::wstring& from,
                              const std::wstring& to) {
	size_t start = 0;
	while((start = str.find(from, start)) != std::wstring::npos) {
		str.replace(start, from.length(), to);
		start += to.length();
	}
}

}

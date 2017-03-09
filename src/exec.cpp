#include "exec.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/cygwin.h>
#include <windows.h>
#include <shlwapi.h>
#include "strconv.hpp"
#include "winerror.hpp"
#include "message.hpp"
#include "env/envvar.hpp"

namespace cygscript {

int ExecCommand::run() {
	wchar_t w_pathbuf[MAX_PATH + 1];
	std::vector<std::wstring> exec_args = _getExecArgs();
	if (0 == exec_args.size()) {
		throw std::runtime_error("Missing executable script.");
	}
	std::wstring script_path = exec_args[0];
	std::wstring script_name = _getScriptName(script_path);
	if (!_fileExists(script_path)) {
		throw std::runtime_error("Script not found.");
	}
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
	/* window title */
	newargs.push_back(L"-t");
	newargs.push_back(script_name);
	/* execute in bash interactive session */
	newargs.push_back(L"--exec");
	newargs.push_back(L"/bin/sh");
	newargs.push_back(L"-il");
	newargs.push_back(L"--");
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
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {};
	env::EnvVar(L"PATH").set(_getEnvPath());
	std::wstringstream cmd_args;
	for(auto arg : args) {
		cmd_args << "\"" << _escapeArg(arg) << "\" ";
	}
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

bool ExecCommand::_fileExists(std::wstring path) {
	return TRUE == PathFileExists(path.c_str());
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

std::wstring ExecCommand::_getEnvPath() {
	wchar_t buf[MAX_PATH + 1];
	std::wstring curpath = env::EnvVar(L"PATH").get();
	if (0 != cygwin_conv_path(
			CCP_POSIX_TO_WIN_W, "/", buf, sizeof(buf))) {
		throw std::runtime_error(strerror(errno));
	}
	std::wstring cygdir(buf);
	std::wstringstream ss;
	ss << cygdir << L"\\usr\\local\\bin;" << cygdir << L"\\bin;" << curpath;
	return ss.str();
}

std::wstring ExecCommand::_getScriptName(std::wstring path) {
	wchar_t* buf = new wchar_t[path.size() + 1];
	wcscpy(buf, path.c_str());
	wchar_t *p = PathFindFileName(buf);
	std::wstring name(p);
	delete[] buf;
	return name;
}

}

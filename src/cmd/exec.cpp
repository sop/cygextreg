#include "exec.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/cygwin.h>
#include <sys/stat.h>
#include <windows.h>
#include <shlwapi.h>
#include "util/strconv.hpp"
#include "util/winerror.hpp"
#include "util/message.hpp"
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
	std::wstringstream cmd_line;
	/* Windows path to mintty.exe */
	if (0 != cygwin_conv_path(
			CCP_POSIX_TO_WIN_W, "/bin/mintty.exe", w_pathbuf,
			sizeof(w_pathbuf))) {
		throw std::runtime_error(strerror(errno));
	}
	cmd_line << w_pathbuf << L" -o Charset=UTF-8"
	         << L" -t " << _escapeWinArg(script_name)
	         << L" --exec /bin/sh -il -c ";
	cmd_line << _escapeWinArg(_getExecCmd());
	_execute(cmd_line.str());
	return 0;
}

std::vector<std::wstring> ExecCommand::_getExecArgs() {
	std::vector<std::wstring> args;
	size_t i;
	/* search beginning of exec arguments */
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

std::wstring ExecCommand::_getExecCmd() {
	std::wstringstream ss;
	for (auto arg : _getExecArgs()) {
		/* convert Windows paths to Cygwin form */
		if (_isWinPath(arg, true)) {
			arg = mb_to_wide(_pathWinToPosix(_toLongPath(arg)));
		}
		ss << _escapePosixArg(arg) << L" ";
	}
	std::wstring cmd = ss.str();
	/* remove leading space */
	if (cmd.size()) {
		cmd.pop_back();
	}
	return cmd;
}

void ExecCommand::_execute(const std::wstring& cmd_line) {
	STARTUPINFO si = {};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {};
	/* set PATH environment variable */
	env::EnvVar(L"PATH").set(_getEnvPath());
	if (0 == CreateProcess(
			NULL, (LPWSTR)cmd_line.c_str(), NULL, NULL, FALSE,
			CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi)) {
		THROW_LAST_ERROR("Failed to start process");
	}
}

bool ExecCommand::_isWinPath(const std::wstring& path, bool must_exist) {
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

bool ExecCommand::_fileExists(const std::wstring& path) {
	std::string posix_path = _pathWinToPosix(path);
	struct stat s;
	if (-1 == stat(posix_path.c_str(), &s)) {
		return false;
	}
	if (!(S_ISREG(s.st_mode) || S_ISLNK(s.st_mode))) {
		return false;
	}
	return true;
}

std::string ExecCommand::_pathWinToPosix(const std::wstring& winpath) {
	char buf[MAX_PATH + 1];
	if (0 != cygwin_conv_path(
			CCP_WIN_W_TO_POSIX, winpath.c_str(), buf, sizeof(buf))) {
		throw std::runtime_error(strerror(errno));
	}
	return std::string(buf);
}

std::wstring ExecCommand::_toLongPath(const std::wstring& path) {
	wchar_t buf[MAX_PATH + 1];
	DWORD len = GetLongPathName(
		path.c_str(), buf, sizeof(buf) / sizeof(buf[0]));
	if (0 == len) {
		return path;
	}
	return std::wstring(buf, len);
}

std::wstring ExecCommand::_escapeWinArg(const std::wstring& arg) {
	std::wstring str = arg;
	_replaceAll(str, L"\"", L"\\\"");
	return str.insert(0, L"\"").append(L"\"");
}

std::wstring ExecCommand::_escapePosixArg(const std::wstring& arg) {
	std::wstring str = arg;
	_replaceAll(str, L"'", L"'\\\''");
	return str.insert(0, L"'").append(L"'");
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

std::wstring ExecCommand::_getScriptName(const std::wstring& path) {
	wchar_t* buf = new wchar_t[path.size() + 1];
	wcscpy(buf, path.c_str());
	wchar_t *p = PathFindFileName(buf);
	std::wstring name(p);
	delete[] buf;
	return name;
}

}

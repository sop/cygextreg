#include "exec.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/cygwin.h>
#include <sys/stat.h>
#include <windows.h>
#include <shlwapi.h>
#include "util/cygpath.hpp"
#include "util/winpath.hpp"
#include "util/strconv.hpp"
#include "util/winerror.hpp"

namespace cygscript {

int ExecCommand::run() {
	std::vector<std::wstring> exec_args = _getExecArgs();
	if (0 == exec_args.size()) {
		throw std::runtime_error("Missing executable script.");
	}
	std::wstring script_path = exec_args[0];
	if (!_fileExists(script_path)) {
		throw std::runtime_error("Script not found.");
	}
	std::wstring script_name = _getScriptName(script_path);
	std::wstringstream cmd_line;
	/* Windows path to mintty.exe */
	WinPathW mintty = CygPath("/bin/mintty.exe").winPath();
	cmd_line << mintty.str() << L" -o Locale=C -o Charset=UTF-8"
	         << L" -t " << _escapeWinArg(script_name)
	         << L" --exec /bin/bash -il -c "
	         << _escapeWinArg(_getExecCmd());
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
			/* Posix path is represented in UTF-8 encoding. However, we must
			   pass the path as a wide string to CreateProcess.
			   The trick is to interpret UTF-8 sequence as an ISO-8859-1
			   and encode it to UTF-16. When Cygwin decodes Windows command
			   line back to C-locale, it receives the original UTF-8 sequence.
			 */
			arg = mb_to_wide(_pathWinToPosix(_toLongPath(arg)), 28591);
		}
		ss << _escapePosixArg(arg) << L" ";
	}
	/* if script exists with a non-zero exit code, keep terminal open
	   until keypress */
	ss << L"|| { echo -e \"\\n[Process exited - exit code $?]\";"
	    " read -n 1 -s; }";
	return ss.str();
}

void ExecCommand::_execute(const std::wstring& cmd_line) {
	STARTUPINFO si = {};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {};
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
	std::string posix_path = CygPath(WinPathW(path));
	struct stat s;
	if (-1 == stat(posix_path.c_str(), &s)) {
		return false;
	}
	if (!(S_ISREG(s.st_mode) || S_ISLNK(s.st_mode))) {
		return false;
	}
	return true;
}

std::string ExecCommand::_pathWinToPosix(const std::wstring& path) {
	return CygPath(WinPathW(path)).str();
}

std::wstring ExecCommand::_toLongPath(const std::wstring& path) {
	return WinPathW(path).longPath().str();
}

std::wstring ExecCommand::_escapeWinArg(const std::wstring& arg) {
	std::wstring str = arg;
	_replaceAll(str, L"\\", L"\\\\");
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

std::wstring ExecCommand::_getScriptName(const std::wstring& path) {
	wchar_t* buf = new wchar_t[path.size() + 1];
	wcscpy(buf, path.c_str());
	wchar_t *p = PathFindFileName(buf);
	std::wstring name(p);
	delete[] buf;
	return name;
}

}

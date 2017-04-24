#include "exec.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <sys/cygwin.h>
#include <sys/stat.h>
#include <windows.h>
#include <shlwapi.h>
#include "util/cygpath.hpp"
#include "util/winpath.hpp"
#include "util/strconv.hpp"
#include "util/winerror.hpp"

namespace cygregext {

int ExecCommand::run() {
	std::vector<std::wstring> exec_args = _getExecArgs();
	if (0 == exec_args.size()) {
		throw std::runtime_error("Missing executable script.");
	}
	_execute(exec_args);
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

void ExecCommand::_execute(const std::vector<std::wstring> args) {
	if (!_fileExists(args[0])) {
		throw std::runtime_error("Script not found.");
	}
	WinPathW script_path = WinPathW(args[0]).longPath();
	std::wstring script_dir = script_path.dirname().str();
	std::wstring script_name = script_path.basename().str();
	std::wstringstream cmd_line;
	/* Windows path to mintty.exe */
	WinPathW mintty = CygPath("/bin/mintty.exe").winPath();
	cmd_line << mintty.str() << L" -o Locale=C -o Charset=UTF-8"
	         << L" -t " << _escapeWinArg(script_name)
	         << L" --exec /bin/bash -il -c "
	         << _escapeWinArg(_getExecCmd(args));
	STARTUPINFO si = {};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {};
	if (0 == CreateProcess(
			NULL, (LPWSTR)cmd_line.str().c_str(), NULL, NULL, FALSE,
			CREATE_NEW_PROCESS_GROUP, NULL, script_dir.c_str(),
			&si, &pi)) {
		THROW_LAST_ERROR("Failed to start process");
	}
}

std::wstring ExecCommand::_getExecCmd(const std::vector<std::wstring> args) {
	/* Posix path is represented in UTF-8 encoding. However, we must
	   pass the path as a wide string to CreateProcess.
	   The trick is to interpret UTF-8 sequence as an ISO-8859-1
	   and encode it to UTF-16. When Cygwin decodes Windows command
	   line back to C-locale, it receives the original UTF-8 sequence.
	 */
	static const UINT codepage = 28591;
	std::wstringstream ss;
	WinPathW script_path = WinPathW(args[0]).longPath();
	std::string dir = script_path.dirname().cygPath().str();
	std::string filename = script_path.basename().cygPath(true).str();
	/* change directory to script's directory and invoke from there */
	ss << L"( cd "
	   << _escapePosixArg(mb_to_wide(dir, codepage)) << L" && "
	   << _escapePosixArg(mb_to_wide(std::string("./") + filename, codepage))
	   << L" ";
	/* remaining arguments */
	auto it = std::begin(args);
	auto end = std::end(args);
	std::advance(it, 1);
	for(; it < end; std::advance(it, 1)) {
		auto arg = *it;
		/* convert Windows paths to Cygwin form */
		if (_isWinPath(arg, true)) {
			std::string posix_path = WinPathW(arg).longPath().cygPath().str();
			arg = mb_to_wide(posix_path, codepage);
		}
		ss << _escapePosixArg(arg) << L" ";
	}
	/* if script exists with a non-zero exit code, keep terminal open
	   until keypress */
	ss << L") || { echo -e \"\\n[Process exited - exit code $?]\";"
	    " read -n 1 -s; }";
	return ss.str();
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

}

#include "winpath.hpp"
#include <windows.h>
#include <Shlwapi.h>
#include <sys/cygwin.h>
#include <stdexcept>
#include <memory>
#include "cygpath.hpp"
#include "winerror.hpp"

namespace cygextreg {

CygPath WinPathW::cygPath(bool keep_relative) const {
	cygwin_conv_path_t flags = CCP_WIN_W_TO_POSIX;
	if (keep_relative) {
		flags |= CCP_RELATIVE;
	}
	char buf[MAX_PATH + 1];
	if (0 == cygwin_conv_path(flags, _path.c_str(), buf, sizeof(buf))) {
		return CygPath(std::string(buf));
	}
	if (ENOSPC != errno) {
		throw std::runtime_error(strerror(errno));
	}
	size_t len = cygwin_conv_path(flags, _path.c_str(), NULL, 0);
	std::unique_ptr<char[]> tmp(new char[len]);
	if(0 != cygwin_conv_path(flags, _path.c_str(), tmp.get(), len)) {
		throw std::runtime_error(strerror(errno));
	}
	return CygPath(std::string(tmp.get()));
}

WinPathW WinPathW::longPath(bool extended) const {
	std::wstring path = _path;
	if (extended && path.substr(0, 4) != L"\\\\?\\") {
		path.insert(0, L"\\\\?\\");
	}
	wchar_t buf[MAX_PATH + 1];
	DWORD len = GetLongPathNameW(
		path.c_str(), buf, sizeof(buf) / sizeof(buf[0]));
	if (0 == len) {
		THROW_LAST_ERROR("Failed to get long path");
	}
	if (len < sizeof(buf) / sizeof(buf[0])) {
		return WinPathW(std::wstring(buf));
	}
	std::unique_ptr<wchar_t[]> tmp(new wchar_t[len]);
	len = GetLongPathNameW(path.c_str(), tmp.get(), len);
	if (0 == len) {
		THROW_LAST_ERROR("Failed to get long path");
	}
	return WinPathW(std::wstring(tmp.get()));
}

WinPathW WinPathW::dirname() const {
	std::unique_ptr<wchar_t[]> buf(new wchar_t[_path.size() + 1]);
	_path.copy(buf.get(), _path.size());
	buf[_path.size()] = L'\0';
	if (FALSE == PathRemoveFileSpec(buf.get())) {
		throw std::runtime_error("No filename to remove from path.");
	}
	return WinPathW(std::wstring(buf.get()));
}

WinPathW WinPathW::basename() const {
	std::unique_ptr<wchar_t[]> buf(new wchar_t[_path.size() + 1]);
	_path.copy(buf.get(), _path.size());
	buf[_path.size()] = L'\0';
	return WinPathW(std::wstring(PathFindFileName(buf.get())));
}

}

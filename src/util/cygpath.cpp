#include "cygpath.hpp"
#include <windows.h>
#include <sys/cygwin.h>
#include <exception>
#include <memory>
#include "winpath.hpp"

namespace cygregext {

CygPath::CygPath(const WinPathW &path) :
	_path(path.cygPath()) {
}

WinPathW CygPath::winPath(bool keep_relative) const {
	cygwin_conv_path_t flags = CCP_POSIX_TO_WIN_W;
	if (keep_relative) {
		flags |= CCP_RELATIVE;
	}
	wchar_t buf[MAX_PATH + 1];
	if (0 == cygwin_conv_path(flags, _path.c_str(), buf, sizeof(buf))) {
		return WinPathW(std::wstring(buf));
	}
	/* errors other than "no space" */
	if (ENOSPC != errno) {
		throw std::runtime_error(strerror(errno));
	}
	size_t len = cygwin_conv_path(flags, _path.c_str(), NULL, 0);
	std::unique_ptr<char[]> tmp(new char[len]);
	if(0 != cygwin_conv_path(flags, _path.c_str(), tmp.get(), len)) {
		throw std::runtime_error(strerror(errno));
	}
	return WinPathW(std::wstring(reinterpret_cast<wchar_t*>(tmp.get())));
}

}

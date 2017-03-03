#include "winerror.hpp"
#include "strconv.hpp"

namespace cygscript {

WinError::operator std::wstring() const {
	return _translateMessage();
}

WinError::operator std::string() const {
	return wide_to_mb(_translateMessage());
}

std::wstring WinError::_translateMessage() const {
	LPWSTR buf;
	std::wstring msg;
	DWORD ret = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, _err, 0, (LPWSTR)&buf, 0, NULL);
	if (ret) {
		msg = buf;
	}
	LocalFree(buf);
	return msg;
}

std::string operator+(const std::string &str, const WinError &err) {
	return str + (std::string)err;
}

}

#include "strconv.hpp"

namespace cygscript {

std::string wide_to_mb(std::wstring str, UINT codepage) {
	std::string out;
	char buf[MAX_PATH];
	int ret;
	/* try to fit string into stack buffer */
	ret = WideCharToMultiByte(
		codepage, 0, str.c_str(), -1, buf, sizeof(buf), NULL, NULL);
	if (ret) {
		out = std::string(buf, ret - 1);
	} else {
		DWORD err = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != err) {
			return out;
		}
		/* too small buffer, query required size and allocate from heap */
		ret = WideCharToMultiByte(
			codepage, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
		char* tmp = new char[ret];
		ret = WideCharToMultiByte(
			codepage, 0, str.c_str(), -1, tmp, ret, NULL, NULL);
		out = std::string(tmp, ret - 1);
		delete[] tmp;
	}
	return out;
}

std::wstring mb_to_wide(std::string str, UINT codepage) {
	std::wstring out;
	wchar_t buf[MAX_PATH];
	int ret;
	/* try to fit string into stack buffer */
	ret = MultiByteToWideChar(
		codepage, 0, str.c_str(), -1, buf, sizeof(buf) / sizeof(buf[0]));
	if (ret) {
		out = std::wstring(buf, ret - 1);
	} else {
		DWORD err = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != err) {
			return out;
		}
		/* too small buffer, query required size and allocate from heap */
		ret = MultiByteToWideChar(
			codepage, 0, str.c_str(), -1, NULL, 0);
		wchar_t* tmp = new wchar_t[ret];
		ret = MultiByteToWideChar(
			codepage, 0, str.c_str(), -1, tmp, ret);
		out = std::wstring(tmp, ret - 1);
		delete[] tmp;
	}
	return out;
}

}

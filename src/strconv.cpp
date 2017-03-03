#include "strconv.hpp"
#include <windows.h>

namespace cygscript {

std::string wide_to_mb(std::wstring str) {
	std::string out;
	char buf[64];
	int ret;
	/* try to fit string into stack buffer */
	ret = WideCharToMultiByte(CP_THREAD_ACP, 0,
	                          str.c_str(), -1, buf, sizeof(buf), NULL, NULL);
	if (ret) {
		out = std::string(buf, ret);
	} else {
		DWORD err = GetLastError();
		if (ERROR_INSUFFICIENT_BUFFER != err) {
			return out;
		}
		/* too small buffer, query required size and allocate from heap */
		ret = WideCharToMultiByte(CP_THREAD_ACP, 0,
		                          str.c_str(), -1, NULL, 0, NULL, NULL);
		char *tmp = (char *)malloc(ret + 1);
		ret = WideCharToMultiByte(CP_THREAD_ACP, 0,
		                          str.c_str(), -1, tmp, ret + 1, NULL, NULL);
		out = std::string(tmp, ret);
		free(tmp);
	}
	return out;
}

}

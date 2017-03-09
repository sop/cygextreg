#include "envvar.hpp"
#include "winerror.hpp"

namespace env {

bool EnvVar::exists() const {
	DWORD ret = GetEnvironmentVariable(_name.c_str(), NULL, 0);
	if (ret > 0) {
		return true;
	}
	if (ERROR_ENVVAR_NOT_FOUND != GetLastError()) {
		THROW_LAST_ERROR("GetEnvironmentVariable");
	}
	return false;
}

std::wstring EnvVar::get() const {
	wchar_t buf[64];
	std::wstring ret;
	DWORD len = GetEnvironmentVariable(
		_name.c_str(), buf, sizeof(buf) / sizeof(buf[0]));
	// not found or error
	if (0 == len) {
		if (ERROR_ENVVAR_NOT_FOUND != GetLastError()) {
			THROW_LAST_ERROR("GetEnvironmentVariable");
		}
		// return empty string for non-existing value
	}
	// buffer too small
	else if (len > sizeof(buf) / sizeof(buf[0])) {
		wchar_t* tmp = new wchar_t[len];
		len = GetEnvironmentVariable(_name.c_str(), tmp, len);
		if (0 == len) {
			THROW_LAST_ERROR("GetEnvironmentVariable");
		}
		ret.assign(tmp, len);
		delete[] tmp;
	} else {
		ret.assign(buf, len);
	}
	return ret;
}

const EnvVar& EnvVar::set(const std::wstring& val) const {
	if (0 == SetEnvironmentVariable(_name.c_str(), val.c_str())) {
		THROW_LAST_ERROR("SetEnvironmentVariable");
	}
	return *this;
}

const EnvVar& EnvVar::unset() const {
	if (0 == SetEnvironmentVariable(_name.c_str(), NULL)) {
		THROW_LAST_ERROR("SetEnvironmentVariable");
	}
	return *this;
}

}

#include "list.hpp"
#include <memory>
#include "util/strconv.hpp"
#include "util/message.hpp"

namespace cygscript {

int ListCommand::run() {
	std::wstringstream ss;
	std::wstring ext(L".sh");
	bool is_registered = false;
	int retval = 0;
	try {
		PredefinedKey cu(HKEY_CURRENT_USER);
		if (_isRegistered(cu, ext)) {
			is_registered |= true;
			ss << "Registered for the current user." << std::endl;
		}
	} catch (std::exception& e) {
		ss << "Failed to query HKEY_CURRENT_USER: " << mb_to_wide(e.what());
	}
	try {
		PredefinedKey lm(HKEY_LOCAL_MACHINE);
		if (_isRegistered(lm, ext)) {
			is_registered |= true;
			ss << "Registered for all users." << std::endl;
		}
	} catch (std::exception& e) {
		ss << "Failed to query HKEY_LOCAL_MACHINE: " << mb_to_wide(e.what());
	}
	if (!is_registered) {
		ss << "Not registered." << std::endl;
		retval = 1;
	}
	show_message(ss.str());
	return retval;
}

bool ListCommand::_isRegistered(const IKey& parent, const std::wstring& ext) {
	Key base(parent, L"Software\\Classes", KEY_QUERY_VALUE);
	if (!base.hasSubKey(ext)) {
		return false;
	}
	Key key(base, ext, KEY_QUERY_VALUE);
	if (key.getString(L"") != L"Cygwin.Script") {
		return false;
	}
	return true;
}

}

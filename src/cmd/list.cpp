#include "list.hpp"
#include <memory>
#include "util/strconv.hpp"
#include "util/message.hpp"
#include "util/winerror.hpp"

namespace cygscript {

int ListCommand::run() {
	std::wstringstream ss;
	std::wstring ext(L".sh");
	bool is_registered = false;
	int retval = 0;
	/* list extensions registered for the current user */
	try {
		PredefinedKey cu(HKEY_CURRENT_USER);
		std::vector<std::wstring> exts = _searchRegisteredExtensions(cu);
		if (exts.size()) {
			is_registered |= true;
			ss << "Extensions registered for the current user:" << std::endl;
			for (auto ext : exts) {
				ss << ext << std::endl;
			}
		}
	} catch (std::exception& e) {
		ss << "Failed to query HKEY_CURRENT_USER: " << mb_to_wide(e.what());
	}
	/* list extensions registered for all users */
	try {
		PredefinedKey lm(HKEY_LOCAL_MACHINE);
		std::vector<std::wstring> exts = _searchRegisteredExtensions(lm);
		if (exts.size()) {
			is_registered |= true;
			ss << "Extensions registered for all users:" << std::endl;
			for (auto ext : exts) {
				ss << ext << std::endl;
			}
		}
	} catch (std::exception& e) {
		ss << "Failed to query HKEY_LOCAL_MACHINE: " << mb_to_wide(e.what());
	}
	/* if no extensions were registered */
	if (!is_registered) {
		ss << "No extensions registered." << std::endl;
		retval = 1;
	}
	show_message(ss.str());
	return retval;
}

std::vector<std::wstring> ListCommand::_searchRegisteredExtensions(
	const IKey& root) {
	Key base(root, L"Software\\Classes", KEY_READ);
	wchar_t name[256];
	DWORD name_size;
	std::vector<std::wstring> actions;
	for(DWORD idx = 0;; ++idx) {
		name_size = sizeof(name) / sizeof(name[0]);
		LONG result = RegEnumKeyEx(
			base, idx, name, &name_size, NULL, NULL, NULL, NULL);
		if (ERROR_NO_MORE_ITEMS == result) {
			break;
		}
		else if (ERROR_MORE_DATA == result) {
			continue;
		}
		else if (ERROR_SUCCESS != result) {
			THROW_ERROR_CODE("Failed to enumerate registry", result);
		}
		if (0 == wcsncmp(name, L"cygscript.", 10)) {
			actions.push_back(name);
		}
	}
	std::vector<std::wstring> exts;
	for (auto action : actions) {
		std::wstring ext = action.substr(9);
		if (_isRegistered(root, ext)) {
			exts.push_back(ext);
		}
	}
	return exts;
}

bool ListCommand::_isRegistered(const IKey& root, const std::wstring& ext) {
	Key base(root, L"Software\\Classes", KEY_QUERY_VALUE);
	if (!base.hasSubKey(ext)) {
		return false;
	}
	Key key(base, ext, KEY_QUERY_VALUE);
	if (key.getString(L"") != std::wstring(L"cygscript") + ext) {
		return false;
	}
	return true;
}

}

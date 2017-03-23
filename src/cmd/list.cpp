#include "list.hpp"
#include <windows.h>
#include <memory>
#include "registry/key.hpp"
#include "registry/registry.hpp"
#include "util/strconv.hpp"
#include "util/message.hpp"

using namespace registry;

namespace cygscript {

int ListCommand::run() {
	std::wstringstream ss;
	bool is_registered = false;
	int retval = 0;
	/* list extensions registered for the current user */
	try {
		std::vector<std::wstring> exts =
			Registry(std::unique_ptr<IKey>(
						 new PredefinedKey(HKEY_CURRENT_USER)))
			    .searchRegisteredExtensions();
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
		std::vector<std::wstring> exts =
			Registry(std::unique_ptr<IKey>(
						 new PredefinedKey(HKEY_LOCAL_MACHINE)))
			    .searchRegisteredExtensions();
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

}

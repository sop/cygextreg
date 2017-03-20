#include "register.hpp"
#include <stdexcept>
#include <string.h>
#include <sys/cygwin.h>
#include <Shlwapi.h>
#include "registry/registry.hpp"
#include "app.hpp"
#include "util/message.hpp"
#include "util/cygpath.hpp"

namespace cygscript {

std::unique_ptr<IKey> BaseRegisterCommand::_getRootKey() const {
	if (_forAllUsers) {
		return std::unique_ptr<PredefinedKey>(
			new PredefinedKey(HKEY_LOCAL_MACHINE));
	}
	return std::unique_ptr<PredefinedKey>(
		new PredefinedKey(HKEY_CURRENT_USER));
}

int RegisterCommand::run() {
	Registry reg(std::move(_getRootKey()));
	if (reg.isRegisteredForOther(_extension) && !_force) {
		throw std::runtime_error(
				  wide_to_mb(_extension) + " extension is already"
				  " registered for another application."
				  " Use -f to override.");
	}
	reg.registerExtension(_extension, _getIconPath());
	show_message(_extension + L" extension registered.");
	return 0;
}

std::wstring RegisterCommand::_getIconPath() {
	if (!_iconPath.size()) {
		return _getDefaultIcon();
	}
	std::string path = _iconPath;
	unsigned int icon_idx = 0;
	size_t pos = path.find_last_of(",");
	if (std::string::npos != pos) {
		icon_idx = std::stoi(path.substr(pos + 1), nullptr);
		path = path.substr(0, pos);
	}
	std::wstring winpath;
	try {
		winpath = CygPath(path).winPath().longPath();
		if (FALSE == PathFileExists(winpath.c_str())) {
			throw std::exception();
		}
	} catch(...) {
		throw std::runtime_error(
				  std::string("Icon file ") + path + " doesn't exists.");
	}
	HICON icon = ExtractIcon(GetModuleHandle(NULL), winpath.c_str(), icon_idx);
	if (NULL == icon || (HICON)1 == icon) {
		throw std::runtime_error(
				  std::string("No icon found from ") + _iconPath + ".");
	}
	DestroyIcon(icon);
	return winpath + L"," + std::to_wstring(icon_idx);
}

std::wstring RegisterCommand::_getDefaultIcon() {
	std::wstring icon = App::getPath().longPath();
	icon += L",0";
	return icon;
}

int UnregisterCommand::run() {
	Registry reg(std::move(_getRootKey()));
	if (reg.isRegisteredForOther(_extension)) {
		throw std::runtime_error(
				  wide_to_mb(_extension) + " extension is registered for"
				  " another application.");
	}
	reg.unregisterExtension(_extension);
	show_message(_extension + L" extension unregistered.");
	return 0;
}

}

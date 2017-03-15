#include "register.hpp"
#include <stdexcept>
#include <string.h>
#include <sys/cygwin.h>
#include <Shlwapi.h>
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

bool BaseRegisterCommand::_isCygscriptExtension(const IKey& parent) const {
	if (!parent.hasSubKey(_extension)) {
		return false;
	}
	Key key(parent, _extension);
	std::wstring handler = std::wstring(L"cygscript") + _extension;
	if (key.getString(L"") != handler) {
		return false;
	}
	return true;
}

int RegisterCommand::run() {
	std::unique_ptr<IKey> root = _getRootKey();
	Key key(*root, L"Software\\Classes");
	/* check whether extension is already registered for
	   another application */
	if (!_force && key.hasSubKey(_extension)) {
		if (!_isCygscriptExtension(key)) {
			throw std::runtime_error(
					  wide_to_mb(_extension) + " extension is already"
					  " registered for another application."
					  " Use -f to override.");
		}
	}
	_registerAction(key);
	_registerExtension(key);
	show_message(_extension + L" extension registered.");
	return 0;
}

void RegisterCommand::_registerAction(const IKey& parent) {
	std::wstring icon = _getIconPath();
	std::wstring kname = std::wstring(L"cygscript") + _extension;
	std::wstring desc = std::wstring(
		L"Cygwin Shell Script (" + _extension + L")");
	/* base key */
	Key base = Key::create(parent, kname)
	               .setString(std::wstring(), desc)
	               .setDword(L"EditFlags", 0x30);
	/* default icon*/
	Key::create(base, L"DefaultIcon")
	    .setString(std::wstring(), icon);
	/* open command */
	Key::create(base, L"shell")
	    .setString(std::wstring(), L"open");
	Key::create(base, L"shell\\open\\command")
	    .setString(std::wstring(), _getOpenCommand());
	/* drop handler */
	Key::create(base, L"shellex\\DropHandler")
	    .setString(std::wstring(), L"{86C86720-42A0-1069-A2E8-08002B30309D}");
}


void RegisterCommand::_registerExtension(const IKey& parent) {
	std::wstring handler = std::wstring(L"cygscript") + _extension;
	Key::create(parent, _extension)
	    .setString(std::wstring(), handler);
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

std::wstring RegisterCommand::_getOpenCommand() {
	std::wstring path = App::getPath().longPath();
	std::wstringstream ss;
	ss << L"\"" << path << L"\"" << L" --exec -- \"%1\" %*";
	return ss.str();
}

int UnregisterCommand::run() {
	std::unique_ptr<IKey> root = _getRootKey();
	Key key(*root, L"Software\\Classes");
	if (!key.hasSubKey(_extension)) {
		show_message(_extension + L" extension is not registered.");
		return 1;
	}
	_unregisterExtension(key);
	_unregisterAction(key);
	show_message(_extension + L" extension unregistered.");
	return 0;
}

void UnregisterCommand::_unregisterAction(const IKey& parent) {
	std::wstring kname = std::wstring(L"cygscript") + _extension;
	if (!parent.hasSubKey(kname)) {
		throw std::runtime_error(std::string("Extension ") +
		                         wide_to_mb(_extension) + " not registered.");
	}
	parent.deleteSubTree(kname);
}

void UnregisterCommand::_unregisterExtension(const IKey& parent) {
	if (!_isCygscriptExtension(parent)) {
		throw std::runtime_error("Not a cygscript extension.");
	}
	parent.deleteSubTree(_extension);
}

}

#include "register.hpp"
#include <stdexcept>
#include <memory>
#include <string.h>
#include <sys/cygwin.h>
#include "app.hpp"
#include "util/strconv.hpp"
#include "util/message.hpp"
#include "util/cygpath.hpp"

namespace cygscript {

int RegisterCommand::run() {
	std::unique_ptr<IKey> root;
	std::wstring ext = mb_to_wide(_extension);
	if (_forAllUsers) {
		root = std::unique_ptr<PredefinedKey>(
			new PredefinedKey(HKEY_LOCAL_MACHINE));
	} else {
		root = std::unique_ptr<PredefinedKey>(
			new PredefinedKey(HKEY_CURRENT_USER));
	}
	Key key(*root, L"Software\\Classes");
	switch (_cmd) {
	case Command::REGISTER:
		/* check whether extension is already registered for
		   another application */
		if (!_force && key.hasSubKey(ext)) {
			if (!_isCygscriptExtension(key, ext)) {
				throw std::runtime_error(
						  _extension + " extension is already registered for"
						  " another application. Use -f to override.");
			}
		}
		_registerAction(key, ext);
		_registerExtension(key, ext);
		show_message(ext + L" extension registered.");
		break;
	case Command::UNREGISTER:
		if (!key.hasSubKey(ext)) {
			show_message(ext + L" extension is not registered.");
			return 1;
		}
		_unregisterExtension(key, ext);
		_unregisterAction(key, ext);
		show_message(ext + L" extension unregistered.");
		break;
	}
	return 0;
}

void RegisterCommand::_registerAction(const IKey& parent,
                                      const std::wstring& ext) {
	std::wstring kname = std::wstring(L"cygscript") + ext;
	std::wstring desc = std::wstring(L"Cygwin Shell Script (" + ext + L")");
	/* base key */
	Key base = Key::create(parent, kname)
	               .setString(std::wstring(), desc)
	               .setDword(L"EditFlags", 0x30);
	/* default icon*/
	Key::create(base, L"DefaultIcon")
	    .setString(std::wstring(), _getDefaultIcon());
	/* open command */
	Key::create(base, L"shell")
	    .setString(std::wstring(), L"open");
	Key::create(base, L"shell\\open\\command")
	    .setString(std::wstring(), _getOpenCommand());
	/* drop handler */
	Key::create(base, L"shellex\\DropHandler")
	    .setString(std::wstring(), L"{86C86720-42A0-1069-A2E8-08002B30309D}");
}

void RegisterCommand::_unregisterAction(const IKey& parent,
                                        const std::wstring& ext) {
	std::wstring kname = std::wstring(L"cygscript") + ext;
	if (!parent.hasSubKey(kname)) {
		throw std::runtime_error(std::string("Extension ") +
		                         wide_to_mb(ext) + " not registered.");
	}
	parent.deleteSubTree(kname);
}

void RegisterCommand::_registerExtension(const IKey& parent,
                                         const std::wstring& ext) {
	std::wstring handler = std::wstring(L"cygscript") + ext;
	Key::create(parent, ext)
	    .setString(std::wstring(), handler);
}

void RegisterCommand::_unregisterExtension(const IKey& parent,
                                           const std::wstring& ext) {
	if (!_isCygscriptExtension(parent, ext)) {
		throw std::runtime_error("Not a cygscript extension.");
	}
	parent.deleteSubTree(ext);
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

bool RegisterCommand::_isCygscriptExtension(const IKey& parent,
                                            const std::wstring& ext) {
	if (!parent.hasSubKey(ext)) {
		return false;
	}
	Key key(parent, ext);
	std::wstring handler = std::wstring(L"cygscript") + ext;
	if (key.getString(L"") != handler) {
		return false;
	}
	return true;
}

}

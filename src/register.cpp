#include "register.hpp"
#include <stdexcept>
#include <memory>
#include <string.h>
#include <sys/cygwin.h>

namespace cygscript {

int RegisterCommand::run() {
	std::unique_ptr<IKey> root;
	if (_forAllUsers) {
		root = std::unique_ptr<PredefinedKey>(
			new PredefinedKey(HKEY_LOCAL_MACHINE));
	} else {
		root = std::unique_ptr<PredefinedKey>(
			new PredefinedKey(HKEY_CURRENT_USER));
	}
	Key key(*root, L"Software\\Classes");
	_registerAction(key);
	_registerExtension(key, L".sh");
	return 0;
}

void RegisterCommand::_registerAction(const IKey& parent) {
	/* base key */
	Key base = Key::create(parent, L"Cygwin.Script")
	               .setString(std::wstring(), L"Cygwin Shell Script")
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

void RegisterCommand::_registerExtension(const IKey& parent,
                                         const std::wstring& ext) {
	Key::create(parent, ext)
	    .setString(std::wstring(), L"Cygwin.Script");
}

std::wstring RegisterCommand::_getDefaultIcon() {
	wchar_t buf[MAX_PATH + 16];
	if(0 != cygwin_conv_path(
		   CCP_POSIX_TO_WIN_W, "/Cygwin-Terminal.ico", buf, sizeof(buf))) {
		throw std::runtime_error(strerror(errno));
	}
	std::wstring str(buf);
	str += L",0";
	return str;
}

std::wstring RegisterCommand::_getOpenCommand() {
	wchar_t buf[MAX_PATH];
	int ret = GetModuleFileName(NULL, buf, sizeof(buf));
	if (0 == ret) {
		THROW_LAST_ERROR("Failed to get executable path");
	}
	if (sizeof(buf) == ret && ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
		throw std::runtime_error("Failed to get executable path");
	}
	std::wstringstream ss;
	ss << L"\"" << std::wstring(buf, ret) << L"\"" << L" --exec \"%1\" %*";
	return ss.str();
}

}

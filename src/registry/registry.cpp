#include "registry.hpp"
#include "app.hpp"
#include "util/strconv.hpp"
#include "util/winerror.hpp"

namespace cygscript
{

void Registry::registerExtension(const std::wstring& ext,
                                 const std::wstring& icon) {
	Key base = _getClassBase();
	std::wstring handler_name = std::wstring(L"cygscript") + ext;
	std::wstring desc = std::wstring(L"Cygwin Shell Script (" + ext + L")");
	/* handler key */
	Key handler = Key::create(base, handler_name)
	                  .setString(std::wstring(), desc)
	                  .setDword(L"EditFlags", 0x30);
	/* default icon*/
	Key::create(handler, L"DefaultIcon")
	    .setString(std::wstring(), icon);
	/* open command */
	Key::create(handler, L"shell")
	    .setString(std::wstring(), L"open");
	Key::create(handler, L"shell\\open\\command")
	    .setString(std::wstring(), _getOpenCommand());
	/* drop handler */
	Key::create(handler, L"shellex\\DropHandler")
	    .setString(std::wstring(), L"{86C86720-42A0-1069-A2E8-08002B30309D}");
	/* extension key */
	Key::create(base, ext)
	    .setString(std::wstring(), handler_name);
}

void Registry::unregisterExtension(const std::wstring& ext) {
	Key base = _getClassBase();
	std::wstring handler_name = std::wstring(L"cygscript") + ext;
	if (!base.hasSubKey(handler_name)) {
		throw std::runtime_error(std::string("Extension ") +
		                         wide_to_mb(ext) + " is not registered.");
	}
	base.deleteSubTree(handler_name);
	base.deleteSubTree(ext);
}

std::vector<std::wstring> Registry::searchRegisteredExtensions() {
	Key base = _getClassBase(KEY_READ);
	wchar_t name[256];
	DWORD name_size;
	std::vector<std::wstring> handlers;
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
			handlers.push_back(name);
		}
	}
	std::vector<std::wstring> exts;
	for (auto handler : handlers) {
		std::wstring ext = handler.substr(9);
		if (base.hasSubKey(ext) && !isRegisteredForOther(ext)) {
			exts.push_back(ext);
		}
	}
	return exts;
}

bool Registry::isRegisteredForOther(const std::wstring& ext) {
	Key base = _getClassBase(KEY_QUERY_VALUE);
	/* if extension is not registered at all */
	if (!base.hasSubKey(ext)) {
		return false;
	}
	Key ext_key(base, ext, KEY_QUERY_VALUE);
	/* if extension doesn't have cygscript handler */
	std::wstring handler_name = std::wstring(L"cygscript") + ext;
	if (ext_key.getString(L"") != handler_name) {
		return true;
	}
	/* get handler's open command program */
	std::wstring command;
	try {
		command = Key(base, handler_name + L"\\shell\\open\\command",
		              KEY_QUERY_VALUE).getString(L"");
	} catch(...) {
		return true;
	}
	/* if there's no open command */
	if (!command.size()) {
		return true;
	}
	int argc;
	LPWSTR* argv = CommandLineToArgvW(command.c_str(), &argc);
	if (NULL == argv || argc < 1) {
		return true;
	}
	std::wstring prog(argv[0]);
	/* if open command differs from this instance */
	if (App::getPath().longPath().str() != prog) {
		return true;
	}
	return false;
}

Key Registry::_getClassBase(REGSAM access) {
	return Key(*_rootKey, L"Software\\Classes", access);
}

std::wstring Registry::_getOpenCommand() {
	std::wstring path = App::getPath().longPath();
	std::wstringstream ss;
	ss << L"\"" << path << L"\"" << L" --exec -- \"%1\" %*";
	return ss.str();
}

}

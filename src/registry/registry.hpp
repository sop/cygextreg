#include "key.hpp"
#include <memory>
#include <vector>
#include <string>

using namespace registry;

namespace cygregext
{

class Registry
{
	std::unique_ptr<IKey> _rootKey;

	/**
	 * Prefix for registered handlers.
	 */
	const std::wstring _handlerPrefix = std::wstring(L"cygregext");

public:
	/**
	 * Constructor.
	 *
	 * @param std::unique_ptr<IKey> root Root key, either
	 *        HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE
	 */
	Registry(std::unique_ptr<IKey> root) :
		_rootKey(std::move(root)) {
	}

	/**
	 * Register extension.
	 *
	 * @param const std::wstring& ext Extension
	 * @param const std::wstring& icon Path to icon
	 */
	void registerExtension(const std::wstring& ext, const std::wstring& icon);

	/**
	 * Unregister extension.
	 *
	 * @param const std::wstring& ext Extension
	 */
	void unregisterExtension(const std::wstring& ext);

	/**
	 * Search for registered extensions.
	 *
	 * @return std::vector<std::wstring> List of extensions
	 */
	std::vector<std::wstring> searchRegisteredExtensions();

	/**
	 * Check whether extensions is registered for another application.
	 *
	 * @param const std::wstring& ext Extension
	 * @return bool True if registered for another application
	 */
	bool isRegisteredForOther(const std::wstring& ext);

private:
	/**
	 * Get base Software\Classes registry key to operate with.
	 *
	 * @param REGSAM access Desired permissions
	 * @return Key
	 */
	Key _getClassBase(REGSAM access = KEY_ALL_ACCESS);

	/**
	 * Get the command line for open command.
	 *
	 * @return std::wstring Command
	 */
	std::wstring _getOpenCommand();
};

}

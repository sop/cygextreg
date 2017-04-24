#include "command.hpp"
#include <string>
#include <memory>
#include <stdexcept>
#include "registry/key.hpp"
#include "util/strconv.hpp"

using namespace registry;

namespace cygregext {

class BaseRegisterCommand : public ICommand
{
public:
	/**
	 * Constructor.
	 *
	 * @param const std::string& ext Extension
	 * @param bool forAll Whether to register or unregister for all users
	 */
	BaseRegisterCommand(const std::string& ext, bool forAll) :
		_extension(mb_to_wide(ext)),
		_forAllUsers(forAll) {
		if (L'.' != _extension[0]) {
			throw std::runtime_error("Extension must start with a dot.");
		}
	}
	virtual ~BaseRegisterCommand() {
	}

protected:
	std::wstring _extension;
	bool _forAllUsers;

	/**
	 * Get root registry key.
	 *
	 * Shall open either HKEY_LOCAL_MACHINE or HKEY_CURRENT_USER.
	 *
	 * @return IKey Registry key
	 */
	std::unique_ptr<IKey> _getRootKey() const;
};

class RegisterCommand : public BaseRegisterCommand
{
	std::string _iconPath;
	bool _force;

public:
	/**
	 * Constructor.
	 *
	 * @param const std::string& ext Extension
	 * @param const std::string& icon Path and index to icon
	 * @param bool forAll Whether to register for all users
	 * @param bool force Force registration if extension is already registered
	 */
	RegisterCommand(const std::string& ext, const std::string& icon,
	                bool forAll, bool force) :
		BaseRegisterCommand(ext, forAll),
		_iconPath(icon),
		_force(force) {
	}

	/**
	 * Run command.
	 *
	 * @return int Exit code
	 */
	int run();

private:
	/**
	 * Get path and index to script's icon.
	 *
	 * @return std::wstring Icon path with ",<idx>" suffix
	 */
	std::wstring _getIconPath();

	/**
	 * Get path to default icon for the filetype.
	 *
	 * @return std::wstring Path
	 */
	std::wstring _getDefaultIcon();
};

class UnregisterCommand : public BaseRegisterCommand
{
public:
	/**
	 * Constructor.
	 *
	 * @param const std::string& ext Extension
	 * @param bool forAll Whether to unregister from all users
	 */
	UnregisterCommand(const std::string& ext, bool forAll) :
		BaseRegisterCommand(ext, forAll) {
	}

	/**
	 * Run command.
	 *
	 * @return int Exit code
	 */
	int run();
};

}

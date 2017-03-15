#include "command.hpp"
#include <string>
#include <exception>
#include "registry/key.hpp"

using namespace registry;

namespace cygscript {

class RegisterCommand : public ICommand
{
public:
	enum class Command { REGISTER, UNREGISTER };

	/**
	 * Constructor.
	 *
	 * @param Command cmd Command to perform
	 * @param const std::string& ext Extension
	 * @param bool forAll Whether to register or unregister for all users
	 * @param bool force Force registration if extension is already registered
	 */
	RegisterCommand(Command cmd, const std::string& ext,
	                bool forAll, bool force) :
		_cmd(cmd),
		_extension(ext),
		_forAllUsers(forAll),
		_force(force) {
		if ('.' != _extension[0]) {
			throw std::runtime_error("Extension must start with a dot.");
		}
	}

	/**
	 * Run command.
	 *
	 * @return int Exit code
	 */
	int run();
private:
	Command _cmd;
	std::string _extension;
	bool _forAllUsers;
	bool _force;

	/**
	 * Create handler key into registry.
	 *
	 * @param const IKey& parent Parent key
	 * @param const std::wstring& ext Extension
	 */
	void _registerAction(const IKey& parent, const std::wstring& ext);

	/**
	 * Remove handler key from registry.
	 *
	 * @param const IKey& parent Parent key
	 * @param const std::wstring& ext Extension
	 */
	void _unregisterAction(const IKey& parent, const std::wstring& ext);

	/**
	 * Create extension key into registry.
	 *
	 * @param const IKey& parent Parent key
	 * @param const std::wstring& ext Extension
	 */
	void _registerExtension(const IKey& parent, const std::wstring& ext);

	/**
	 * Remove extension key from registry.
	 *
	 * @param const IKey& parent Parent key
	 * @param const std::wstring& ext Extension
	 */
	void _unregisterExtension(const IKey& parent, const std::wstring& ext);

	/**
	 * Get path to default icon for the filetype.
	 *
	 * @return std::wstring Path
	 */
	std::wstring _getDefaultIcon();

	/**
	 * Get the command line for open command.
	 *
	 * @return std::wstring Command
	 */
	std::wstring _getOpenCommand();

	/**
	 * Check whether given extension is registered for cygscript.
	 *
	 * @param const IKey& parent Parent key
	 * @param const std::wstring& ext Extension
	 * @return bool True if registered for cygscript
	 */
	bool _isCygscriptExtension(const IKey& parent, const std::wstring& ext);
};

}

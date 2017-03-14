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
	RegisterCommand(Command cmd, std::string ext, bool forAll, bool force) :
		_cmd(cmd),
		_extension(ext),
		_forAllUsers(forAll),
		_force(force) {
		if ('.' != _extension[0]) {
			throw std::runtime_error("Extension must start with a dot.");
		}
	}
	~RegisterCommand() {
	}
	int run();
private:
	Command _cmd;
	std::string _extension;
	bool _forAllUsers;
	bool _force;
	void _registerAction(const IKey& parent, const std::wstring& ext);
	void _unregisterAction(const IKey& parent, const std::wstring& ext);
	void _registerExtension(const IKey& parent, const std::wstring& ext);
	void _unregisterExtension(const IKey& parent, const std::wstring& ext);
	std::wstring _getDefaultIcon();
	std::wstring _getOpenCommand();
	bool _isCygscriptExtension(const IKey& parent, const std::wstring& ext);
};

}

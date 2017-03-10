#include "command.hpp"
#include "registry/key.hpp"

using namespace registry;

namespace cygscript {

class RegisterCommand : public ICommand
{
public:
	enum class Command { REGISTER, UNREGISTER };
	RegisterCommand(Command cmd, bool forAll) :
		_cmd(cmd),
		_forAllUsers(forAll) {
	}
	~RegisterCommand() {
	}
	int run();
private:
	Command _cmd;
	bool _forAllUsers;
	void _registerAction(const IKey& parent);
	void _unregisterAction(const IKey& parent);
	void _registerExtension(const IKey& parent, const std::wstring& ext);
	void _unregisterExtension(const IKey& parent, const std::wstring& ext);
	std::wstring _getDefaultIcon();
	std::wstring _getOpenCommand();
};

}

#include <iostream>
#include "command.hpp"
#include "registry/key.hpp"

using namespace registry;

namespace cygscript {

class RegisterCommand : public ICommand
{
	bool _forAllUsers;
public:
	RegisterCommand(bool forAll) : _forAllUsers(forAll) {
	}
	~RegisterCommand() {
	}
	int run();
private:
	void _registerAction(const IKey& parent);
	void _registerExtension(const IKey& parent, const std::wstring& ext);
	std::wstring _getDefaultIcon();
	std::wstring _getOpenCommand();
};

}

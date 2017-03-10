#include "command.hpp"
#include "registry/key.hpp"

using namespace registry;

namespace cygscript {

class ListCommand : public ICommand
{
public:
	ListCommand() {
	}
	~ListCommand() {
	}
	int run();
private:
	bool _isRegistered(const IKey& parent, const std::wstring& ext);
};

}

#include "command.hpp"
#include <vector>
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
	std::vector<std::wstring> _searchRegisteredExtensions(const IKey& root);
	bool _isRegistered(const IKey& parent, const std::wstring& ext);
};

}

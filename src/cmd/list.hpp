#include "command.hpp"
#include <vector>
#include "registry/key.hpp"

using namespace registry;

namespace cygscript {

class ListCommand : public ICommand
{
public:
	/**
	 * Run command.
	 *
	 * @return int Exit code
	 */
	int run();
private:
	/**
	 * Search for registered cygscript extensions.
	 *
	 * @param const IKey& root Root registry key
	 * @return std::vector<std::wstring> List of extensions
	 */
	std::vector<std::wstring> _searchRegisteredExtensions(const IKey& root);

	/**
	 * Check whether extension is registered for cygscript.
	 *
	 * @param const IKey& root Root registry key
	 * @param const std::wstring& ext Extension
	 * @return bool True if registered
	 */
	bool _isRegistered(const IKey& root, const std::wstring& ext);
};

}

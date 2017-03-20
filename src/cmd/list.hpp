#include "command.hpp"
#include "app.hpp"
#include <vector>
#include "registry/key.hpp"

using namespace registry;

namespace cygscript {

class ListCommand : public ICommand
{
public:
	/**
	 * Constructor.
	 */
	ListCommand() :
		_progPath(App::getPath().longPath()) {
	}
	/**
	 * Run command.
	 *
	 * @return int Exit code
	 */
	int run();
private:
	std::wstring _progPath;

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

	/**
	 * Get program path that is registered for open command.
	 *
	 * @param const IKey& handler Handler registry key
	 * @return std::wstring Path
	 */
	std::wstring _getOpenCommandProg(const IKey& handler);
};

}

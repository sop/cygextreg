#include "command.hpp"

namespace cygregext {

class ListCommand : public ICommand
{
public:
	/**
	 * Run command.
	 *
	 * @return int Exit code
	 */
	int run();
};

}

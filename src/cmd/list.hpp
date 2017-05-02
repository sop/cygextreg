#include "command.hpp"

namespace cygextreg {

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

#include "command.hpp"

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
};

}

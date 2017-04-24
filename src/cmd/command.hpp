#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

namespace cygregext {

class ICommand
{
public:
	virtual ~ICommand() {
	}

	/**
	 * Run the command and return exit code.
	 *
	 * @return int Exit code
	 */
	virtual int run() = 0;
};

}

#endif

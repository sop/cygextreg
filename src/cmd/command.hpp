#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

namespace cygscript {

class ICommand {
public:
	virtual ~ICommand() {
	}
	virtual int run() = 0;
};

}

#endif

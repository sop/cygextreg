#include "command.hpp"
#include <vector>
#include <string>

namespace cygscript {

class ExecCommand : public ICommand
{
	std::vector<std::string> _args;
public:
	ExecCommand(std::vector<std::string> args) : _args(args) {
	}
	~ExecCommand() {
	}
	int run();
private:
	int _execute(std::vector<std::string> args);
	bool _isWinPath(std::string path, bool must_exist);
	bool _fileExists(std::string path);
};

}

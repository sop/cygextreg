#include <vector>
#include <string>

namespace cygscript {

class App
{
	char* const* _argv;
	std::vector<std::string> _execArgs;
	enum class Command { NONE, EXEC, REGISTER };
	Command _cmd;
	enum class RegisterType { USER, EVERYONE };
	RegisterType _regType;
public:
	App(const int argc, char* const argv[]);
	int run();
private:
	void _printUsage(char* progname);
	void _checkElevated();
};

}

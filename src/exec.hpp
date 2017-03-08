#include "command.hpp"
#include <vector>
#include <string>

namespace cygscript {

class ExecCommand : public ICommand
{
	std::vector<std::wstring> _args;
public:
	ExecCommand(std::vector<std::wstring> args) : _args(args) {
	}
	~ExecCommand() {
	}
	int run();
private:
	std::vector<std::wstring> _getExecArgs();
	int _execute(std::vector<std::wstring> args);
	bool _isWinPath(std::wstring path, bool must_exist);
	std::wstring _pathWinToPosix(std::wstring winpath);
	std::wstring _escapeArg(std::wstring arg);
	void _replaceAll(std::wstring& str, const std::wstring& from,
	                 const std::wstring& to);
};

}

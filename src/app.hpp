#ifndef __APP_HPP__
#define __APP_HPP__

#include <vector>
#include <string>
#include "util/winpath.hpp"

namespace cygscript {

class App
{
	int _argc;
	char* const* _argv;
	enum class Command { NONE, EXEC, REGISTER, UNREGISTER, LIST };
	Command _cmd;
	enum class RegisterType { USER, EVERYONE };
	RegisterType _regType;
	std::string _extension;
	std::string _iconPath;
	bool _force;

public:
	/**
	 * Constructor.
	 *
	 * @param const int argc Argc from main function
	 * @param char* const argv[] Argv from main function
	 */
	App(const int argc, char* const argv[]);

	/**
	 * Run application.
	 *
	 * @return int Exit code
	 */
	int run();

	/**
	 * Get Windows path to this application.
	 *
	 * @return WinPathW Windows path
	 */
	static WinPathW getPath();

private:
	/**
	 * Print application usage.
	 */
	void _printUsage(char* progname);

	/**
	 * Print application version.
	 */
	void _printVersion();

	/**
	 * Get the command line argumenst in wide strings.
	 *
	 * @return std::vector<std::wstring> List of arguments
	 */
	std::vector<std::wstring> _wideArgs();

	/**
	 * Check whether process is currently running as elevated.
	 *
	 * Prompts for elevation if not currently elevated.
	 */
	void _checkElevated();
};

}

#endif

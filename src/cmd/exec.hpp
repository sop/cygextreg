#include "command.hpp"
#include <vector>
#include <string>

namespace cygscript {

class ExecCommand : public ICommand
{
	std::vector<std::wstring> _args;

public:
	/**
	 * Constructor.
	 *
	 * @param std::vector<std::wstring> args Wide arguments from main function
	 */
	ExecCommand(std::vector<std::wstring> args) : _args(args) {
	}

	/**
	 * Run command.
	 *
	 * @return int Exit code
	 */
	int run();

private:
	/**
	 * Get the arguments for script execution.
	 *
	 * @return std::vector<std::wstring> A list of wstring arguments
	 */
	std::vector<std::wstring> _getExecArgs();

	/**
	 * Get the command line for /bin/bash.
	 *
	 * @param const std::vector<std::wstring> args Wide arguments
	 * @return std::wstring Command line
	 */
	std::wstring _getExecCmd(const std::vector<std::wstring> args);

	/**
	 * Execute script using a given command line.
	 *
	 * @param std::wstring cmd_line Command line
	 * @throws std::runtime_error on failure
	 */
	void _execute(const std::wstring& cmd_line);

	/**
	 * Check whether given path exists and is a regular file or symlink.
	 *
	 * @param std::wstring path Path
	 * @return bool True if file exists
	 */
	bool _fileExists(const std::wstring& path);

	/**
	 * Check whether given path is in Windows format.
	 *
	 * @param std::wstring path Path
	 * @param bool must_exist Whether file must exist
	 * @return bool True if path is in Windows format
	 */
	bool _isWinPath(const std::wstring& path, bool must_exist);

	/**
	 * Convert Windows path to POSIX format.
	 *
	 * @param std::wstring path Windows formatted path
	 * @return std::string POSIX path in UTF-8 charset
	 */
	std::string _pathWinToPosix(const std::wstring& path);

	/**
	 * Convert 8.3 short path to long path format.
	 *
	 * @param const std::wstring& path Short path
	 * @return std::wstring Long path
	 */
	std::wstring _toLongPath(const std::wstring& path);

	/**
	 * Escape command line argument with Windows semantics.
	 *
	 * @param std::wstring arg Argument
	 * @return std::wstring Escaped argument
	 */
	std::wstring _escapeWinArg(const std::wstring& arg);

	/**
	 * Escape command line argument with Posix semantics.
	 *
	 * @param std::wstring arg Argument
	 * @return std::wstring Escaped argument
	 */
	std::wstring _escapePosixArg(const std::wstring& arg);

	/**
	 * Replace all occurences of a string.
	 *
	 * @param std::wstring& str String to modify
	 * @param const std::wstring& from Substring to search
	 * @param const std::wstring& to Replacement string
	 */
	void _replaceAll(std::wstring& str, const std::wstring& from,
	                 const std::wstring& to);

	/**
	 * Get readable script name for the shell window title.
	 *
	 * @return std::wstring
	 */
	std::wstring _getScriptName(const std::wstring& path);
};

}

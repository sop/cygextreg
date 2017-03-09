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
	/**
	 * Get the arguments for script execution.
	 *
	 * @return A list of wstring arguments
	 */
	std::vector<std::wstring> _getExecArgs();

	/**
	 * Execute script using a given command line.
	 *
	 * @param std::wstring cmd_line Command line
	 * @throws std::runtime_error on failure
	 */
	void _execute(std::wstring cmd_line);

	/**
	 * Check whether given path exists and is a regular file or symlink.
	 *
	 * @param std::wstring path Path
	 * @return bool True if file exists
	 */
	bool _fileExists(std::wstring path);

	/**
	 * Check whether given path is in Windows format.
	 *
	 * @param std::wstring path Path
	 * @param bool must_exist Whether file must exist
	 * @return bool True if path is in Windows format
	 */
	bool _isWinPath(std::wstring path, bool must_exist);

	/**
	 * Convert Windows path to POSIX format.
	 *
	 * @param std::wstring winpath Windows formatted path
	 * @return std::string POSIX path in UTF-8 charset
	 */
	std::string _pathWinToPosix(std::wstring winpath);

	/**
	 * Escape command line argument with Windows semantics.
	 *
	 * @param std::wstring arg Argument
	 * @return std::wstring Escaped argument
	 */
	std::wstring _escapeArg(std::wstring arg);

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
	 * Get PATH environment variable for to be set before executing script.
	 *
	 * @return std::wstring
	 */
	std::wstring _getEnvPath();

	/**
	 * Get readable script name for the shell window title.
	 *
	 * @return std::wstring
	 */
	std::wstring _getScriptName(std::wstring path);
};

}

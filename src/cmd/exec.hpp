#include "command.hpp"
#include <vector>
#include <string>
#include "settings/settings.hpp"

namespace cygextreg {

class ExecCommand : public ICommand
{
	std::vector<std::wstring> _args;

	Settings _settings;

public:
	/**
	 * Constructor.
	 *
	 * @param std::vector<std::wstring> args Wide arguments from main function
	 * @param const Settings& settings Settings
	 */
	ExecCommand(std::vector<std::wstring> args,
	            const Settings& settings) :
		_args(args),
		_settings(settings) {
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
	 * Execute script using a given command line.
	 *
	 * @param std::wstring cmd_line Command line
	 * @throws std::runtime_error on failure
	 */
	void _execute(const std::vector<std::wstring> args);

	/**
	 * Get the command line for /bin/bash.
	 *
	 * @param const std::vector<std::wstring> args Wide arguments
	 * @return std::wstring Command line
	 */
	std::wstring _getExecCmd(const std::vector<std::wstring> args);

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
};

}

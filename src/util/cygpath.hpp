#ifndef __UTIL__CYGPATH_HPP__
#define __UTIL__CYGPATH_HPP__

#include <string>
#include "winpath.hpp"

namespace cygscript {

class WinPathW;

class CygPath
{
	std::string _path;
public:
	/**
	 * Constructor.
	 *
	 * @param const std::string& path Path in Cygwin format
	 */
	CygPath(const std::string& path) :
		_path(path) {
	}

	/**
	 * Constructor.
	 *
	 * @param const WinPathW& path Windows path
	 */
	CygPath(const WinPathW& path);

	/**
	 * Cast to string.
	 *
	 * @return std::string Path
	 */
	operator std::string() {
		return _path;
	}

	/**
	 * Get path as a string.
	 *
	 * @return std::string Path
	 */
	std::string str() const {
		return _path;
	}

	/**
	 * Convert to Windows path.
	 *
	 * @param bool keep_relative Whether to keep path relative
	 * @return WinPathW Windows path
	 */
	WinPathW winPath(bool keep_relative = false) const;
};

}

#endif

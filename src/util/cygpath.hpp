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
	CygPath(const std::string& path) :
		_path(path) {
	}
	CygPath(const WinPathW& path);
	~CygPath() {
	}
	operator std::string() {
		return _path;
	}
	std::string str() const {
		return _path;
	}
	WinPathW winPath(bool keep_relative = false) const;
};

}

#endif

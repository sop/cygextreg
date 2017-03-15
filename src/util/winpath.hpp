#ifndef __UTIL__WINPATH_HPP__
#define __UTIL__WINPATH_HPP__

#include <string>
#include "cygpath.hpp"

namespace cygscript {

class CygPath;

class IWinPath
{
public:
	IWinPath() {
	}
	virtual ~IWinPath() {
	}
};

class WinPathW : public IWinPath
{
	std::wstring _path;
public:
	WinPathW(const std::wstring& path) :
		_path(path) {
	}
	~WinPathW() {
	}
	operator std::wstring() {
		return _path;
	}
	std::wstring str() const {
		return _path;
	}
	CygPath cygPath(bool keep_relative = false) const;
	WinPathW longPath() const;
};

}

#endif

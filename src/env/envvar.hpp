#ifndef __ENVVAR_HPP__
#define __ENVVAR_HPP__

#include <windows.h>
#include <string>

namespace env {

class EnvVar
{
public:
	EnvVar(const std::wstring& name) :
		_name(name) {
	};
	~EnvVar() {
	};
	bool exists() const;
	std::wstring get() const;
	const EnvVar& set(const std::wstring& val) const;
	const EnvVar& unset() const;
private:
	std::wstring _name;
};

}

#endif

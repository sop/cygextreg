#ifndef __ENV__ENVVAR_HPP__
#define __ENV__ENVVAR_HPP__

#include <windows.h>
#include <string>

namespace env {

class EnvVar
{
public:
	/**
	 * Constructor.
	 *
	 * @param const std::wstring& name Environment variable name
	 */
	EnvVar(const std::wstring& name) :
		_name(name) {
	};

	/**
	 * Check whether environment variable exists.
	 *
	 * @return bool True if exists
	 */
	bool exists() const;

	/**
	 * Get the environment variable value.
	 *
	 * @return std::wstring Value
	 */
	std::wstring get() const;

	/**
	 * Set the environment variable value.
	 *
	 * @return EnvVar& Self
	 */
	const EnvVar& set(const std::wstring& val) const;

	/**
	 * Remove the environment variable.
	 *
	 * @return EnvVar& Self
	 */
	const EnvVar& unset() const;
private:
	std::wstring _name;
};

}

#endif

#ifndef __REGISTRY__KEY_HPP__
#define __REGISTRY__KEY_HPP__

#include <windows.h>
#include <string>
#include "util/winerror.hpp"

namespace registry
{

class IKey
{
public:
	/**
	 * Constructor.
	 */
	IKey() :
		_hKey(NULL) {
	}

	/**
	 * Constructor.
	 *
	 * @param HKEY hKey Key handle
	 */
	IKey(HKEY hKey) :
		_hKey(hKey) {
	}

	/**
	 * Destructor.
	 */
	virtual ~IKey() {
	}

	/**
	 * Get key handle.
	 *
	 * @return HKEY
	 */
	virtual HKEY handle() const {
		return _hKey;
	}

	/**
	 * Cast to HKEY.
	 *
	 * @return HKEY
	 */
	operator HKEY() const {
		return _hKey;
	}

	/**
	 * Check whether key has a subkey.
	 *
	 * @param const std::wstring& subkey Key name
	 * @return bool True if subkey exists
	 */
	bool hasSubKey(const std::wstring& subkey) const;

	/**
	 * Delete registry tree of a given subkey.
	 *
	 * @param const std::wstring& subkey Key name
	 */
	void deleteSubTree(const std::wstring& subkey) const;

protected:
	HKEY _hKey;
};

class Key : public IKey
{
public:
	/**
	 * Constructor.
	 *
	 * @param HKEY hKey Key handle
	 */
	Key(HKEY hKey) :
		IKey(hKey) {
	}

	/**
	 * Constructor.
	 *
	 * Open existing registry key.
	 *
	 * @param const IKey& parent Parent key
	 * @param const std::wstring& subkey Subkey name
	 * @param REGSAM access Requested permissions
	 */
	Key(const IKey& parent, const std::wstring& subkey,
	    REGSAM access = KEY_ALL_ACCESS);

	/**
	 * Copy constructor.
	 */
	Key(const Key& rhs) : IKey() {
		if (!DuplicateHandle(
				GetCurrentProcess(), rhs._hKey,
				GetCurrentProcess(), (LPHANDLE)&_hKey,
				0, FALSE, DUPLICATE_SAME_ACCESS)) {
			THROW_LAST_ERROR("Failed to duplicate handle");
		}
	}
	Key(Key&& rhs) : IKey(rhs._hKey) {
		rhs._hKey = NULL;
	}
	~Key();

	/**
	 * Create new registry key.
	 *
	 * @param const IKey& parent Parent key
	 * @param const std::wstring& subkey Key name
	 * @param REGSAM access Requested permissions
	 * @return Key Created key
	 */
	static Key create(const IKey& parent, const std::wstring& subkey,
	                  REGSAM access = KEY_ALL_ACCESS);

	/**
	 * Check whether value exists.
	 *
	 * @param const std::wstring& name Value name
	 * @param DWORD type Value type
	 * @return bool True if exists
	 */
	bool valueExists(const std::wstring& name, DWORD type = RRF_RT_ANY) const;

	/**
	 * Set string value.
	 *
	 * @param const std::wstring& name Value name, empty string to set default
	 * @param const std::wstring& value Value
	 * @return const Key& Self
	 */
	const Key& setString(const std::wstring& name,
	                     const std::wstring& value) const;

	/**
	 * Get string value.
	 *
	 * @param const std::wstring& name Value name, empty string for default
	 * @return std::wstring Value
	 */
	std::wstring getString(const std::wstring& name) const;

	/**
	 * Set DWORD value.
	 *
	 * @param const std::wstring& name Value name, empty string to set default
	 * @param DWORD value value Value
	 * @return const Key& Self
	 */
	const Key& setDword(const std::wstring& name, DWORD value) const;
};

class PredefinedKey : public IKey
{
public:
	/**
	 * Constructor.
	 *
	 * @param HKEY hKey Key handle
	 */
	PredefinedKey(HKEY hKey) : IKey(hKey) {
	}
	~PredefinedKey() {
	}
};

}

#endif

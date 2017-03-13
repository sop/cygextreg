#ifndef __REGISTRY__KEY_HPP__
#define __REGISTRY__KEY_HPP__

#include <string>
#include <iostream>
#include <windows.h>
#include "util/winerror.hpp"

namespace registry
{

class IKey {
public:
	IKey() :
		_hKey(NULL) {
	}
	IKey(HKEY hKey) : _hKey(hKey) {
	}
	virtual ~IKey() {
	}
	virtual HKEY handle() const {
		return _hKey;
	}
	operator HKEY() const {
		return _hKey;
	}
	bool hasSubKey(const std::wstring& subkey) const;
	void deleteSubTree(const std::wstring& subkey) const;
protected:
	HKEY _hKey;
};

class Key : public IKey
{
public:
	Key(HKEY hKey) : IKey(hKey) {
	}
	Key(const IKey& parent, const std::wstring& subkey,
	    REGSAM access = KEY_ALL_ACCESS);
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
	static Key create(const IKey& parent, const std::wstring& subkey,
	                  REGSAM access = KEY_ALL_ACCESS);
	bool valueExists(const std::wstring& name, DWORD type = RRF_RT_ANY) const;
	const Key& setString(const std::wstring& name, std::wstring value) const;
	std::wstring getString(const std::wstring& name) const;
	const Key& setDword(const std::wstring& name, DWORD value) const;
};

class PredefinedKey : public IKey
{
public:
	PredefinedKey(HKEY hKey) : IKey(hKey) {
	}
	~PredefinedKey() {
	}
};

}

#endif

#include <string>
#include <iostream>
#include <windows.h>
#include "winerror.hpp"

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
protected:
	HKEY _hKey;
};

class Key : public IKey
{
public:
	Key(HKEY hKey) : IKey(hKey) {
	}
	Key(const IKey& parent, std::wstring subkey,
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
	static Key create(const IKey& parent, std::wstring subkey,
	                  REGSAM access = KEY_ALL_ACCESS);
	bool valueExists(std::wstring name, DWORD type = RRF_RT_ANY) const;
	const Key& setString(std::wstring name, std::wstring value) const;
	std::wstring getString(std::wstring name) const;
	const Key& setDword(std::wstring name, DWORD value) const;
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

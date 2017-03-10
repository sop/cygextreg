#ifndef __UTIL__ELEVATED_HPP__
#define __UTIL__ELEVATED_HPP__

#include <windows.h>

namespace cygscript {

/**
 * Admin permission checking implemented according to
 * https://support.microsoft.com/en-us/kb/118626
 */
class ElevatedProcess
{
public:
	ElevatedProcess() {
	}
	~ElevatedProcess() {
	}
	bool isAdmin() const;
	HINSTANCE startElevated(int argc, char* const argv[]);
private:
	class ImpersonationToken;
	class AdminSID;
	class ACL;
	class AdminACL;
};

class ElevatedProcess::ImpersonationToken
{
	HANDLE _hToken = 0;
public:
	ImpersonationToken();
	~ImpersonationToken();
	operator HANDLE() const;
};

class ElevatedProcess::AdminSID
{
	PSID _sid = 0;
public:
	AdminSID();
	~AdminSID();
	operator PSID() const;
};

class ElevatedProcess::ACL
{
	size_t _size = 0;
	PACL _acl = 0;
public:
	ACL(const PSID& psid);
	~ACL();
	size_t size() const;
	operator PACL() const;
};

class ElevatedProcess::AdminACL
{
	SECURITY_DESCRIPTOR _sd = {};
public:
	AdminACL(const ACL& acl, const AdminSID& sid);
	~AdminACL();
	bool hasAccess(const ImpersonationToken& token) const;
};

}

#endif

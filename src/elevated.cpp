#include "elevated.hpp"
#include <sstream>
#include <sys/cygwin.h>
#include "winerror.hpp"

namespace cygscript {

bool ElevatedProcess::isAdmin() const {
	ImpersonationToken it;
	AdminSID sid;
	ACL acl(sid);
	AdminACL adminAcl(acl, sid);
	return adminAcl.hasAccess(it);
}

HINSTANCE ElevatedProcess::startElevated(int argc, char* const argv[]) {
	char winpath[MAX_PATH + 1];
	char windir[MAX_PATH + 1];
	int i;
	std::stringstream ss;
	for (i = 1; i < argc; ++i) {
		ss << argv[i] << " ";
	}
	std::string args = ss.str();
	args.pop_back();
	if (0 != cygwin_conv_path(
			CCP_POSIX_TO_WIN_A, argv[0], winpath, sizeof(winpath))) {
		throw std::runtime_error(strerror(errno));
	}
	if (0 != cygwin_conv_path(
			CCP_POSIX_TO_WIN_A, "/bin", windir, sizeof(windir))) {
		throw std::runtime_error(strerror(errno));
	}
	HINSTANCE inst = ShellExecuteA(
		NULL, "runas", winpath, args.c_str(), windir, SW_SHOWNORMAL);
	if ((int)(intptr_t)inst <= 32) {
		THROW_ERROR_CODE("Failed to start elevated process",
		                 (int)(intptr_t)inst);
	}
	return inst;
}

ElevatedProcess::ImpersonationToken::ImpersonationToken() {
	HANDLE hToken;
	if (!OpenThreadToken(
			GetCurrentThread(), TOKEN_DUPLICATE | TOKEN_QUERY, TRUE, &hToken)) {
		if (!OpenProcessToken(
				GetCurrentProcess(), TOKEN_DUPLICATE | TOKEN_QUERY, &hToken)) {
			THROW_LAST_ERROR("Failed to open process token");
		}
	}
	BOOL ret = DuplicateToken(hToken, SecurityImpersonation, &_hToken);
	DWORD err = GetLastError();
	CloseHandle(hToken);
	if (!ret) {
		THROW_ERROR_CODE("Failed to duplicate process token", err);
	}
}

ElevatedProcess::ImpersonationToken::~ImpersonationToken() {
	if (_hToken) {
		CloseHandle(_hToken);
	}
}

ElevatedProcess::ImpersonationToken::operator HANDLE() const {
	return _hToken;
}

ElevatedProcess::AdminSID::AdminSID() {
	SID_IDENTIFIER_AUTHORITY sidAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(
			&sidAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
			DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &_sid)) {
		THROW_LAST_ERROR("Failed to initialize Admin SID");
	}
}

ElevatedProcess::AdminSID::~AdminSID() {
	if (_sid) {
		FreeSid(_sid);
	}
}

ElevatedProcess::AdminSID::operator PSID() const {
	return _sid;
}

ElevatedProcess::ACL::ACL(PSID const& psid) {
	_size = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) +
	        GetLengthSid(psid) - sizeof(DWORD);
	_acl = (PACL)LocalAlloc(LPTR, _size);
	if (!_acl) {
		THROW_LAST_ERROR("Failed to allocate ACL");
	}
}

ElevatedProcess::ACL::~ACL() {
	if (_acl) {
		LocalFree(_acl);
	}
}

size_t ElevatedProcess::ACL::size() const {
	return _size;
}

ElevatedProcess::ACL::operator PACL() const {
	return _acl;
}

ElevatedProcess::AdminACL::AdminACL(ACL const& acl, AdminSID const& sid) {
	if (!InitializeSecurityDescriptor(&_sd, SECURITY_DESCRIPTOR_REVISION)) {
		THROW_LAST_ERROR("InitializeSecurityDescriptor");
	}
	if (!InitializeAcl(acl, (DWORD)acl.size(), ACL_REVISION2)) {
		THROW_LAST_ERROR("InitializeAcl");
	}
	if (!AddAccessAllowedAce(
			acl, ACL_REVISION2, FILE_READ_ACCESS | FILE_WRITE_ACCESS, sid)) {
		THROW_LAST_ERROR("AddAccessAllowedAce");
	}
	if (!SetSecurityDescriptorDacl(&_sd, TRUE, acl, FALSE)) {
		THROW_LAST_ERROR("SetSecurityDescriptorDacl");
	}
	SetSecurityDescriptorGroup(&_sd, sid, FALSE);
	SetSecurityDescriptorOwner(&_sd, sid, FALSE);
	if (!IsValidSecurityDescriptor(&_sd)) {
		throw std::runtime_error("Invalid security descriptor");
	}
}

ElevatedProcess::AdminACL::~AdminACL() {
}

bool ElevatedProcess::AdminACL::hasAccess(
	ImpersonationToken const& token) const {
	GENERIC_MAPPING gm;
	gm.GenericRead = FILE_READ_ACCESS;
	gm.GenericWrite = FILE_WRITE_ACCESS;
	gm.GenericExecute = 0;
	gm.GenericAll = FILE_READ_ACCESS | FILE_WRITE_ACCESS;
	PRIVILEGE_SET ps;
	DWORD size = sizeof(ps);
	DWORD access;
	BOOL status;
	if (!AccessCheck(
			(const PSECURITY_DESCRIPTOR)&_sd,
			token, FILE_READ_ACCESS,
			&gm, &ps, &size, &access, &status)) {
		THROW_LAST_ERROR("Failed to check access rights");
	}
	return status == TRUE ? true : false;
}

}

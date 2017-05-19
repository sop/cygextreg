#ifndef __UTIL__SHUTIL_H__
#define __UTIL__SHUTIL_H__

#include <string>

namespace cygextreg
{

/**
 * Escape command line argument with Windows semantics.
 *
 * @param std::wstring arg Argument
 * @return std::wstring Escaped argument
 */
std::wstring escapeWinArg(const std::wstring& arg);

/**
 * Escape command line argument with Posix semantics.
 *
 * @param std::wstring arg Argument
 * @return std::wstring Escaped argument
 */
std::wstring escapePosixArg(const std::wstring& arg);

}

#endif

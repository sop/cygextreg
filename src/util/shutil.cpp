#include "shutil.hpp"

namespace cygextreg
{

/**
 * Replace all occurences of a string in place.
 *
 * @param std::wstring& str String to modify
 * @param const std::wstring& from Substring to search
 * @param const std::wstring& to Replacement string
 */
void replaceAll(std::wstring& str, const std::wstring& from,
                const std::wstring& to) {
	size_t start = 0;
	while((start = str.find(from, start)) != std::wstring::npos) {
		str.replace(start, from.length(), to);
		start += to.length();
	}
}

std::wstring escapeWinArg(const std::wstring& arg) {
	std::wstring str;
	str.push_back(L'"');
	for (size_t i = 0; i < arg.length(); ++i) {
		// backslash
		if (L'\\' == arg[i]) {
			// if last character
			if (arg.length() - 1 == i) {
				str.push_back(L'\\');
			}
			// if followed by a doublequote
			else if(L'"' == arg[i+1]) {
				str.push_back(L'\\');
			}
		}
		// doublequote
		else if (L'"' == arg[i]) {
			str.push_back(L'\\');
		}
		str.push_back(arg[i]);
	}
	str.push_back(L'"');
	return str;
}

std::wstring escapePosixArg(const std::wstring& arg) {
	std::wstring str = arg;
	replaceAll(str, L"'", L"'\\\''");
	return str.insert(0, L"'").append(L"'");
}

}

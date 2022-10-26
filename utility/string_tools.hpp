#ifndef __STRING_TOOLS_H__
#define __STRING_TOOLS_H__

#include <string.h>
#include <stdlib.h>

inline bool has_suffix(const char* src, const char* suffix)
{
	int suffixlen = strlen(suffix);
	int srclen = strlen(src);
	return ((srclen - suffixlen) >= 0 && (strcmp(&src[srclen - suffixlen],suffix) == 0));
}

inline bool has_suffix(const wchar_t* src, const wchar_t* suffix)
{
	int suffixlen = lstrlen(suffix);
	int srclen = lstrlen(src);
	return ((srclen - suffixlen) >= 0 && (lstrcmp(&src[srclen - suffixlen],suffix) == 0));
}

inline std::string bin2hex(const char* buffer, unsigned int size)
{
	static const char kNUM_TABLE[] = "0123456789ABCDEF";
	std::string temp(size*2,'0');
	for(unsigned int i = 0; i < size; ++i)
	{
		unsigned char c = static_cast<unsigned char>(buffer[i]);
		temp[(i << 1)] = kNUM_TABLE[(c >> 4) & 0xf];
		temp[(i << 1) + 1] = kNUM_TABLE[(c & 0xf)];
	}
	return temp;
}

#endif//__STRING_TOOLS_H__
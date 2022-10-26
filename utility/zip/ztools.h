#ifndef __MACHO_ZTOOLS_H__
#define __MACHO_ZTOOLS_H__

#include <vector>

std::vector<char> compressz(const char* indata, unsigned int size);
std::vector<char> compressz(const std::vector<char>& indata);
std::vector<char> uncompressz(const std::vector<char>& indata);

#endif //__MACHO_ZTOOLS_H__
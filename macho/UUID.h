#ifndef __MACHO_UUID_H__
#define __MACHO_UUID_H__

#include <types.h>
#include <string>

namespace macho
{
	enum { kUUID_BYTE_SIZE = 16 };
	enum { kUUID_CHARACTER_COUNT = kUUID_BYTE_SIZE * 2 };

	std::string UUIDMem2Str(char* data);
	int UUIDStr2Mem(const std::string& str, char outres[]);
}

#endif //__MACHO_UUID_H__
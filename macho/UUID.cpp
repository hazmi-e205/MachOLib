#include "UUID.h"
#include <assert.h>
namespace macho
{
	static const char kNUM_TABLE[] = "0123456789ABCDEF";

	static unsigned int char2num(int character)
	{
		unsigned ret = 0;
		if(character >= '0' && character <= '9')
		{
			ret = character - '0';
		}
		else if(character >= 'a' && character <= 'f')
		{
			ret = (character - 'a') + 0xA;
		}
		else if(character >= 'A' && character <= 'F')
		{
			ret = (character - 'A') + 0xA;
		}
		return (ret&0xf);
	}

	std::string UUIDMem2Str(char* data)
	{
		std::string temp(32,'0');
		for(t_uint32 i = 0; i < kUUID_BYTE_SIZE; ++i)
		{
			unsigned char c = static_cast<unsigned char>(data[i]);
			temp[(i << 1)] = kNUM_TABLE[(c >> 4) & 0xf];
			temp[(i << 1) + 1] = kNUM_TABLE[(c & 0xf)];
		}
		return temp;
	}
	int UUIDStr2Mem(const std::string& str, char outres[])
	{
		assert(str.size() >= kUUID_CHARACTER_COUNT);
		memset(outres, 0, kUUID_BYTE_SIZE);
		for(t_uint32 i = 0; i < kUUID_BYTE_SIZE; ++i)
		{
			char h = str[(i<<1)], l = str[(i<<1) + 1];
			unsigned int hnum = char2num(h), lnum = char2num(l);
			outres[i] = (hnum<<4 | lnum);
		}
		return kUUID_CHARACTER_COUNT;
	}
}
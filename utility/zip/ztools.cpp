#include "ztools.h"
#include <zlib.h>

static unsigned char kMAGIC = 0x54;

std::vector<char> compressz(const std::vector<char>& indata)
{
	return compressz(&indata[0], indata.size());
}
std::vector<char> compressz(const char* indata, unsigned int size)
{
	enum { kOFFSET = 5 };
	std::vector<char> ores;
	uLongf dstsize = compressBound((uLong)size), dst2 = dstsize;
	ores.resize(dstsize + kOFFSET);
	if (compress2((Bytef*)&ores[kOFFSET], &dstsize, (const Bytef*)indata, (uLong)size, Z_BEST_COMPRESSION) == Z_OK)
	{
		//save size of uncompressed data
		ores[0] = kMAGIC;
		ores[1] = (size & 0xff);
		ores[2] = ((size >> 8) & 0xff);
		ores[3] = ((size >> 16) & 0xff);
		ores[4] = ((size >> 24) & 0xff);
		ores.resize(dstsize + kOFFSET);
	}
	else
	{
		ores.clear();
	}
	return ores;
}
std::vector<char> uncompressz(const std::vector<char>& indata)
{
	enum { kOFFSET = 5 };
	std::vector<char> ores;
	if (indata.size() >= kOFFSET)
	{
		if (((unsigned char)indata[0]) == kMAGIC)
		{
			uLong dstSize = ((unsigned char)indata[1]) | (((unsigned char)indata[2]) << 8) | (((unsigned char)indata[3]) << 16) | (((unsigned char)indata[4]) << 24);
			ores.resize(dstSize);
			if (uncompress((Bytef*)&ores[0], &dstSize, (const Bytef*)&indata[kOFFSET], (uLong)(indata.size() - kOFFSET)) == Z_OK)
			{
				ores.resize(dstSize);
			}
			else
			{
				ores.clear();
			}
		}
	}
	return ores;
}

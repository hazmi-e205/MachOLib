#ifndef __MACHO_ZIPED_PACKAGE_H__
#define __MACHO_ZIPED_PACKAGE_H__

#include <vector>
#include <shared_ptr_alias.hpp>
extern "C"
{
	#include <zlib/zlib.h>
	#include <unzip.h>
}

class ZMachOPack
{
public:
	typedef tools::shared_ptr<ZMachOPack> Ptr;
public:
	ZMachOPack(const char* path);
	~ZMachOPack();
	bool is_open() const;
	bool exist(const char* fileName);
	std::vector<char> extract(const char* fileName);
	std::vector<std::wstring> files();
private:
	ZMachOPack(const ZMachOPack&);
	ZMachOPack& operator =(const ZMachOPack&);
private:
	unzFile m_zip;
};

#endif //__MACHO_ZIPED_PACKAGE_H__
#ifndef __MACHO_ZIPED_PACKAGE_H__
#define __MACHO_ZIPED_PACKAGE_H__

#include <vector>
#include <memory>
extern "C"
{
    #include <zlib.h>
	#include <unzip.h>
}

class ZMachOPack
{
public:
	typedef std::shared_ptr<ZMachOPack> Ptr;
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

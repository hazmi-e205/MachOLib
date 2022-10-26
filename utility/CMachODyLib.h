#ifndef __CMACH_OBJECT_DYLIB_H__
#define __CMACH_OBJECT_DYLIB_H__
#include <macho/CFAT.h>
#include <macho/UUID.h>
#include <utility/zip/ZMachOPack.h>

class CMachODyLib
{
private:
	std::wstring				m_path;
	std::wstring				m_name;
	std::vector<std::wstring>	m_uuids;
	macho::CFAT::ptr			m_fat;
	ZMachOPack::Ptr				m_zmacho;
public:
	CMachODyLib();
	CMachODyLib(const std::wstring& path, const std::wstring& name);
	CMachODyLib(const std::wstring& name, ZMachOPack::Ptr pack);

	std::wstring& setPath(const std::wstring& path);
	const std::wstring& path() const;
	std::wstring& setName(const std::wstring& name);
	const std::wstring& name() const;
	std::vector<std::wstring>& uuids();
	const std::vector<std::wstring>& uuids() const;
	macho::CFAT::ptr fat();
private:
	void _loadFat();
};

#endif //__CMACH_OBJECT_DYLIB_H__
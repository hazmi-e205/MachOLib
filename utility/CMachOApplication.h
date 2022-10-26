#ifndef __CMACH_OBJECT_APPLICATON_H__
#define __CMACH_OBJECT_APPLICATON_H__

#include <macho/CFAT.h>

class CMachOApplication
{
private:
	std::wstring				m_path;
	std::wstring				m_name;
	std::vector<std::wstring>	m_uuids;
	macho::CFAT::ptr			m_fat;
public:
	CMachOApplication();
	CMachOApplication(const std::wstring& path, const std::wstring& name);

	std::wstring& setPath(const std::wstring& path);
	const std::wstring& path() const;
	std::wstring& setName(const std::wstring& name);
	const std::wstring& name() const;
	std::vector<std::wstring>& uuids();
	const std::vector<std::wstring>& uuids() const;
	macho::CFAT::ptr fat() const;
private:
	void _loadFat();
};

#endif //__CMACH_OBJECT_APPLICATON_H__
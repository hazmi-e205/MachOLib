#include "CMachODSym.h"
#include <algorithm>


const wchar_t CMachODSym::kRELATIVE_PATH_OF_MACHO_DSYM[] = L"\\Contents\\Resources\\DWARF\\";

CMachODSym::CMachODSym()
	:m_path(),
	m_name(),
	m_uuids(),
	m_fat()
{
	return;
}

CMachODSym::CMachODSym(const std::wstring& path, const std::wstring& name)
	:m_path(path),
	m_name(name),
	m_uuids()
{
	if(!m_path.empty() && !m_name.empty()) {
		_loadFat();
	}
	return;
}

bool CMachODSym::isExistUUID(const std::wstring& uuid) {
	return (std::find(m_uuids.begin(),m_uuids.end(),uuid) != m_uuids.end());
}
std::wstring& CMachODSym::setPath(const std::wstring& path)
{
	m_path = path;
	if(!m_path.empty() && !m_name.empty()) {
		_loadFat();
	}
	return m_path;
}
const std::wstring& CMachODSym::path() const {
	return m_path;
}
std::wstring& CMachODSym::setName(const std::wstring& name) {
	m_name = name;
	if(!m_path.empty() && !m_name.empty()) {
		_loadFat();
	}
	return m_name;
}
const std::wstring& CMachODSym::name() const {
	return m_name;
}
std::vector<std::wstring>& CMachODSym::uuids() {
	return m_uuids;
}
const std::vector<std::wstring>& CMachODSym::uuids() const {
	return m_uuids;
}
macho::CFAT::ptr CMachODSym::fat() const {
	return m_fat;
}
void CMachODSym::_loadFat()
{
	std::wstring macho_path = m_path + kRELATIVE_PATH_OF_MACHO_DSYM + m_name;
	io::source_of_stream source(macho_path.c_str(), io::EENCODING_UNKNOWN);
	m_fat = macho::CFAT::ptr(new macho::CFAT(source));
	if(m_fat->validate() == base::ERESULT_CODE_SUCCESS) {
		m_fat->getAllUUIDs(m_uuids);
	}
	return;
}

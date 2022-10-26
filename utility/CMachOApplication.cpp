#include "CMachOApplication.h"

CMachOApplication::CMachOApplication()
:m_path(),
m_name(),
m_uuids()
{
	return;
}
CMachOApplication::CMachOApplication(const std::wstring& path, const std::wstring& name)
:m_path(path),
m_name(name),
m_uuids()
{
	if(!m_path.empty() && !m_name.empty()) {
		_loadFat();
	}
	return;
}

std::wstring& CMachOApplication::setPath(const std::wstring& path) {
	m_path = path;
	if(!m_path.empty() && !m_name.empty()) {
		_loadFat();
	}
	return m_path;
}
const std::wstring& CMachOApplication::path() const {
	return m_path;
}
std::wstring& CMachOApplication::setName(const std::wstring& name) {
	m_name = name;
	if(!m_path.empty() && !m_name.empty()) {
		_loadFat();
	}
	return m_name;
}
const std::wstring& CMachOApplication::name() const {
	return m_name;
}
std::vector<std::wstring>& CMachOApplication::uuids() {
	return m_uuids;
}
const std::vector<std::wstring>& CMachOApplication::uuids() const {
	return m_uuids;
}
macho::CFAT::ptr CMachOApplication::fat() const {
	return m_fat;
}
void CMachOApplication::_loadFat()
{
	std::wstring macho_path = m_path + L'\\' + m_name;
	io::source_of_stream source(macho_path.c_str(), io::EENCODING_UNKNOWN);
	m_fat = macho::CFAT::ptr(new macho::CFAT(source));
	if(m_fat->validate() == base::ERESULT_CODE_SUCCESS) {
		m_fat->getAllUUIDs(m_uuids);
	}
	return;
}

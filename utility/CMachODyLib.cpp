#include "CMachODyLib.h"

CMachODyLib::CMachODyLib()
:m_path(),
m_name(),
m_uuids(),
m_fat(),
m_zmacho()
{
	return;
}
CMachODyLib::CMachODyLib(const std::wstring& path, const std::wstring& name)
:m_path(path),
m_name(name),
m_uuids(),
m_fat(),
m_zmacho()
{
	if(!m_path.empty()) {
		_loadFat();
	}
	return;
}
CMachODyLib::CMachODyLib(const std::wstring& name, ZMachOPack::Ptr pack)
:m_path(name),
m_name(name),
m_uuids(),
m_fat(),
m_zmacho(pack)
{
	if(name.size() >= macho::kUUID_CHARACTER_COUNT)
	{
		m_uuids.push_back(std::wstring(&name[0], &name[macho::kUUID_CHARACTER_COUNT]));
	}
	return;
}

std::wstring& CMachODyLib::setPath(const std::wstring& path) {
	m_path = path;
	if(!m_path.empty()) {
		_loadFat();
	}
	return m_path;
}
const std::wstring& CMachODyLib::path() const {
	return m_path;
}
std::wstring& CMachODyLib::setName(const std::wstring& name) {
	m_name = name;
	if(!m_path.empty()) {
		_loadFat();
	}
	return m_name;
}
const std::wstring& CMachODyLib::name() const {
	return m_name;
}
std::vector<std::wstring>& CMachODyLib::uuids() {
	return m_uuids;
}
const std::vector<std::wstring>& CMachODyLib::uuids() const {
	return m_uuids;
}
macho::CFAT::ptr CMachODyLib::fat() {
	if(m_fat.get() == NULL) {
		_loadFat();
	}
	return m_fat;
}
void CMachODyLib::_loadFat()
{
	io::source_of_stream source;
	if(m_zmacho.get() != NULL)
	{
		std::vector<char> bf = m_zmacho->extract(std::string(m_name.begin(), m_name.end()).c_str());
		io::stream_memory::ptr memory(new io::stream_memory(&bf[0], bf.size()));
		source = io::source_of_stream( memory, io::EENCODING_UNKNOWN);
	}
	else
	{
		std::wstring macho_path = m_path;
		source = io::source_of_stream(macho_path.c_str(), io::EENCODING_UNKNOWN);
	}
	m_fat = macho::CFAT::ptr(new macho::CFAT(source));
	if(m_fat->validate() == base::ERESULT_CODE_SUCCESS) {
		m_fat->getAllUUIDs(m_uuids);
	}
	return;
}
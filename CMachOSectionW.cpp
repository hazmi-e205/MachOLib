#include "CMachOSectionW.h"

namespace MachO
{
	CMachOSectionW::CMachOSectionW(const macho::CSection::Ptr& other)
		:m_section(other)
	{
		return;
	}
    std::wstring CMachOSectionW::GetName()
	{
        return std::wstring(m_section->getName().begin(), m_section->getName().end());
	}
    uint64_t	CMachOSectionW::GetAddress()
	{
		return m_section->getAddress();
	}
    uint64_t	CMachOSectionW::GetSize()
	{
		return m_section->getSize();
	}
}

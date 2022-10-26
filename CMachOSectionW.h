#pragma once
#include <macho/CSection.h>

namespace MachO
{
    class CMachOSectionW
	{
	public:
		CMachOSectionW(const macho::CSection::Ptr& other);
        std::wstring GetName();
        uint64_t	GetAddress();
        uint64_t	GetSize();
	private:
        std::shared_ptr<macho::CSection> m_section;
	};
}

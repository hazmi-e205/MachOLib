#pragma once
#include <macho/CSegment.h>
#include "CMachOSectionW.h"

namespace MachO
{
    class CMachOSegmentW
	{
	public:
		CMachOSegmentW(const macho::CSegment::Ptr& other);
        std::wstring GetName();
        uint64_t	GetVMAddress();
        uint64_t	GetVMSize();
        uint32_t	GetSectionCount();
        CMachOSectionW*	GetSectionAt(int idx);
	private:
        std::shared_ptr<macho::CSegment> m_segment;
	};
}

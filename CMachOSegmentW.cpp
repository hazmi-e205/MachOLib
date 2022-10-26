#include "CMachOSegmentW.h"

namespace MachO 
{
	CMachOSegmentW::CMachOSegmentW(const macho::CSegment::Ptr& other)
		:m_segment(other)
	{
		return;
	}
    std::wstring CMachOSegmentW::GetName()
	{
        return std::wstring(m_segment->getName().begin(), m_segment->getName().end());
	}
    uint64_t	CMachOSegmentW::GetVMAddress()
	{
		return m_segment->getVmAddress();
	}
    uint64_t	CMachOSegmentW::GetVMSize()
	{
		return m_segment->getVmSize();
	}
    uint32_t	CMachOSegmentW::GetSectionCount()
	{
		return m_segment->getSectionCount();
	}
    CMachOSectionW*	CMachOSegmentW::GetSectionAt(int idx)
	{
        return new CMachOSectionW(m_segment->getSectionAt(idx));
	}
}

#ifndef __CDWARF_RANGE_RESOLVER_H__
#define __CDWARF_RANGE_RESOLVER_H__

#include <dwarf/CDWARFRangeTable.h>

namespace dwarf
{
	namespace utility
	{
		class CDWARFRangeResolver
		{
		public:
			CDWARFRangeResolver()
				:m_pranges(NULL),
				m_address_byte_size(0),
				m_stream()
			{
				return;
			}
			CDWARFRangeResolver(CDWARFRangeTable& ranges, t_uint8 addressByteSize, io::stream_base::ptr stream)
				:m_pranges(&ranges),
				m_address_byte_size(addressByteSize),
				m_stream(stream)
			{
				return;
			}
			const CDWARFRangeTable::RangeSet* operator() (t_uint64 offset) const
			{
				const CDWARFRangeTable::RangeSet* ret = NULL;
				if(m_pranges) {
					ret = m_pranges->extractRangeSetBy(m_address_byte_size, m_stream, offset);
				}
				return ret;
			}
		private:
			CDWARFRangeTable*		m_pranges;
			t_uint8					m_address_byte_size;
			io::stream_base::ptr	m_stream;
		};
	}
}
#endif //__CDWARF_RANGE_RESOLVER_H__
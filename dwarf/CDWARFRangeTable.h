#ifndef __CDWARF_RANGE_TABLE_H__
#define __CDWARF_RANGE_TABLE_H__

#include <set>
#include <map>
#include <vector>
#include <string>
#include <limits>
#include <io/stream_base.h>
#include <io/source_of_stream.h>
#include <memory>
#include <io/stream_memory.h>
#include "EDWARF.h"
#include "DWARFTypes.h"
#include "CDWARFFormValue.h"

namespace dwarf
{
	class CDWARFRangeTable
	{
	public:
		struct CDWARFRangeListEntry 
		{
			CDWARFRangeListEntry(t_uint64 begin, t_uint64 end)
				:StartAddress(begin),
				EndAddress(end)
			{
				return;
			}
			bool isEndOfListEntry() const 
			{
				return (StartAddress == 0) && (EndAddress == 0);
			}
			bool isBaseAddressSelectionEntry(t_uint8 addressSize) const 
			{
				bool ret = false;
				if (addressSize == 4) {
					ret = (StartAddress == static_cast<t_uint32>(-1));
				}
				else {
					 ret = (StartAddress == static_cast<t_uint64>(-1));
				}
				return ret;
			}
			bool containsAddress(t_uint64 baseAddress, t_uint64 address) const 
			{
				return (baseAddress + StartAddress <= address) && (address < baseAddress + EndAddress);
			}
			
			t_uint64 StartAddress;
			t_uint64 EndAddress;
		};
		
		typedef std::vector<CDWARFRangeListEntry> RangeCollention;

		struct RangeSet
		{
			explicit RangeSet(t_uint64 offset)
				:Offset(offset),
				Collection()
			{
				return;
			}
			bool containsAddress(t_uint64 baseAddress, t_uint64 address) const 
			{
				bool ret = false;
				for(RangeCollention::const_iterator i = Collection.begin(), e = Collection.end();
					i != e && !ret;
					++i)
				{
					ret = (*i).containsAddress(baseAddress, address);
				}
				return ret;
			}
			
			bool operator < (const RangeSet& other) const
			{
				return (Offset < other.Offset);
			}

			t_uint64		Offset;
			RangeCollention Collection;
		};

		struct RangeSetComparator
		{
			bool operator ()(const t_uint64& left, const RangeSet& right) const
			{
				return (left < right.Offset);
			}
			bool operator ()(const RangeSet& left, const t_uint64& right) const
			{
				return (left.Offset < right);
			}
			bool operator ()(const RangeSet& left, const RangeSet& right) const
			{
				return (left.Offset < right.Offset);
			}
		};
		
		typedef std::vector<RangeSet> RangeTable;
	public:
		bool parse(t_uint8 addressSize, io::stream_base::ptr stream);
		const RangeSet* getRangeBy(t_uint64 offset) const;
		const RangeSet* extractRangeSetBy(t_uint8 addressSize, io::stream_base::ptr stream, t_uint64 offset);
	private:
		const RangeSet* extract(t_uint8 addressSize, io::stream_base::ptr stream);
	private:
		RangeTable m_table;
	};
}

#endif //__CDWARF_RANGE_TABLE_H__
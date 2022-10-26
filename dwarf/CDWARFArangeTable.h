#ifndef __CDWARF_ARANGE_TABEL_H__
#define __CDWARF_ARANGE_TABEL_H__

#include <map>
#include <vector>
#include <string>
#include <io/stream_base.h>
#include <io/source_of_stream.h>
#include "DWARFTypes.h"
#include "EDWARF.h"

namespace dwarf
{
	class CDWARFArangeTable
	{
	public:
		struct CDWARFRange
		{
		public:
			CDWARFRange()
				:begin(0),
				end(0),
				CUOffset(0)
			{
			}
			CDWARFRange(t_uint64 b, t_uint64 e, t_uint64 off)
				:begin(b),
				end(e),
				CUOffset(off)
			{
			}

			bool operator < (const CDWARFRange& other)
			{
				return (end < other.end);
			}

			t_uint64	begin;
			t_uint64	end;
			t_uint64	CUOffset;
		};
		struct CDWARFArangesHeader
		{
			CDWARFArangesHeader()
				:length(0),
				version(0),
				cu_offset(0),
				addr_size(0),
				seg_size(0)
			{
				return;
			}
			t_uint32    length;
			t_uint16    version;
			t_uint32    cu_offset;
			t_uint8     addr_size;
			t_uint8     seg_size;
		};
		struct RangeComparator
		{
			bool operator ()(t_uint64 left, const CDWARFRange& right) const
			{
				return (left < right.end);
			}
			bool operator ()(const CDWARFRange& left, t_uint64 right) const
			{
				return (left.end < right);
			}
			bool operator ()(const CDWARFRange& left, const CDWARFRange& right) const
			{
				return (left.end < right.end);
			}
		};
		typedef std::vector<CDWARFRange> RangeTable;
	public:
		CDWARFArangeTable();
		bool parse(io::stream_base::ptr stream);
		const CDWARFRange* getRangesBy(t_uint64 address) const;
	private:
		bool parse_set(io::stream_base::ptr stream);
	private:
		RangeTable m_ranges;
	};
}

#endif //__CDWARF_ARANGE_TABEL_H__
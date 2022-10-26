#ifndef __CDWARF_COMPILE_UNIT_H__
#define __CDWARF_COMPILE_UNIT_H__

#include <map>
#include <vector>
#include <string>
#include <limits>
#include <io/stream_base.h>
#include <io/source_of_stream.h>
#include <memory>
#include <io/stream_memory.h>
#include "utility/CDWARFRangeResolver.hpp"
#include "CDWARFDebugInfoEntry.h"
#include "DWARFTypes.h"
#include "EDWARF.h"

namespace dwarf
{
	namespace utility
	{
		class CDWARFRangeResolver;
	}
	class CDWARFRangeTable;
	class CDWARFCompileUnit
	{
	public:
		struct DebugInfoEntryNode
		{
			DebugInfoEntryNode(t_uint64 offset)
				:Offset(offset),
				DIE(),
				Depth(0)
			{
				return;
			}
			t_uint64			 Offset;
			CDWARFDebugInfoEntry DIE;
			t_uint32			 Depth;
		};
		typedef std::vector<DebugInfoEntryNode>				DIETable;
		typedef std::vector<const CDWARFDebugInfoEntry*>	PDIETable;
		typedef std::vector<unsigned int>					IDX_DIETable;
		typedef t_uint32									DIEIdx;
		typedef std::vector<DIEIdx>							DIEIdxTable;
	public:
		static const DIEIdx									kINVALID_IDX = 0xffffffff;
	public:
		CDWARFCompileUnit();
		bool							parse(const CDWARFAbbreviationTable& abbr_table,
												io::stream_base::ptr debug_str,
												io::stream_base::ptr stream);

		/*deprecated*/
		const CDWARFDebugInfoEntry*		extractDIEByOffset(const CDWARFAbbreviationTable& abbr_table,
														io::stream_base::ptr debug_str,
														io::stream_base::ptr stream, 
														t_uint64 absolute_die_offset,
														bool full_extract = false);
		const CDWARFDebugInfoEntry*		getDIEByOffset(t_uint64 absolute_die_offset) const;
		DIEIdx							getIdxByOffset(t_uint64 absolute_die_offset) const;
		DIEIdx							getIdxByDIE(const CDWARFDebugInfoEntry* die) const;
		const CDWARFDebugInfoEntry*		getDIEByIdx(DIEIdx idx) const;
		bool							fullExtract(DIEIdx idx,
													const CDWARFAbbreviationTable& abbr_table,
													io::stream_base::ptr debug_str,
													io::stream_base::ptr stream);

		PDIETable						getDIEBy(t_uint64 address, const utility::CDWARFRangeResolver& range_resolver) const;
		DIEIdxTable						getDIEIdxsBy(t_uint64 address, const utility::CDWARFRangeResolver& range_resolver, const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str, io::stream_base::ptr stream);
		PDIETable						getParents(const CDWARFDebugInfoEntry* die);
		PDIETable						getChildrens(const CDWARFDebugInfoEntry* die);
		DIEIdxTable						getParentsIdxs(DIEIdx idx) const;
		DIEIdxTable						getChildrensIdxs(DIEIdx idx) const;
		DIEIdxTable						getAllChildrensIdxs(DIEIdx idx) const;
		bool							fullExtract(const CDWARFDebugInfoEntry* die,
													const CDWARFAbbreviationTable& abbr_table,
													io::stream_base::ptr debug_str,
													io::stream_base::ptr stream);
		
		t_uint64						getLength() const;
		t_uint16						getVersion() const;
		t_uint8							getAddrSize() const;
		t_uint8							getDwarfFromat() const;
		t_uint32						getHeaderSize() const;
		t_uint64						getBaseAddress() const;
		t_uint64						getStmtList() const;
		std::string						getName() const;
		bool operator < (const CDWARFCompileUnit& other) const;
	private:
		DebugInfoEntryNode*				extract(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str,io::stream_base::ptr stream, bool full_extract = false);
		DIETable::iterator				getNodeByDIE(const CDWARFDebugInfoEntry* die);
		DIETable::const_iterator		getNodeByDIE(const CDWARFDebugInfoEntry* die) const;
		DIETable::iterator				getNodeByIdx(DIEIdx idx);
		DIETable::const_iterator		getNodeByIdx(DIEIdx idx) const;
		DIETable::iterator				getNodeByOffset(t_uint64 absolute_die_offset);
		DIETable::const_iterator		getNodeByOffset(t_uint64 absolute_die_offset) const;
		DIETable::iterator				begin();
		DIETable::const_iterator		begin() const;
		DIETable::iterator				end();
		DIETable::const_iterator		end() const;
	private:
		t_uint64							m_base_address;
		t_uint64							m_stmt_list;
		t_uint64							m_length;
		t_uint8								m_version:4,m_addrSize:4;
		t_uint8								m_dwarf_format; //spec section 7.4 32-Bit and 64-Bit DWARF Formats
		std::string							m_name;
		DIETable							m_debugInfoEntry;
	};
}

#endif //__CDWARF_COMPILE_UNIT_H__
#include "CDWARFCompileUnit.h"
#include "CDWARFRangeTable.h"
#include "CDWARFAbbreviationTable.h"
#include "utility/CDWARFRangeResolver.hpp"
#include <utility/CConsole.h>

namespace dwarf
{
	static bool is_complie_unit(DW_TAG tag)
	{
		return (tag == DW_TAG_compile_unit);
	}
	/*
	* CDWARFCompileUnitTable::CDWARFCompileUnit
	*/
	CDWARFCompileUnit::CDWARFCompileUnit()
		:m_base_address(CDWARFDebugInfoEntry::kINVALID_ADDRESS),
		m_stmt_list(CDWARFDebugInfoEntry::kINVALID_ADDRESS),
		m_length(0),
		m_version(0),
		m_addrSize(0),
		m_name(),
		m_debugInfoEntry()
	{
		return;
	}
	bool CDWARFCompileUnit::parse(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str,io::stream_base::ptr stream)
	{
		bool ret = false;
		t_uint16 version;
		t_uint8 addrSize;
		t_uint32 abbrOffset;
		io::bstreamsize end = 0;
		stream->t_read(m_length);
		
		end = stream->position() + m_length;
		stream->t_read(version);
		stream->t_read(abbrOffset);
		stream->t_read(addrSize);
		m_version = static_cast<t_uint8>(version&0xf);
		m_addrSize = static_cast<t_uint8>(addrSize&0xf);
		if(m_version == 2 && (m_addrSize == 4 || m_addrSize == 8))
		{
			t_uint32 depth = 0;
			m_debugInfoEntry.clear();
			while(stream->position() < end)
			{
				CDWARFCompileUnit::DebugInfoEntryNode* die = extract(abbr_table, debug_str, stream);
				if(die)
				{
					die->Depth = depth;
					if(die->DIE.hasChild())
					{
						++depth;
					}
				}
				else
				{
					--depth;
				}
			}
			ret = !m_debugInfoEntry.empty();
		}
		return ret;
	}
	CDWARFCompileUnit::DebugInfoEntryNode* CDWARFCompileUnit::extract(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str,io::stream_base::ptr stream, bool full_extract)
	{
		DebugInfoEntryNode* ret = NULL;
		m_debugInfoEntry.push_back(DebugInfoEntryNode(stream->position()));
		if(!m_debugInfoEntry.back().DIE.parse_if(abbr_table, *this, debug_str, stream, is_complie_unit))
		{
			m_debugInfoEntry.pop_back();
			ret = NULL;
		}
		else
		{
			ret = &m_debugInfoEntry.back();
			if(m_debugInfoEntry.back().DIE.isCompileUnit())
			{
				m_stmt_list = m_debugInfoEntry.back().DIE.getStmtList();
				m_base_address = m_debugInfoEntry.back().DIE.getLowPC();
				m_name = m_debugInfoEntry.back().DIE.getName(debug_str);
				if(m_base_address == CDWARFDebugInfoEntry::kINVALID_ADDRESS)
				{
					m_base_address = m_debugInfoEntry.back().DIE.getEnterPC();
				}
			}
		}
		return ret;
	}
	const CDWARFDebugInfoEntry* CDWARFCompileUnit::getDIEByOffset(t_uint64 absolute_die_offset) const
	{
		const CDWARFDebugInfoEntry* ret = NULL;
		DIETable::const_iterator i = getNodeByOffset(absolute_die_offset);
		if(i != end())
		{
			ret = &(*i).DIE;
		}
		return ret;
	}
	CDWARFCompileUnit::DIEIdx CDWARFCompileUnit::getIdxByOffset(t_uint64 absolute_die_offset) const
	{
		DIEIdx idx = kINVALID_IDX;
		DIETable::const_iterator i = getNodeByOffset(absolute_die_offset);
		if(i != end())
		{
			idx = std::distance(begin(), i);
		}
		return idx;
	}
	CDWARFCompileUnit::DIEIdx CDWARFCompileUnit::getIdxByDIE(const CDWARFDebugInfoEntry* die) const
	{
		DIEIdx idx = kINVALID_IDX;
		DIETable::const_iterator i = getNodeByDIE(die);
		if(i != end())
		{
			idx = std::distance(begin(), i);
		}
		return idx;
	}
	const CDWARFDebugInfoEntry* CDWARFCompileUnit::getDIEByIdx(DIEIdx idx) const
	{
		const CDWARFDebugInfoEntry* ret = NULL;
		if(idx != kINVALID_IDX)
		{
			ret = &(*(begin() + idx)).DIE;
		}
		return ret;
	}

	/*deprecated*/
	const CDWARFDebugInfoEntry* CDWARFCompileUnit::extractDIEByOffset(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str, io::stream_base::ptr stream, t_uint64 absolute_die_offset, bool full_extract)
	{
		return getDIEByOffset(absolute_die_offset);
	}
	t_uint32 CDWARFCompileUnit::getLength() const
	{
		return m_length;
	}
	t_uint16 CDWARFCompileUnit::getVersion() const
	{
		return m_version;
	}
	t_uint8 CDWARFCompileUnit::getAddrSize() const
	{
		return m_addrSize;
	}
	t_uint32 CDWARFCompileUnit::getHeaderSize() const
	{
		return sizeof(t_uint32)
				+ sizeof(t_uint16)
				+ sizeof(t_uint32)
				+ sizeof(t_uint8);
	}
	t_uint64 CDWARFCompileUnit::getBaseAddress() const
	{
		return m_base_address;
	}
	t_uint64 CDWARFCompileUnit::getStmtList() const
	{
		return m_stmt_list;
	}
	std::string CDWARFCompileUnit::getName() const
	{
		return m_name;
	}
	CDWARFCompileUnit::PDIETable CDWARFCompileUnit::getParents(const CDWARFDebugInfoEntry* die)
	{
		PDIETable parents;
		CDWARFCompileUnit::DIETable::iterator it = getNodeByDIE(die);
		if(it != end() && ((*it).Depth > 0))
		{
			t_uint32 depth = ((*it).Depth - 1);
			for(DIETable::reverse_iterator i(it), re = m_debugInfoEntry.rend();
				i != re;
				++i)
			{
				if((*i).Depth <= depth)
				{
					parents.push_back(&((*i).DIE));
					--depth;
				}
			}
		}
		return parents;
	}
	CDWARFCompileUnit::PDIETable CDWARFCompileUnit::getChildrens(const CDWARFDebugInfoEntry* die)
	{
		CDWARFCompileUnit::PDIETable ret;
		CDWARFCompileUnit::DIETable::iterator it = getNodeByDIE(die);
		if(it != end() && ((*it).Depth > 0))
		{
			t_uint32 depth = ((*it).Depth + 1);
			for(DIETable::iterator i = (it + 1), e = end();
				i != e;
				++i)
			{
				if((*i).Depth == depth)
				{
					ret.push_back(&((*i).DIE));
				}
				else
				{
					break;
				}
			}
		}
		return ret;
	}
	CDWARFCompileUnit::DIEIdxTable CDWARFCompileUnit::getParentsIdxs(DIEIdx idx) const
	{
		DIEIdxTable ret;
		CDWARFCompileUnit::DIETable::const_iterator it = getNodeByIdx(idx);
		if(it != end() && ((*it).Depth > 0))
		{
			t_uint32 depth = ((*it).Depth - 1);
			for(DIETable::const_reverse_iterator i(it), e = m_debugInfoEntry.rend();
				i != e;
				++i)
			{
				t_uint32 cdepth = (*i).Depth;
				if(cdepth <= depth)
				{
					/*
						reference __CLR_OR_THIS_CALL const_reverse_iterator::operator*() const
						{	// return designated value
							_RanIt _Tmp = current;
							return (*--_Tmp);
						}
					*/
					ret.push_back(std::distance(begin() , i.base()) - 1);
					if((depth--) == 0) { break; }
				}
			}
		}
		return ret;
	}
	CDWARFCompileUnit::DIEIdxTable CDWARFCompileUnit::getChildrensIdxs(DIEIdx idx) const
	{
		DIEIdxTable ret;
		CDWARFCompileUnit::DIETable::const_iterator it = getNodeByIdx(idx);
		if(it != end() && ((*it).Depth > 0))
		{
			t_uint32 depth = ((*it).Depth + 1);
			for(DIETable::const_iterator i = (it + 1), e = end();
				i != e;
				++i)
			{
				if((*i).Depth == depth)
				{
					ret.push_back(std::distance(begin(), i));
				}
				else
				{
					break;
				}
			}
		}
		return ret;
	}
	CDWARFCompileUnit::DIEIdxTable CDWARFCompileUnit::getAllChildrensIdxs(DIEIdx idx) const
	{
		DIEIdxTable ret;
		CDWARFCompileUnit::DIETable::const_iterator it = getNodeByIdx(idx);
		if(it != end() && ((*it).Depth > 0))
		{
			t_uint32 depth = ((*it).Depth + 1);
			for(DIETable::const_iterator i = (it + 1), e = end();
				i != e;
				++i)
			{
				if((*i).Depth >= depth)
				{
					ret.push_back(std::distance(begin(), i));
				}
				else
				{
					break;
				}
			}
		}
		return ret;
	}
	bool CDWARFCompileUnit::fullExtract(const CDWARFDebugInfoEntry* die,
										const CDWARFAbbreviationTable& abbr_table,
										io::stream_base::ptr debug_str,
										io::stream_base::ptr stream)
	{
		bool ret = false;
		CDWARFCompileUnit::DIETable::iterator it = getNodeByDIE(die);
		if(it != end())
		{
			stream->seek((*it).Offset);
			ret = (*it).DIE.parse_all(abbr_table, *this, debug_str, stream);
		}
		return ret;
	}
	bool CDWARFCompileUnit::fullExtract(DIEIdx idx, const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str, io::stream_base::ptr stream)
	{
		bool ret = false;
		CDWARFCompileUnit::DIETable::iterator it = getNodeByIdx(idx);
		if(it != end())
		{
			stream->seek((*it).Offset);
			ret = (*it).DIE.parse_all(abbr_table, *this, debug_str, stream);
		}
		return ret;
	}
	CDWARFCompileUnit::DIETable::iterator CDWARFCompileUnit::getNodeByDIE(const CDWARFDebugInfoEntry* die)
	{
		DIETable::iterator i = begin();
		for(DIETable::iterator e = end();
			i != e && die != &((*i).DIE);
			++i);
		return i;
	}
	CDWARFCompileUnit::DIETable::const_iterator CDWARFCompileUnit::getNodeByDIE(const CDWARFDebugInfoEntry* die) const
	{
		DIETable::const_iterator i = begin();
		for(DIETable::const_iterator e = end();
			i != e && die != &((*i).DIE);
			++i);
		return i;
	}
	CDWARFCompileUnit::DIETable::iterator CDWARFCompileUnit::getNodeByIdx(DIEIdx idx)
	{
		CDWARFCompileUnit::DIETable::iterator i = end();
		if(idx != kINVALID_IDX)
		{
			i = begin() + idx;
		}
		return i;
	}
	CDWARFCompileUnit::DIETable::const_iterator CDWARFCompileUnit::getNodeByIdx(DIEIdx idx) const
	{
		CDWARFCompileUnit::DIETable::const_iterator i = end();
		if(idx != kINVALID_IDX)
		{
			i = begin() + idx;
		}
		return i;
	}
	CDWARFCompileUnit::DIETable::iterator CDWARFCompileUnit::getNodeByOffset(t_uint64 absolute_die_offset)
	{
		DIETable::iterator i = begin();
		for(DIETable::iterator e = end();
			i != e && (*i).Offset != absolute_die_offset;
			++i);
		return i;
	}
	CDWARFCompileUnit::DIETable::const_iterator CDWARFCompileUnit::getNodeByOffset(t_uint64 absolute_die_offset) const
	{
		DIETable::const_iterator i = begin();
		for(DIETable::const_iterator e = end();
			i != e && (*i).Offset != absolute_die_offset;
			++i);
		return i;
	}
	CDWARFCompileUnit::DIETable::iterator CDWARFCompileUnit::begin()
	{
		return m_debugInfoEntry.begin();
	}
	CDWARFCompileUnit::DIETable::const_iterator CDWARFCompileUnit::begin() const
	{
		return m_debugInfoEntry.begin();
	}
	CDWARFCompileUnit::DIETable::iterator CDWARFCompileUnit::end()
	{
		return m_debugInfoEntry.end();
	}
	CDWARFCompileUnit::DIETable::const_iterator CDWARFCompileUnit::end() const
	{
		return m_debugInfoEntry.end();
	}
	CDWARFCompileUnit::PDIETable CDWARFCompileUnit::getDIEBy(t_uint64 address, const utility::CDWARFRangeResolver& range_resolver) const
	{
		PDIETable ret;
		for(DIETable::const_iterator i = m_debugInfoEntry.begin(), e = m_debugInfoEntry.end();
			i != e;
			++i)
		{
#ifndef NDEBUG
			int idx = std::distance(m_debugInfoEntry.begin(), i);
#endif

			if((*i).DIE.isInRange(address, m_base_address, range_resolver))
			{
				ret.push_back(&(*i).DIE);
			}
		}
		return ret;
	}
	CDWARFCompileUnit::DIEIdxTable CDWARFCompileUnit::getDIEIdxsBy(t_uint64 address, const utility::CDWARFRangeResolver& range_resolver, const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str, io::stream_base::ptr stream)
	{
		DIEIdxTable ret;
		for(DIETable::iterator i = m_debugInfoEntry.begin(), e = m_debugInfoEntry.end();
			i != e;
			++i)
		{
#ifndef NDEBUG
			int idx = std::distance(begin(), i);
#endif
			if((*i).DIE.getTag() == DW_TAG_subprogram)
			{
				if((*i).DIE.isEmpty())
				{
					fullExtract(std::distance(begin(), i),abbr_table, debug_str, stream);
				}
				if((*i).DIE.isInRange(address, m_base_address, range_resolver))
				{
					ret.push_back(std::distance(begin(), i));

					DIEIdxTable idxs = getAllChildrensIdxs(std::distance(begin(), i));
					for(DIEIdxTable::const_iterator j = idxs.begin(), je = idxs.end();
						j != je;
						++j)
					{
						const CDWARFDebugInfoEntry* info = getDIEByIdx(*j);
						if(info && info->getTag() == DW_TAG_inlined_subroutine)
						{
							if(fullExtract(*j,abbr_table, debug_str, stream))
							{
								if(info->isInRange(address, m_base_address, range_resolver))
								{
									ret.push_back(*j);
								}
							}
							else
							{
								TRACE("[CDWARFCompileUnit] ERROR getDIEIdxsBy on fullExtract");
							}
						}
					}
				}
			}
		}
		return ret;
	}
	bool CDWARFCompileUnit::operator < (const CDWARFCompileUnit& other) const
	{
		return (m_length < other.m_length);
	}
}
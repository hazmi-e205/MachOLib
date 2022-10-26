#include "CSymbolTable.h"
#include "MachO.h"
#include <utility/CConsole.h>
#include <io/stream_memory.h>
#include <algorithm>

namespace macho
{
	const t_uint32 CSymbolTable::kINVALID_SYMBLO_IDX = 0xffffffffUL;

	CSymbolTable::CSymbolTable()
		:m_symbols(),
		m_offset(0),
		m_count(0),
		m_source(),
		m_isx64(false)
	{
		return;
	}
	CSymbolTable::CSymbolTable(const io::source_of_stream& source,macho::t_uint32 offset,macho::t_uint32 count,bool is_x64)
		:m_symbols(),
		m_offset(offset),
		m_count(count),
		m_source(source),
		m_isx64(is_x64)
	{
		return;
	}
	CSymbolTable::~CSymbolTable()
	{
		return;
	}
	void CSymbolTable::insert(const CSymbol& symbol)
	{
		std::vector<CSymbol>::iterator it = std::lower_bound(m_symbols.begin(), m_symbols.end(), symbol);
		m_symbols.insert(it, symbol);
		return;
	}
	t_uint32 CSymbolTable::getSymbolIdxFirstAtRange(const macho::t_uint64& address)
	{
		if(m_symbols.empty()) {
			preload();
		}
		std::vector<CSymbol>::iterator it = std::upper_bound(m_symbols.begin(), m_symbols.end(), CSymbol(address));
		return (it != m_symbols.end() && it != m_symbols.begin())?static_cast<t_uint32>((it - 1) - m_symbols.begin()):kINVALID_SYMBLO_IDX;
	}
	CSymbol& CSymbolTable::at(t_uint32 idx)
	{
		if(m_symbols.empty()) {
			preload();
		}
		return m_symbols[idx];
	}
	t_uint32 CSymbolTable::size()
	{
		if(m_symbols.empty()) {
			preload();
		}
		return m_symbols.size();
	}
	base::ERESULT_CODE CSymbolTable::preload()
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		io::stream_base::ptr stream = m_source.open();
		if(stream.get())
		{
			stream->seek(m_offset);

			t_uint32		idxOfStr;	/* index into the string table */
			t_uint8			flags;	/* type flag, see below */
			t_uint8			section;	/* section number or NO_SECT */
			t_uint16		desc;	/* see <mach-o/stab.h> */

			m_symbols.reserve(m_count);
			if(m_isx64)
			{
				static const int kSIZE_OF_STRUCT = (sizeof(idxOfStr)
													+ sizeof(flags)
													+ sizeof(section)
													+ sizeof(desc)
													+ sizeof(t_uint64));
				io::stream_memory memory(stream,m_count * kSIZE_OF_STRUCT);
				for(io::t_uint32 j = 0; j < m_count; ++j)
				{
					t_uint64 value;
					memory.t_read(idxOfStr);
					memory.t_read(flags);
					memory.t_read(section);
					memory.t_read(desc);
					memory.t_read(value);
					m_symbols.push_back(CSymbol(value, idxOfStr, flags, section));
				}
			}
			else
			{
				static const int kSIZE_OF_STRUCT = (sizeof(idxOfStr)
													+ sizeof(flags)
													+ sizeof(section)
													+ sizeof(desc)
													+ sizeof(t_uint32));
				io::stream_memory memory(stream,m_count * kSIZE_OF_STRUCT);
				for(io::t_uint32 j = 0; j < m_count; ++j)
				{
					t_uint32 value;
					memory.t_read(idxOfStr);
					memory.t_read(flags);
					memory.t_read(section);
					memory.t_read(desc);
					memory.t_read(value);
					m_symbols.push_back(CSymbol(value, idxOfStr, flags, section));
				}
			}
			std::sort(m_symbols.begin(),m_symbols.end());
			ret_code = base::ERESULT_CODE_SUCCESS;
		}
		else
		{
			TRACE("[CSymbolTable] ERROR preload (stream is not opened!!!)");
		}
		return ret_code;
	}
	io::stream_base::ptr  CSymbolTable::load() const
	{
		io::stream_base::ptr oret;
		io::stream_base::ptr stream = m_source.open();
		if(stream.get())
		{
			stream->seek(m_offset);
			if(m_isx64)
			{
				oret.reset(new io::stream_memory(stream, m_count * nlist_size));
			}
			else
			{
				oret.reset(new io::stream_memory(stream, m_count * nlist_64_size));
			}
		}
		return oret;
	}
}
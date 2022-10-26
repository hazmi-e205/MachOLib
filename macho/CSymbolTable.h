#ifndef __CSYMBOL_TABLE_H__
#define __CSYMBOL_TABLE_H__

#include <vector>
#include <base/base.h>
#include <io/source_of_stream.h>
#include <io/stream_base.h>
#include "CSymbol.h"

namespace macho
{
	class CSymbolTable
	{
	public:
		static const t_uint32 kINVALID_SYMBLO_IDX;
	private:
		std::vector<CSymbol>	m_symbols;
		t_uint32				m_offset;
		t_uint32				m_count;
		io::source_of_stream	m_source;
		bool					m_isx64;
	private:
		void insert(const CSymbol& symbol);
		base::ERESULT_CODE preload();
	public:
		CSymbolTable();
		CSymbolTable(const io::source_of_stream& source,macho::t_uint32 offset,macho::t_uint32 count,bool is_x64);
		~CSymbolTable();
		t_uint32 getSymbolIdxFirstAtRange(const macho::t_uint64& address);
		CSymbol& at(t_uint32 idx);
		t_uint32 size();
		io::stream_base::ptr load() const;
	};
}

#endif //__CSYMBOL_TABLE_H__
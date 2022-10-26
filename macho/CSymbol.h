#ifndef __CSYMBOL_H__
#define __CSYMBOL_H__

#include <string>
#include <base/base.h>
#include <io/stream_base.h>

namespace macho
{
	class CSymbol
	{
	private:
		t_uint64	m_address;
		t_uint32	m_str_idx;
		t_uint8		m_type;
		t_uint8		m_section;
		std::string m_demagled;
	public:
		static const CSymbol kNULL;
		static const t_uint8 kNO_SECT;
		static const t_uint8 kN_STAB;
		static const t_uint8 kN_PEXT;
		static const t_uint8 kN_EXT;
		static const t_uint8 kN_TYPE_MASK;
		static const t_uint8 kN_TYPE_UNDF;
		static const t_uint8 kN_TYPE_ABS;
		static const t_uint8 kN_TYPE_SECT;
		static const t_uint8 kN_TYPE_PBUD;
		static const t_uint8 kN_TYPE_INDR;
	public:
		CSymbol();
		explicit CSymbol(const t_uint64& address);
		explicit CSymbol(const t_uint64& address,t_uint32 str_idx,t_uint8 type,t_uint8 section);
		const t_uint64& address() const;
		const t_uint32& strIdx() const;
		const t_uint8& section() const;
		bool isDefined() const;
		bool isSTAB() const;
		bool isEXT() const;
		bool isPEXT() const;
		std::string& demagled();
		const std::string& demagled() const;
		bool operator <(const CSymbol &other) const;
	};
}

#endif //__CSYMBOL_H__
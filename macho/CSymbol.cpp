#include "CSymbol.h"
#include <utility/CConsole.h>

namespace macho
{
	const CSymbol CSymbol::kNULL;
	const t_uint8 CSymbol::kNO_SECT = 0;
	const t_uint8 CSymbol::kN_STAB = 0xE0;
	const t_uint8 CSymbol::kN_PEXT = 0x10;
	const t_uint8 CSymbol::kN_EXT = 0x01;
	const t_uint8 CSymbol::kN_TYPE_MASK = 0xE;
	const t_uint8 CSymbol::kN_TYPE_UNDF = 0x0;
	const t_uint8 CSymbol::kN_TYPE_ABS = 0x2;
	const t_uint8 CSymbol::kN_TYPE_SECT = 0xE;
	const t_uint8 CSymbol::kN_TYPE_PBUD = 0xC;
	const t_uint8 CSymbol::kN_TYPE_INDR = 0xA;

	CSymbol::CSymbol(const t_uint64& address)
		:m_address(address),
		m_str_idx(0),
		m_type(0),
		m_section(0),
		m_demagled()
	{
		return;
	}
	CSymbol::CSymbol(const t_uint64& address,t_uint32 str_idx,t_uint8 type,t_uint8 section)
		:m_address(address),
		m_str_idx(str_idx),
		m_type(type),
		m_section(section),
		m_demagled()
	{
		//TRACE("[CSymbol] ctor with addr = 0x%llx str_idx = %x section = %hhu",m_address,m_str_idx,m_section);
		return;
	}
	CSymbol::CSymbol()
		:m_address(0),
		m_str_idx(0),
		m_section(0)
	{
		return;
	}
	const t_uint64& CSymbol::address() const {
		return m_address;
	}
	const t_uint32& CSymbol::strIdx() const {
		return m_str_idx;
	}
	const t_uint8& CSymbol::section() const {
		return m_section;
	}
	bool CSymbol::isDefined() const {
		t_uint8 is_stab = (m_type & kN_STAB);
		t_uint8 is_ext = (m_type & kN_EXT);
		t_uint8 is_pext = (m_type & kN_PEXT);
		t_uint8 type = (m_type & kN_TYPE_MASK);
		return (type == kN_TYPE_SECT && is_stab == 0);
	}
	bool CSymbol::isSTAB() const {
		t_uint8 is_stab = (m_type & kN_STAB);
		return (is_stab != 0);
	}
	bool CSymbol::isEXT() const {
		t_uint8 is_ext = (m_type & kN_EXT);
		return (is_ext != 0);
	}
	bool CSymbol::isPEXT() const {
		t_uint8 is_pext = (m_type & kN_PEXT);
		return (is_pext != 0);
	}
	std::string& CSymbol::demagled()
	{
		return m_demagled;
	}
	const std::string& CSymbol::demagled() const
	{
		return m_demagled;
	}
	bool CSymbol::operator <(const CSymbol &other) const
	{
		return (m_address < other.m_address);
	}
}
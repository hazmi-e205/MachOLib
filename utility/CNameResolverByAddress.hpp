#ifndef __NAME_RESOLVER_BY_ADDRESS_H__
#define __NAME_RESOLVER_BY_ADDRESS_H__

#include <string>
#include <macho/CFAT.h>
#include <dwarf/IDWARFNameResolverByAddress.h>

class CNameResolverByAddress : public dwarf::IDWARFNameResolverByAddress
{
private:
	macho::CMachO* m_macho;
public:
	CNameResolverByAddress(macho::CMachO& macho)
		:m_macho(&macho)
	{
		return;
	}
	~CNameResolverByAddress()
	{
		m_macho = NULL;
		return;
	}
	virtual std::string resolve(unsigned long long int address)
	{
		std::string ret;
		macho::CSymbolTable& tab = m_macho->getSymbolTable();
		macho::t_uint64 addr = address;

		macho::t_uint32 sym_idx = tab.getSymbolIdxFirstAtRange(addr);
		if(sym_idx != macho::CSymbolTable::kINVALID_SYMBLO_IDX) 
		{
			macho::CSymbol& sym = tab.at(sym_idx);
			ret = sym.demagled();
			if(sym.demagled().empty() && sym.isDefined() && sym.section() != macho::CSymbol::kNO_SECT)
			{
				std::string magled = m_macho->getSymbolAt(sym.strIdx());
				ret = sym.demagled() = macho::CMachO::Demangle(magled.c_str());
			}
		}
		return ret;
	}
};

#endif //__NAME_RESOLVER_BY_ADDRESS_H__
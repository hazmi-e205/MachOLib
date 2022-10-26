#include "CSymbolsPackW.h"
#include <macho/CMachO.h>

namespace MachO 
{

	CSymbolsPackW::CSymbolsPackW(const SymbolPackage::Ptr& app)
		:m_pack(app)
	{
	}
    std::wstring CSymbolsPackW::GetSymbolNameAt(std::wstring uuid, uint64_t address)
    {
        std::wstring ustr = uuid;
		std::string astr(ustr.begin(), ustr.end());
		std::string symbol = m_pack->getSymbol(astr, address);
		if(!symbol.empty())
		{
			std::string demangled = macho::CMachO::Demangle(symbol.c_str());
			if(!demangled.empty())
			{
				symbol = demangled;
			}
		}
        return std::wstring(symbol.begin(), symbol.end());
	}
}

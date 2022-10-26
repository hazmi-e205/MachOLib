#pragma once
#include <utility/zip/SymbolsPack.h>
#include <string>

namespace MachO
{
    class CSymbolsPackW
	{
	public:
		CSymbolsPackW(const SymbolPackage::Ptr& app);
        std::wstring GetSymbolNameAt(std::wstring uuid, uint64_t address);
	private:
        std::shared_ptr<SymbolPackage> m_pack;
	};
}

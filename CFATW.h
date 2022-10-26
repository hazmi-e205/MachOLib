#pragma once
#include <utility/CSearchMachO.h>
#include <utility/CConsole.h>
#include <dwarf/CDWARFDebugLine.h>
#include <list>

namespace MachO
{
    class CMachOW;
    class CFATW
	{
	protected:
		virtual macho::CFAT::Ptr pfat() = 0;
	public:
		virtual macho::CFAT& fat() = 0;
		virtual base::ERESULT_CODE get_uuids(std::vector<std::string>& uuid_list);
		const dwarf::CDWARFDebugLine* debug_lines(const std::wstring& uuid);
	public:
		CFATW();
		~CFATW();
        //!CFATW();
        bool HasMachO(std::wstring uuid);
        CMachOW* GetMachO(std::wstring arch_uuid);
        std::list<std::wstring> GetUUIDs();
        std::wstring GetSymbols(std::wstring arch_uuid, std::wstring with);
        std::wstring GetSymbolNameAt(std::wstring uuid, uint64_t address);
        bool HasDebugInformation(std::wstring uuid);
        std::wstring DumpDebugLines(std::wstring uuid);
        static bool Export(std::list<CFATW*> fat_list, std::wstring exportFolder);
	};
}

#include "CFATW.h"
#include "CMachOW.h"
//#include "ConsoleW.h"
#include <dwarf/CDWARFDebugLine.h>
#include <utility/CNameResolverByAddress.hpp>
#include <utility/MachOBuilder.h>

namespace MachO 
{
	CFATW::CFATW()
	{
		return;
	}
	CFATW::~CFATW()
	{
		return;
	}
    /*CFATW::!CFATW()
    {
        return;
    }*/
	base::ERESULT_CODE CFATW::get_uuids(std::vector<std::string>& uuid_list)
	{
		return  fat().getAllUUIDs(uuid_list);
	}
    bool CFATW::HasMachO(std::wstring uuid)
    {
        return fat().isExistMachO(uuid);
	}
    CMachOW* CFATW::GetMachO(std::wstring arch_uuid)
    {
        return new CMachOW(fat().getMachO(arch_uuid));
	}
	const dwarf::CDWARFDebugLine* CFATW::debug_lines(const std::wstring& uuid)
	{
		if(fat().isExistMachO(uuid))
		{
			macho::CMachO& macho = fat().getMachODeprecated(uuid);
			if(macho.hasDebugLines()) 
			{
				return macho.getDebugLines().get();
			}
		}
		return NULL;
	}
    std::list<std::wstring> CFATW::GetUUIDs()
	{
        std::list<std::wstring> uuids;
		std::vector<std::string> v;
		get_uuids(v);
		for(std::vector<std::string>::iterator i = v.begin(),e = v.end();
			i != e;
			++i)
		{
            uuids.push_back(std::wstring((*i).begin(), (*i).end()));
		}
		return uuids;
	}
    std::wstring CFATW::GetSymbols(std::wstring arch_uuid, std::wstring with)
	{
        char s_num_buffer[64] = {0};
        std::wstring with_w(with);
		std::string with_ascii(with_w.begin(),with_w.end()), all_symbols;

        if(fat().isExistMachO(arch_uuid))
		{
            macho::CMachO& macho = fat().getMachODeprecated(arch_uuid);
			macho::CSymbolTable& tab = macho.getSymbolTable();

			macho::CSegment::Ptr seg = macho.getSegment(macho::CSegment::kSEGMENT_TEXT);
			if(seg.get() != NULL)
			{
				macho::t_uint64 vmaddr = seg->getVmAddress();

				all_symbols.reserve(tab.size() * 10);
				for(macho::t_uint32 i = 0, e = tab.size(); i < e; ++i)
				{
					std::string line;
					macho::CSymbol& sym = tab.at(i);
					if(!sym.isSTAB()) {
						if(sym.demagled().empty()) {
							std::string magled = macho.getSymbolAt(sym.strIdx());
							sym.demagled() = macho::CMachO::Demangle(magled.c_str());
						}
						{
							line.append(sym.demagled());
							line.append(" [addr: ");
							sprintf_s<sizeof(s_num_buffer)>(s_num_buffer,"0x%llx",(sym.address() - vmaddr));
							line.append(s_num_buffer);
							if(!sym.isEXT()) {
								line.append("] [sect: ");
								sprintf_s<sizeof(s_num_buffer)>(s_num_buffer,"0x%hhu",sym.section());
								line.append(s_num_buffer);
								line.append("]\r\n");
							}
							else {
								line.append("] [sect: EXTERN]\r\n");
							}
						}
						if(with_ascii.empty() || line.find(with_ascii) != std::string::npos) {
							all_symbols.append(line);
						}
					}
				}
			}
            return std::wstring(all_symbols.begin(), all_symbols.end());
		}
		else
		{
            return std::wstring(L"undefined uuid");
		}
	}
    std::wstring CFATW::GetSymbolNameAt(std::wstring uuid, uint64_t address)
	{
        char s_num_buffer[64] = {0};
        if(fat().isExistMachO(uuid))
		{
			std::string demagled = "undefined";
            macho::CMachO& macho = fat().getMachODeprecated(uuid);
			macho::CSymbolTable& tab = macho.getSymbolTable();
			macho::CSegment::Ptr seg = macho.getSegment(macho::CSegment::kSEGMENT_TEXT);
			if(seg.get() != NULL)
			{
				macho::t_uint64 vm_addr = seg->getVmAddress();
				macho::t_uint64 addr = address + vm_addr;

				macho::t_uint32 sym_idx = tab.getSymbolIdxFirstAtRange(addr);
				if(sym_idx != macho::CSymbolTable::kINVALID_SYMBLO_IDX) {
					macho::CSymbol& sym = tab.at(sym_idx);
					if(sym.isDefined() && sym.section() != macho::CSymbol::kNO_SECT)
					{
						std::string file;
						{
                            if(debug_lines(uuid) != NULL)
							{
                                const dwarf::CDWARFDebugLine::CDWARFLineTable& t1 = debug_lines(uuid)->getLineTable(addr);
								unsigned int idx_of_row = t1.lookupAddress(addr,0);
								if(idx_of_row != dwarf::CDWARFDebugLine::CDWARFLineTable::kBAD_ROW_IDX)
								{
									const dwarf::CDWARFDebugLine::CDWARFRow& r = t1.at(idx_of_row);
									file = t1.getFile(r);
									sprintf_s<sizeof(s_num_buffer)>(s_num_buffer,"%u",r.Line);
									file.append(1,':');
									file.append(s_num_buffer);
								}
							}
						}
						std::string magled = macho.getSymbolAt(sym.strIdx());
						demagled = sym.demagled() = macho::CMachO::Demangle(magled.c_str());
						if(!file.empty())
						{
							demagled.append(" (");
							demagled.append(file);
							demagled.append(1,')');
						}
					}
				}
			}
            return std::wstring(demagled.begin(), demagled.end());
		}
		else
		{
            return std::wstring(L"undefined uuid");
		}
	}
    bool CFATW::HasDebugInformation(std::wstring uuid)
	{
        bool ret = false;
        if(fat().isExistMachO(uuid))
		{
            macho::CMachO& macho = fat().getMachODeprecated(uuid);
			macho::CSegment::Ptr dwarf = macho.getSegment(macho::CSegment::kSEGMENT_DWARF);
			ret = (dwarf.get() != NULL && dwarf->isExistSection(macho::CSection::kSECTION_DEBUG_LINE));
		}
		return ret;
	}
    std::wstring CFATW::DumpDebugLines(std::wstring uuid)
	{
        std::wstring ret = L"";
		if(HasDebugInformation(uuid))
		{
            macho::CMachO& macho = fat().getMachODeprecated(uuid);
			macho::CSegment::Ptr dwarf = macho.getSegment(macho::CSegment::kSEGMENT_DWARF);
			if(dwarf.get() != NULL)
			{
				macho::CSection::Ptr sec_lines = dwarf->getSection(macho::CSection::kSECTION_DEBUG_LINE);
				if(sec_lines.get() != NULL)
				{
					dwarf::CDWARFDebugLine lines;
					if(lines.parse(sec_lines->load()))
					{
						CNameResolverByAddress resolver(macho);
						std::string sret = lines.dump(&resolver);
                        ret = std::wstring(sret.begin(), sret.end());
					}
				}
			}
		}
		return ret;
	}
    bool CFATW::Export(std::list<CFATW*> fat_list, std::wstring exportFolder)
    {
		std::vector<macho::CFAT::Ptr> fats;
        for (auto const& _fat : fat_list)
        {
            macho::CFAT::Ptr fat = _fat->pfat();
            fats.push_back(fat);
        }
        std::wstring folder(exportFolder);
		MachOBuilder::BuildMultiFat(fats, folder, io::stream_base::ptr(), NULL);
		return true;
	}
}

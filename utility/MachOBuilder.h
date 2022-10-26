#ifndef __MACHO_BUILDER_H__
#define __MACHO_BUILDER_H__

#include <map>
#include <types.h>
#include <shared_ptr_alias.hpp>
#include <base/base.h>
#include <macho/CFAT.h>
#include <macho/CSegment.h>
#include <macho/EMagic.h>
#include <macho/ELoadCommands.h>
#include <io/source_of_stream.h>
#include <io/stream_base.h>
#include <dwarf/CDWARFDebugLine.h>
#include <dwarf/CDWARFAbbreviationTable.h>
#include <dwarf/CDWARFCompileUnitTable.h>
#include <utility/CConsole.h>
struct MachOBuilder
{
	typedef bool (*ContinueCallback)(int persent);
	static bool BuildMultiFat(const std::vector<macho::CFAT::Ptr>& input_fat, const std::wstring& transit_folder, io::stream_base::ptr ostr, ContinueCallback callback);
	static std::vector<char> BuildMiniMachO(macho::CMachO& macho);
};

#endif //__MACHO_BUILDER_H__
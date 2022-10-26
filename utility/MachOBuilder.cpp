#include "MachOBuilder.h"
#include <macho/UUID.h>
#include <macho/MachOWrite.hpp>

using namespace macho;

bool MachOBuilder::BuildMultiFat(const std::vector<macho::CFAT::Ptr>& input_fat, const std::wstring& transit_folder, io::stream_base::ptr ostr, ContinueCallback callback)
{
	//ostr->set_encoding(io::EENCODING_LITTLE_ENDIAN);
	std::map<std::string,macho::CMachO::Ptr> machos;
	for(size_t i = 0; i < input_fat.size(); ++i)
	{
		for(int j = 0, e = input_fat[i]->count(); j < e; ++j)
		{
			if(input_fat[i]->getMachOAt(j)->hasUUID() && !input_fat[i]->getMachOAt(j)->getUUID().empty())
			{
				machos[input_fat[i]->getMachOAt(j)->getUUID()] = input_fat[i]->getMachOAt(j);
			}
		}
	}
	t_uint32 magic = kFAT_MAGIC;
	t_uint32 count_of_archs = machos.size();
	//ostr->t_write(magic);
	//ostr->t_write(count_of_archs);
	//make all mini machos
	std::set<std::string> symbols;
	for(std::map<std::string,macho::CMachO::Ptr>::iterator i = machos.begin(), e = machos.end();
		i != e;
		++i)
	{
		CMachO& macho = *(*i).second;
		macho::CSymbolTable& table = macho.getSymbolTable();
		for(size_t j = 0, je = table.size(); j < je; ++j)
		{
			macho::CSymbol& symbol = table.at(j);
			symbols.insert(macho.getSymbolAt(symbol.strIdx()));
		}
		/*bool isnt_open = true;
		io::stream_base::ptr  minimacho(new io::stream_file((transit_folder + L"\\" + std::wstring((*i).first.begin(), (*i).first.end()) + L".temp.dylib").c_str(),false, true, isnt_open));
		assert(!isnt_open);
		if(!isnt_open)
		{
			std::vector<char> mmacho = BuildMiniMachO(macho);
			minimacho->write(&mmacho[0], mmacho.size());
		}*/
	}
	return true;
}
std::vector<char> MachOBuilder::BuildMiniMachO(macho::CMachO& macho)
{
	std::vector<char> oret;
	if((macho.getTypeOfCPU()&ECPU_ABI64_FLAG) == 0)
	{
		static const t_uint32 commands_size = symtab_command_size + segment_command_size + uuid_command_size;
		io::stream_base::ptr symtabmem = macho.getSymbolTable().load();
		io::stream_base::ptr stringtab = macho.loadStringTable();
		io::bstreamsize full_size = mach_header_size + commands_size + symtabmem->size() + stringtab->size();
		io::stream_memory minimacho(full_size);
		minimacho.set_encoding(macho.getEncoding());
		//make headers x32
		mach_header header = mach_header::Make(CMachO::kMACHO_MAGIC,
															macho.getTypeOfCPU(),
															macho.getSubTypeOfCPU(),
															macho.getTypeOfFile(),
															3,
															commands_size,
															0);
		char uuid[kUUID_BYTE_SIZE];
		UUIDStr2Mem(macho.getUUID(), uuid);
		uuid_command uuidcommand = uuid_command::Make(ELOAD_COMMAND_UUID,
															uuid_command_size,
															uuid);

		symtab_command tabcommand = symtab_command::Make(ELOAD_COMMAND_SYMTAB,
															symtab_command_size,
															mach_header_size + commands_size,
															macho.getSymbolTable().size(),
															(t_uint32)(mach_header_size + commands_size + symtabmem->size()),
															(t_uint32)stringtab->size());

		segment_command segcommand = segment_command::Make(ELOAD_COMMAND_SEGMENT,
															segment_command_size,
															CSegment::kSEGMENT_TEXT,
															(t_uint32)macho.getSegment(CSegment::kSEGMENT_TEXT)->getVmAddress(),
															0,
															0,
															0,
															0,
															0,
															0,
															0);
		//write headers
		write(header, minimacho);
		write(uuidcommand, minimacho);
		write(tabcommand, minimacho);
		write(segcommand, minimacho);
		minimacho.swrite(symtabmem, symtabmem->size());
		minimacho.swrite(stringtab, stringtab->size());
		//copy to buffer
		oret.resize((size_t)full_size);
		minimacho.seek(0);
		minimacho.read(&oret[0], oret.size());
	}
	else
	{
		static const t_uint32 commands_size = symtab_command_size + segment_command_64_size + uuid_command_size;
		io::stream_base::ptr symtabmem = macho.getSymbolTable().load();
		io::stream_base::ptr stringtab = macho.loadStringTable();
		io::bstreamsize full_size = mach_header_size + commands_size + symtabmem->size() + stringtab->size();
		io::stream_memory minimacho(full_size);
		minimacho.set_encoding(macho.getEncoding());
		//make headers x64
		mach_header_64 header64 = mach_header_64::Make(CMachO::kMACHO_MAGIC_64,
																	macho.getTypeOfCPU(),
																	macho.getSubTypeOfCPU(),
																	macho.getTypeOfFile(),
																	3,
																	commands_size,
																	0,
																	0);
		char uuid[kUUID_BYTE_SIZE];
		UUIDStr2Mem(macho.getUUID(), uuid);
		uuid_command uuidcommand = uuid_command::Make(ELOAD_COMMAND_UUID,
																	uuid_command_size,
																	uuid);

		symtab_command tabcommand = symtab_command::Make(ELOAD_COMMAND_SYMTAB,
																	symtab_command_size,
																	mach_header_64_size + commands_size,
																	macho.getSymbolTable().size(),
																	(t_uint32)(mach_header_64_size + commands_size + symtabmem->size()),
																	(t_uint32)stringtab->size());

		segment_command_64 segcommand_64 = segment_command_64::Make(ELOAD_COMMAND_SEGMENT_64,
																	segment_command_64_size,
																	CSegment::kSEGMENT_TEXT,
																	macho.getSegment(CSegment::kSEGMENT_TEXT)->getVmAddress(),
																	0,
																	0,
																	0,
																	0,
																	0,
																	0,
																	0);
		write(header64, minimacho);
		write(uuidcommand, minimacho);
		write(tabcommand, minimacho);
		write(segcommand_64, minimacho);
		minimacho.swrite(symtabmem, symtabmem->size());
		minimacho.swrite(stringtab, stringtab->size());
		//copy to buffer
		oret.resize((size_t)full_size);
		minimacho.seek(0);
		minimacho.read(&oret[0], oret.size());
	}
	return oret;
}
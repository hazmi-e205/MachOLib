#ifndef __MACHO_WRITE_STRUCTS_H__
#define __MACHO_WRITE_STRUCTS_H__

#include "MachO.h"
#include <io/stream_base.h>

namespace macho
{
	io::bstreamsize write(const mach_header& header, io::stream_base& ores)
	{
		io::bstreamsize osize = 0, temp;
		if((temp = ores.t_write(header.magic)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.cputype)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.cpusubtype)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.filetype)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.ncmds)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.sizeofcmds)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.flags)) == 0) { return 0ULL; } else { osize += temp; }
		return osize;
	}

	io::bstreamsize write(const mach_header_64& header, io::stream_base& ores)
	{
		io::bstreamsize osize = 0, temp;
		if((temp = ores.t_write(header.magic)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.cputype)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.cpusubtype)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.filetype)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.ncmds)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.sizeofcmds)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.flags)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.reserved)) == 0) { return 0ULL; } else { osize += temp; }
		return osize;
	}
	io::bstreamsize write(const segment_command& header, io::stream_base& ores)
	{
		io::bstreamsize osize = 0, temp;
		if((temp = ores.t_write(header.cmd)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.cmdsize)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.write(header.segname, sizeof(header.segname))) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.vmaddr)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.vmsize)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.fileoff)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.filesize)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.maxprot)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.initprot)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.nsects)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.flags)) == 0) { return 0ULL; } else { osize += temp; }
		return osize;
	}
	io::bstreamsize write(const segment_command_64& header, io::stream_base& ores)
	{
		io::bstreamsize osize = 0, temp;
		if((temp = ores.t_write(header.cmd)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.cmdsize)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.write(header.segname, sizeof(header.segname))) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.vmaddr)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.vmsize)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.fileoff)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.filesize)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.maxprot)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.initprot)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.nsects)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(header.flags)) == 0) { return 0ULL; } else { osize += temp; }
		return osize;
	}
	io::bstreamsize write(const symtab_command& symtab, io::stream_base& ores)
	{
		io::bstreamsize osize = 0, temp;
		if((temp = ores.t_write(symtab.cmd)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(symtab.cmdsize)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(symtab.symoff)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(symtab.nsyms)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(symtab.stroff)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(symtab.strsize)) == 0) { return 0ULL; } else { osize += temp; }
		return osize;
	}
	io::bstreamsize write(const uuid_command& uuid, io::stream_base& ores)
	{
		io::bstreamsize osize = 0, temp;
		if((temp = ores.t_write(uuid.cmd)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.t_write(uuid.cmdsize)) == 0) { return 0ULL; } else { osize += temp; }
		if((temp = ores.write(uuid.uuid, sizeof(uuid.uuid))) == 0) { return 0ULL; } else { osize += temp; }
		return osize;
	}
}

#endif //__MACHO_WRITE_STRUCTS_H__
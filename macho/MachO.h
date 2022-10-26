#ifndef __MACHO_STRUCTS_H__
#define __MACHO_STRUCTS_H__

#include <base/base.h>
#include <cstring>

namespace macho
{
	struct mach_header {
		t_uint32 magic;
		t_uint32 cputype;
		t_uint32 cpusubtype;
		t_uint32 filetype;
		t_uint32 ncmds;
		t_uint32 sizeofcmds;
		t_uint32 flags;

		static mach_header Make(t_uint32 pmagic,
								t_uint32 pcputype,
								t_uint32 pcpusubtype,
								t_uint32 pfiletype,
								t_uint32 pncmds,
								t_uint32 psizeofcmds,
								t_uint32 pflags)
		{
			mach_header oret;
			oret.magic = pmagic;
			oret.cputype = pcputype;
			oret.cpusubtype = pcpusubtype;
			oret.filetype = pfiletype;
			oret.ncmds = pncmds;
			oret.sizeofcmds = psizeofcmds;
			oret.flags = pflags;
			return oret;
		}
	};

	enum { mach_header_size = sizeof(mach_header) };

	struct mach_header_64 {
		t_uint32 magic;
		t_uint32 cputype;
		t_uint32 cpusubtype;
		t_uint32 filetype;
		t_uint32 ncmds;
		t_uint32 sizeofcmds;
		t_uint32 flags;
		t_uint32 reserved;

		static mach_header_64 Make(t_uint32 pmagic,
								t_uint32 pcputype,
								t_uint32 pcpusubtype,
								t_uint32 pfiletype,
								t_uint32 pncmds,
								t_uint32 psizeofcmds,
								t_uint32 pflags,
								t_uint32 preserved)
		{
			mach_header_64 oret;
			oret.magic = pmagic;
			oret.cputype = pcputype;
			oret.cpusubtype = pcpusubtype;
			oret.filetype = pfiletype;
			oret.ncmds = pncmds;
			oret.sizeofcmds = psizeofcmds;
			oret.flags = pflags;
			oret.reserved = preserved;
			return oret;
		}
	};

	enum { mach_header_64_size = sizeof(mach_header_64) };

	struct load_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
	};

	struct segment_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		char segname[16];
		t_uint32 vmaddr;
		t_uint32 vmsize;
		t_uint32 fileoff;
		t_uint32 filesize;
		t_uint32 maxprot;
		t_uint32 initprot;
		t_uint32 nsects;
		t_uint32 flags;

		static segment_command Make(t_uint32 cmd,
									t_uint32 cmdsize,
									const char segname[16],
									t_uint32 vmaddr,
									t_uint32 vmsize,
									t_uint32 fileoff,
									t_uint32 filesize,
									t_uint32 maxprot,
									t_uint32 initprot,
									t_uint32 nsects,
									t_uint32 flags)
		{
			segment_command oret;
			oret.cmd = cmd;
			oret.cmdsize = cmdsize;
			memset(oret.segname,0,sizeof(oret.segname));
#pragma warning(push)
#pragma warning( disable : 4996)
			strncat(oret.segname, segname, sizeof(oret.segname));
#pragma warning( pop )  
			oret.vmaddr = vmaddr;
			oret.vmsize = vmsize;
			oret.fileoff = fileoff;
			oret.filesize = filesize;
			oret.maxprot = maxprot;
			oret.initprot = initprot;
			oret.nsects = nsects;
			oret.flags = flags;
			return oret;
		}
	};

	enum { segment_command_size = 16 + (10 * 4) };

	struct segment_command_64 {
		t_uint32 cmd;
		t_uint32 cmdsize;
		char segname[16];
		t_uint64 vmaddr;
		t_uint64 vmsize;
		t_uint64 fileoff;
		t_uint64 filesize;
		t_uint32 maxprot;
		t_uint32 initprot;
		t_uint32 nsects;
		t_uint32 flags;

		static segment_command_64 Make(t_uint32 cmd,
									t_uint32 cmdsize,
									const char segname[16],
									t_uint64 vmaddr,
									t_uint64 vmsize,
									t_uint64 fileoff,
									t_uint64 filesize,
									t_uint32 maxprot,
									t_uint32 initprot,
									t_uint32 nsects,
									t_uint32 flags)
		{
			segment_command_64 oret;
			oret.cmd = cmd;
			oret.cmdsize = cmdsize;
			memset(oret.segname,0,sizeof(oret.segname));
#pragma warning(push)
#pragma warning(disable:4996)
			strncat(oret.segname, segname, sizeof(oret.segname));
#pragma warning(pop)  
			oret.vmaddr = vmaddr;
			oret.vmsize = vmsize;
			oret.fileoff = fileoff;
			oret.filesize = filesize;
			oret.maxprot = maxprot;
			oret.initprot = initprot;
			oret.nsects = nsects;
			oret.flags = flags;
			return oret;
		}
	};

	enum { segment_command_64_size = 16 + (6 * 4) + (4 * 8) };

	struct section {
		char sectname[16];
		char segname[16];
		t_uint32 addr;
		t_uint32 size;
		t_uint32 offset;
		t_uint32 align;
		t_uint32 reloff;
		t_uint32 nreloc;
		t_uint32 flags;
		t_uint32 reserved1;
		t_uint32 reserved2;
	};

	struct section_64 {
		char sectname[16];
		char segname[16];
		t_uint64 addr;
		t_uint64 size;
		t_uint32 offset;
		t_uint32 align;
		t_uint32 reloff;
		t_uint32 nreloc;
		t_uint32 flags;
		t_uint32 reserved1;
		t_uint32 reserved2;
		t_uint32 reserved3;
	};

	struct fvmlib {
		t_uint32 name;
		t_uint32 minor_version;
		t_uint32 header_addr;
	};

	struct fvmlib_command {
		t_uint32  cmd;
		t_uint32 cmdsize;
		struct fvmlib fvmlib;
	};

	struct dylib {
		t_uint32 name;
		t_uint32 timestamp;
		t_uint32 current_version;
		t_uint32 compatibility_version;
	};

	struct dylib_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		struct dylib dylib;
	};

	struct sub_framework_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 umbrella;
	};

	struct sub_client_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 client;
	};

	struct sub_umbrella_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 sub_umbrella;
	};

	struct sub_library_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 sub_library;
	};

	struct prebound_dylib_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 name;
		t_uint32 nmodules;
		t_uint32 linked_modules;
	};

	struct dylinker_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 name;
	};

	struct thread_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
	};

	struct routines_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 init_address;
		t_uint32 init_module;
		t_uint32 reserved1;
		t_uint32 reserved2;
		t_uint32 reserved3;
		t_uint32 reserved4;
		t_uint32 reserved5;
		t_uint32 reserved6;
	};

	struct routines_command_64 {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint64 init_address;
		t_uint64 init_module;
		t_uint64 reserved1;
		t_uint64 reserved2;
		t_uint64 reserved3;
		t_uint64 reserved4;
		t_uint64 reserved5;
		t_uint64 reserved6;
	};

	struct symtab_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 symoff;
		t_uint32 nsyms;
		t_uint32 stroff;
		t_uint32 strsize;

		static symtab_command Make(t_uint32 cmd,
								t_uint32 cmdsize,
								t_uint32 symoff,
								t_uint32 nsyms,
								t_uint32 stroff,
								t_uint32 strsize)
		{
			symtab_command oret;
			oret.cmd = cmd;
			oret.cmdsize = cmdsize;
			oret.symoff = symoff;
			oret.nsyms = nsyms;
			oret.stroff = stroff;
			oret.strsize = strsize;
			return oret;
		}
	};

	enum { symtab_command_size = (6 * 4) };

	struct dysymtab_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 ilocalsym;
		t_uint32 nlocalsym;
		t_uint32 iextdefsym;
		t_uint32 nextdefsym;
		t_uint32 iundefsym;
		t_uint32 nundefsym;
		t_uint32 tocoff;
		t_uint32 ntoc;
		t_uint32 modtaboff;
		t_uint32 nmodtab;
		t_uint32 extrefsymoff;
		t_uint32 nextrefsyms;
		t_uint32 indirectsymoff;
		t_uint32 nindirectsyms;
		t_uint32 extreloff;
		t_uint32 nextrel;
		t_uint32 locreloff;
		t_uint32 nlocrel;
	};	

	struct dylib_table_of_contents {
		t_uint32 symbol_index;
		t_uint32 module_index;
	};	

	struct dylib_module {
		t_uint32 module_name;
		t_uint32 iextdefsym;
		t_uint32 nextdefsym;
		t_uint32 irefsym;
		t_uint32 nrefsym;
		t_uint32 ilocalsym;
		t_uint32 nlocalsym;
		t_uint32 iextrel;
		t_uint32 nextrel;
		t_uint32 iinit_iterm;
		t_uint32 ninit_nterm;
		t_uint32 objc_module_info_addr;
		t_uint32 objc_module_info_size;
	};	

	struct dylib_module_64 {
		t_uint32 module_name;
		t_uint32 iextdefsym;
		t_uint32 nextdefsym;
		t_uint32 irefsym;
		t_uint32 nrefsym;
		t_uint32 ilocalsym;
		t_uint32 nlocalsym;
		t_uint32 iextrel;
		t_uint32 nextrel;
		t_uint32 iinit_iterm;
		t_uint32 ninit_nterm;
		t_uint32 objc_module_info_size;
		t_uint64 objc_module_info_addr;
	};

	struct dylib_reference {
		t_uint32 isym:24,
					flags:8;
	};


	struct twolevel_hints_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 offset;
		t_uint32 nhints;
	};

	struct twolevel_hint {
		t_uint32 isub_image:8,
					itoc:24;
	};

	struct prebind_cksum_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 cksum;
	};

	struct uuid_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		char uuid[16];

		
		static uuid_command Make(t_uint32 cmd,
									t_uint32 cmdsize,
									char uuid[16])
		{
			uuid_command oret;
			oret.cmd = cmd;
			oret.cmdsize = cmdsize;
			memset(oret.uuid,0,sizeof(oret.uuid));
#pragma warning(push)
#pragma warning( disable : 4996)
			memcpy(oret.uuid, uuid, sizeof(oret.uuid));
#pragma warning( pop )
			return oret;
		}
	};

	enum { uuid_command_size = (8 + 16) };

	struct rpath_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 path;
	};

	struct linkedit_data_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 dataoff;
		t_uint32 datasize;
	};

	struct encryption_info_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 cryptoff;
		t_uint32 cryptsize;
		t_uint32 cryptid;
	};

	struct version_min_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 version;
		t_uint32 reserved;
	};

	struct dyld_info_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 rebase_off;
		t_uint32 rebase_size;
		t_uint32 bind_off;
		t_uint32 bind_size;
		t_uint32 weak_bind_off;
		t_uint32 weak_bind_size;
		t_uint32 lazy_bind_off;
		t_uint32 lazy_bind_size;
		t_uint32 export_off;
		t_uint32 export_size;
	};

	struct symseg_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 offset;
		t_uint32 size;
	};

	struct ident_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
	};

	struct fvmfile_command {
		t_uint32 cmd;
		t_uint32 cmdsize;
		t_uint32 name;
		t_uint32 header_addr;
	};


	// Structs from <mach-o/fat.h>
	struct fat_header {
		t_uint32 magic;
		t_uint32 nfat_arch;
	};

	struct fat_arch {
		t_uint32 cputype;
		t_uint32 cpusubtype;
		t_uint32 offset;
		t_uint32 size;
		t_uint32 align;
	};

	// Structs from <mach-o/fat.h>
	struct nlist {
		t_uint32 n_strx;
		t_uint8 n_type;
		t_uint8 n_sect;
		t_int16 n_desc;
		t_uint32 n_value;
	};

	enum { nlist_size = 4 + 1 + 1 + 2 + 4 };

	struct nlist_64 {
		t_uint32 n_strx;
		t_uint8 n_type;
		t_uint8 n_sect;
		t_uint16 n_desc;
		t_uint64 n_value;
	};

	enum { nlist_64_size = 4 + 1 + 1 + 2 + 8 };

}//end of namespace macho

#endif //__MACHO_STRUCTS_H__

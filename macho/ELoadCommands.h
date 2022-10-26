#ifndef __ELOAD_COMMANDS_H__
#define __ELOAD_COMMANDS_H__

namespace macho
{
	enum ELOAD_COMMAND
	{
		ELOAD_COMMAND_REQ_DYLD = 0x80000000,
	/* Constants for the cmd field of all load commands, the type */
		ELOAD_COMMAND_SEGMENT = 0x1,	/* segment of this file to be mapped */
		ELOAD_COMMAND_SYMTAB = 0x2,	/* link-edit stab symbol table info */
		ELOAD_COMMAND_SYMSEG = 0x3,	/* link-edit gdb symbol table info (obsolete) */
		ELOAD_COMMAND_THREAD = 0x4,	/* thread */
		ELOAD_COMMAND_UNIXTHREAD = 0x5,	/* unix thread (includes a stack) */
		ELOAD_COMMAND_LOADFVMLIB = 0x6,	/* load a specified fixed VM shared library */
		ELOAD_COMMAND_IDFVMLIB = 0x7,	/* fixed VM shared library identification */
		ELOAD_COMMAND_IDENT = 0x8,	/* object identification info (obsolete) */
		ELOAD_COMMAND_FVMFILE = 0x9,	/* fixed VM file inclusion (internal use) */
		ELOAD_COMMAND_PREPAGE = 0xa,     /* prepage command (internal use) */
		ELOAD_COMMAND_DYSYMTAB = 0xb,	/* dynamic link-edit symbol table info */
		ELOAD_COMMAND_LOAD_DYLIB = 0xc,	/* load a dynamically linked shared library */
		ELOAD_COMMAND_ID_DYLIB = 0xd,	/* dynamically linked shared lib ident */
		ELOAD_COMMAND_LOAD_DYLINKER = 0xe,	/* load a dynamic linker */
		ELOAD_COMMAND_ID_DYLINKER = 0xf,	/* dynamic linker identification */
		ELOAD_COMMAND_PREBOUND_DYLIB = 0x10,	/* modules prebound for a dynamically */
					/*  linked shared library */
		ELOAD_COMMAND_ROUTINES = 0x11,	/* image routines */
		ELOAD_COMMAND_SUB_FRAMEWORK = 0x12,	/* sub framework */
		ELOAD_COMMAND_SUB_UMBRELLA = 0x13,	/* sub umbrella */
		ELOAD_COMMAND_SUB_CLIENT = 0x14,	/* sub client */
		ELOAD_COMMAND_SUB_LIBRARY = 0x15,	/* sub library */
		ELOAD_COMMAND_TWOLEVEL_HINTS = 0x16,	/* two-level namespace lookup hints */
		ELOAD_COMMAND_PREBIND_CKSUM = 0x17,	/* prebind checksum */

	/*
	 * load a dynamically linked shared library that is allowed to be missing
	 * (all symbols are weak imported).
	 */
		ELOAD_COMMAND_LOAD_WEAK_DYLIB = (0x18 | ELOAD_COMMAND_REQ_DYLD),

		ELOAD_COMMAND_SEGMENT_64 = 0x19,	/* 64-bit segment of this file to be
					   mapped */
		ELOAD_COMMAND_ROUTINES_64 = 0x1a,	/* 64-bit image routines */
		ELOAD_COMMAND_UUID = 0x1b,	/* the uuid */
		ELOAD_COMMAND_RPATH = (0x1c | ELOAD_COMMAND_REQ_DYLD),    /* runpath additions */
		ELOAD_COMMAND_CODE_SIGNATURE = 0x1d,	/* local of code signature */
		ELOAD_COMMAND_SEGMENT_SPLIT_INFO = 0x1e, /* local of info to split segments */
		ELOAD_COMMAND_REEXPORT_DYLIB = (0x1f | ELOAD_COMMAND_REQ_DYLD), /* load and re-export dylib */
		ELOAD_COMMAND_LAZY_LOAD_DYLIB = 0x20,	/* delay load of dylib until first use */
		ELOAD_COMMAND_ENCRYPTION_INFO = 0x21,	/* encrypted segment information */
		ELOAD_COMMAND_DYLD_INFO = 0x22,	/* compressed dyld information */
		ELOAD_COMMAND_DYLD_INFO_ONLY = (0x22|ELOAD_COMMAND_REQ_DYLD),	/* compressed dyld information only */
		ELOAD_COMMAND_VERSION_MIN_MACOSX = 0x24, // LC_VERSION_MIN_MACOSX
		ELOAD_COMMAND_VERSION_MIN_IPHONE_OS = 0x25, // LC_VERSION_MIN_IPHONEOS
		ELOAD_COMMAND_SOURCE_VERSION = 0x2A,
		
	};
}
#endif //__ELOAD_COMMANDS_H__
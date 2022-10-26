#ifndef __EFILE_TYPES_H__
#define __EFILE_TYPES_H__

namespace macho
{
	enum EFILE_TYPE
	{
		EFILE_TYPE_UNKNOWN = 0x0,
		EFILE_TYPE_OBJECT = 0x1,/* relocatable object file */
		EFILE_TYPE_EXECUTE = 0x2,/* demand paged executable file */
		EFILE_TYPE_FVMLIB = 0x3,/* fixed VM shared library file */
		EFILE_TYPE_CORE = 0x4,/* core file */
		EFILE_TYPE_PRELOAD = 0x5,/* preloaded executable file */
		EFILE_TYPE_DYLIB = 0x6,/* dynamically bound shared library */
		EFILE_TYPE_DYLINKER = 0x7,/* dynamic link editor */
		EFILE_TYPE_BUNDLE = 0x8,/* dynamically bound bundle file */
		EFILE_TYPE_DYLIB_STUB = 0x9,/* shared library stub for static */
						/*  linking only, no section contents */
		EFILE_TYPE_DSYM = 0xa,/* companion file with only debug */
						/*  sections */
		EFILE_TYPE_KEXT_BUNDLE = 0xb,/* x86_64 kexts */
	};
	enum EMACHO_FLAG
	{
	/* Constants for the flags field of the mach_header */
		EMACHO_FLAG_NOUNDEFS = 0x1,/* the object file has no undefined
						   references */
		EMACHO_FLAG_INCRLINK = 0x2,/* the object file is the output of an
						   incremental link against a base file
						   and can't be link edited again */
		EMACHO_FLAG_DYLDLINK = 0x4,/* the object file is input for the
						   dynamic linker and can't be staticly
						   link edited again */
		EMACHO_FLAG_BINDATLOAD = 0x8,/* the object file's undefined
						   references are bound by the dynamic
						   linker when loaded. */
		EMACHO_FLAG_PREBOUND = 0x10,/* the file has its dynamic undefined
						   references prebound. */
		EMACHO_FLAG_SPLIT_SEGS = 0x20,/* the file has its read-only and
						   read-write segments split */
		EMACHO_FLAG_LAZY_INIT = 0x40,/* the shared library init routine is
						   to be run lazily via catching memory
						   faults to its writeable segments
						   (obsolete) */
		EMACHO_FLAG_TWOLEVEL = 0x80,/* the image is using two-level name
						   space bindings */
		EMACHO_FLAG_FORCE_FLAT = 0x100,/* the executable is forcing all images
						   to use flat name space bindings */
		EMACHO_FLAG_NOMULTIDEFS = 0x200,/* this umbrella guarantees no multiple
						   defintions of symbols in its
						   sub-images so the two-level namespace
						   hints can always be used. */
		EMACHO_FLAG_NOFIXPREBINDING = 0x400,	/* do not have dyld notify the
						   prebinding agent about this
						   executable */
		EMACHO_FLAG_PREBINDABLE = 0x800,/* the binary is not prebound but can
						   have its prebinding redone. only used
											   when MH_PREBOUND is not set. */
		EMACHO_FLAG_ALLMODSBOUND = 0x1000,/* indicates that this binary binds to
											   all two-level namespace modules of
						   its dependent libraries. only used
						   when MH_PREBINDABLE and MH_TWOLEVEL
						   are both set. */ 
		EMACHO_FLAG_SUBSECTIONS_VIA_SYMBOLS = 0x2000,/* safe to divide up the sections into
							sub-sections via symbols for dead
							code stripping */
		EMACHO_FLAG_CANONICAL = 0x4000,/* the binary has been canonicalized
						   via the unprebind operation */
		EMACHO_FLAG_WEAK_DEFINES = 0x8000,/* the final linked image contains
						   external weak symbols */
		EMACHO_FLAG_BINDS_TO_WEAK = 0x10000	,/* the final linked image uses
						   weak symbols */

		EMACHO_FLAG_ALLOW_STACK_EXECUTION = 0x20000,/* When this bit is set, all stacks 
						   in the task will be given stack
						   execution privilege.  Only used in
						   MH_EXECUTE filetypes. */
		EMACHO_FLAG_ROOT_SAFE = 0x40000          ,/* When this bit is set, the binary 
						  declares it is safe for use in
						  processes with uid zero */
	                                         
		EMACHO_FLAG_SETUID_SAFE = 0x80000        ,/* When this bit is set, the binary 
						  declares it is safe for use in
						  processes when issetugid() is true */

		EMACHO_FLAG_NO_REEXPORTED_DYLIBS = 0x100000,/* When this bit is set on a dylib, 
						  the static linker does not need to
						  examine dependent dylibs to see
						  if any are re-exported */
		EMACHO_FLAG_PIE = 0x200000	,/* When this bit is set, the OS will
						   load the main executable at a
						   random address.  Only used in
						   MH_EXECUTE filetypes. */
		EMACHO_FLAG_DEAD_STRIPPABLE_DYLIB = 0x400000,/* Only for use on dylibs.  When
							 linking against a dylib that
							 has this bit set, the static linker
							 will automatically not create a
							 LC_LOAD_DYLIB load command to the
							 dylib if no symbols are being
							 referenced from the dylib. */
		EMACHO_FLAG_HAS_TLV_DESCRIPTORS = 0x800000,/* Contains a section of type 
							S_THREAD_LOCAL_VARIABLES */

		EMACHO_FLAG_NO_HEAP_EXECUTION = 0x1000000	/* When this bit is set, the OS will
						   run the main executable with
						   a non-executable heap even on
						   platforms (e.g. i386) that don't
						   require it. Only used in MH_EXECUTE
						   filetypes. */
	};
}

#endif //__EFILE_TYPES_H__
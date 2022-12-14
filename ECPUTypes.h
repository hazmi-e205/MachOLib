#ifndef __ECPU_TYPES_H__
#define __ECPU_TYPES_H__

namespace macho
{
	enum { ECPU_ABI64_FLAG = 0x01000000 };

	enum ECPU_TYPE
	{
		ECPU_TYPE_ANY = -1,
		ECPU_TYPE_VAX = 1,
		ECPU_TYPE_X86 = 7,
		ECPU_TYPE_MC98000 = 10,
		ECPU_TYPE_HPPA = 11,
		ECPU_TYPE_X86_64 = ( ECPU_ABI64_FLAG | ECPU_TYPE_X86),
		ECPU_TYPE_ARM = 12,
		ECPU_TYPE_ARM_X64 = (ECPU_ABI64_FLAG | ECPU_TYPE_ARM),
		ECPU_TYPE_MC88000 = 13,
		CPU_TYPE_SPARC = 14,
		CPU_TYPE_I860 = 15,
		CPU_TYPE_POWERPC = 18,
		CPU_TYPE_POWERPC64 = (CPU_TYPE_POWERPC | ECPU_ABI64_FLAG)
	};
}
#endif //__ECPU_TYPES_H__
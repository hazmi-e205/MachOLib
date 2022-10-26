#ifndef __EMAGIC_H__
#define __EMAGIC_H__

namespace macho
{
		//macho v2
		enum { kFAT_MAGIC = 0xCAFEBABEUL };
		enum { kFAT_MAGIC_SWAP = 0xBEBAFECA };
		//macho v1 x32
		enum { kMACHO_MAGIC = 0xFEEDFACE };
		enum { kMACHO_MAGIC_SWAP = 0xCEFAEDFE };
		//macho v1 x64
		enum { kMACHO_MAGIC_64 = 0xFEEDFACF };
		enum { kMACHO_MAGIC_SWAP_64 = 0xCFFAEDFE };
}

#endif //__EMAGIC_H__
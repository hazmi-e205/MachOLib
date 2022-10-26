#ifndef __CDYLIB_O_H__
#define __CDYLIB_O_H__

#include <types.h>
#include <string>

namespace macho
{
	struct CDyLib
	{
		std::string mPath;
		t_uint32 mTimestamp;
		t_uint32 mCurrentVersion;
		t_uint32 mCompatibilityVersion;
	};
}

#endif //__CDYLIB_O_H__
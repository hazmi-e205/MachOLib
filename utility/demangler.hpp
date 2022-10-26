#ifndef __MACHO_DEMAGLER_H__
#define __MACHO_DEMAGLER_H__

#include <string>
#include <libiberty/libiberty.h>
#include <libiberty/demangle.h>

inline std::string demangle(const char* symbol)
{
	std::string temp(symbol);
	char* result = cplus_demangle(temp.c_str(),DMGL_ANSI|DMGL_PARAMS|DMGL_TYPES);
	if(result)
	{
		temp = result;
		free(result);
	}
	else
	{
		if(temp.size() > 1) {
			char* result = cplus_demangle(temp.c_str()+1,DMGL_ANSI|DMGL_PARAMS|DMGL_TYPES);
			if(result)
			{
				temp = result;
				free(result);
			}
		}
	}
	return temp;
}

#endif //__MACHO_DEMAGLER_H__

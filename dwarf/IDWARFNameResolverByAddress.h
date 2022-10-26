#ifndef __IDWARF_NAME_RESOLVER_BY_ADDRESS_H__
#define __IDWARF_NAME_RESOLVER_BY_ADDRESS_H__

#include <string>

namespace dwarf
{
	class IDWARFNameResolverByAddress
	{
	protected:
		~IDWARFNameResolverByAddress(){}
	public:
		virtual std::string resolve(unsigned long long int address) = 0;
	};
}
#endif //__IDWARF_NAME_RESOLVER_BY_ADDRESS_H__
#ifndef __ECPU_SUB_TYPES_H__
#define __ECPU_SUB_TYPES_H__

namespace macho
{
	enum ECPU_SUB_TYPE
	{
		/*if cpu type ARM*/
		ECPU_SUB_TYPE_ALL		= 0,
		ECPU_SUB_TYPE_ARM_V4T	= 5,
		ECPU_SUB_TYPE_ARM_V5	= 7,
		ECPU_SUB_TYPE_ARM_V6	= 6,
		ECPU_SUB_TYPE_ARM_V7	= 9,
		ECPU_SUB_TYPE_ARM_V7F	= 10,
		ECPU_SUB_TYPE_ARM_V7S	= 11,
		ECPU_SUB_TYPE_ARM_V7K	= 12,
		ECPU_SUB_TYPE_ARM_V6M	= 14,
		ECPU_SUB_TYPE_ARM_V7M	= 15,
		ECPU_SUB_TYPE_ARM_V7EM	= 16
	};
}
#endif //__ECPU_SUB_TYPES_H__
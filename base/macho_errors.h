#ifndef __MACHO_ERRORS_H__
#define __MACHO_ERRORS_H__

namespace base
{
	enum ERESULT_CODE
	{
		ERESULT_CODE_SUCCESS = 0,
		ERESULT_CODE_SUCCESS_ALREADY_LOADED,
		ERESULT_CODE_BAD_MACHO,
		ERESULT_CODE_FAIL
	};

	inline bool is_successful(ERESULT_CODE code)
	{
		return (code == ERESULT_CODE_SUCCESS
				|| code == ERESULT_CODE_SUCCESS_ALREADY_LOADED);
	}
	inline bool is_successful_only(ERESULT_CODE code)
	{
		return (code == ERESULT_CODE_SUCCESS);
	}
};

#endif //__MACHO_TYPES_H__

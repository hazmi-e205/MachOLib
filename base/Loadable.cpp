#include "Loadable.h"
#include "macho_errors.h"

namespace base
{
	/*
	* NVI
	*/
	Loadable::Loadable()
		:m_loading_state(ELOADING_STATE_INCOMPLETE)
	{
		return;
	}
	Loadable::~Loadable()
	{
		m_loading_state = ELOADING_STATE_INCOMPLETE;
		return;
	}
	ERESULT_CODE Loadable::load(ERESERVED_FLAGS flags)
	{
		ERESULT_CODE ret_code = _load(flags);
		m_loading_state = is_successful_only(ret_code)?ELOADING_STATE_COMPLETE:ELOADING_STATE_INCOMPLETE;
		return ret_code;
	}
	ERESULT_CODE Loadable::load_if_notloaded(ERESERVED_FLAGS flags)
	{
		return _load_if_notloaded(flags);
	}
	ERESULT_CODE Loadable::validate(ERESERVED_FLAGS flags) const
	{
		return _validate(flags);
	}
	/*
	* VI
	*/
	ERESULT_CODE Loadable::_load(ERESERVED_FLAGS flags)
	{
		return ERESULT_CODE_FAIL;
	}
	ERESULT_CODE Loadable::_load_if_notloaded(ERESERVED_FLAGS flags)
	{
		ERESULT_CODE ret_code = ERESULT_CODE_SUCCESS_ALREADY_LOADED;
		if(m_loading_state == ELOADING_STATE_INCOMPLETE) {
			ret_code = _load(flags);
			//assert(is_successful_only(ret_code));
			m_loading_state = is_successful_only(ret_code)?ELOADING_STATE_COMPLETE:ELOADING_STATE_INCOMPLETE;
		}
		return ret_code;
	}
	ERESULT_CODE Loadable::_validate(ERESERVED_FLAGS flags) const
	{
		return ERESULT_CODE_FAIL;
	}
}
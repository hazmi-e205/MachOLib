#ifndef __LOADABLE_H__
#define __LOADABLE_H__

#include <types.h>
#include <memory>
#include "macho_errors.h"

namespace base
{
	class Loadable
	{
	public:
		enum ERESERVED_FLAGS { ERESERVED_FLAGS_DEFAULT = 0 };
		enum ELOADING_STATE { ELOADING_STATE_INCOMPLETE = 0, ELOADING_STATE_COMPLETE };
	public:
		ERESULT_CODE load(ERESERVED_FLAGS flags = ERESERVED_FLAGS_DEFAULT);
		ERESULT_CODE load_if_notloaded(ERESERVED_FLAGS flags = ERESERVED_FLAGS_DEFAULT);
		ERESULT_CODE validate(ERESERVED_FLAGS flags = ERESERVED_FLAGS_DEFAULT) const;
	protected:
		Loadable();
		virtual ~Loadable();
		/*********Should be overloaded*********/
		virtual ERESULT_CODE _load(ERESERVED_FLAGS flags) = 0;
		virtual ERESULT_CODE _validate(ERESERVED_FLAGS flags) const = 0;
		/*********Default Implementation*********/
		virtual ERESULT_CODE _load_if_notloaded(ERESERVED_FLAGS flags);
	private:
		ELOADING_STATE m_loading_state;
	};
}

#endif //__LOADABLE_H__

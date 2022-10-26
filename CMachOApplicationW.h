#pragma once
#ifndef __CMACHO_APP_LOG_H__
#define __CMACHO_APP_LOG_H__
#include <utility/CSearchMachO.h>
#include "CFATW.h"

namespace MachO
{
    class CMachOApplicationW : public CFATW
	{
	private:
		CMachOApplication* m_app;
	public:
		virtual macho::CFAT& fat() override
		{
			return *m_app->fat();
		}
		virtual macho::CFAT::Ptr pfat() override
		{
			return m_app->fat();
		}
	public: 
		CMachOApplicationW(const CMachOApplication& app)
		{
			m_app = new CMachOApplication(app);
		}
		~CMachOApplicationW()
		{
			delete m_app;
			m_app = NULL;
		}
        /*!CMachOApplicationW()
		{
			delete m_app;
			m_app = NULL;
        }*/
        std::wstring GetPath()
        {
            return std::wstring(m_app->path().begin(), m_app->path().end());
        }
        std::wstring GetName()
        {
            return std::wstring(m_app->name().begin(), m_app->name().end());
        }
	};
}

#endif //__CMACHO_APP_LOG_H__

#pragma once
#ifndef __CMACHO_DSYM_LOG_H__
#define __CMACHO_DSYM_LOG_H__
#include <utility/CSearchMachO.h>
#include "CFATW.h"

namespace MachO
{
    class CMachODSymW : public CFATW
	{
	private:
		CMachODSym* m_dsym;
	public:
        macho::CFAT& fat() override
		{
			return *m_dsym->fat();
		}
        macho::CFAT::Ptr pfat() override
		{
			return m_dsym->fat();
		}
	public: 
		CMachODSymW(const CMachODSym& app)
		{
			m_dsym = new CMachODSym(app);
		}
		~CMachODSymW()
		{
			delete m_dsym;
			m_dsym = NULL;
		}
        /*!CMachODSymW()
		{
			delete m_dsym;
			m_dsym = NULL;
        }*/
        std::wstring GetPath()
        {
            return std::wstring(m_dsym->path().begin(), m_dsym->path().end());
        }
        std::wstring GetName()
        {
            return std::wstring(m_dsym->name().begin(), m_dsym->name().end());
        }
	};
}

#endif //__CMACHO_DSYM_LOG_H__

#pragma once
#ifndef __CMACHO_DYLIB_LOG_H__
#define __CMACHO_DYLIB_LOG_H__
#include <utility/CSearchMachO.h>
#include "CFATW.h"

namespace MachO
{
    class CMachODyLibW : public CFATW
	{
	private:
		CMachODyLib* m_dylib;
	public:
		virtual macho::CFAT& fat() override
		{
			macho::CFAT::Ptr oret = m_dylib->fat();
			return *oret;
		}
		virtual macho::CFAT::Ptr pfat() override
		{
			return m_dylib->fat();
		}
		virtual base::ERESULT_CODE get_uuids(std::vector<std::string>& uuid_list) override
		{
			std::vector<std::wstring>& uids = m_dylib->uuids();
			for(size_t i = 0; i < uids.size(); ++i)
			{
				uuid_list.push_back(std::string(uids[i].begin(), uids[i].end()));
			}
			return base::ERESULT_CODE_SUCCESS;
		}
	public: 
		CMachODyLibW(const CMachODyLib& app)
		{
			m_dylib = new CMachODyLib(app);
		}
		~CMachODyLibW()
		{
			delete m_dylib;
			m_dylib = NULL;
		}
        /*!CMachODyLibW()
		{
			delete m_dylib;
			m_dylib = NULL;
        }*/
        std::wstring GetPath()
		{
            return std::wstring(m_dylib->path().begin(), m_dylib->path().end());
		}
        std::wstring GetName()
		{
            return std::wstring(m_dylib->name().begin(), m_dylib->name().end());
		}
	};
}

#endif //__CMACHO_DYLIB_LOG_H__

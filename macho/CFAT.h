#ifndef __CFAT_H__
#define __CFAT_H__

#include <map>
#include <memory>
#include <base/base.h>
#include <types.h>
#include "CMachO.h"
#include "ECPUTypes.h"
#include "CFATArch.h"
#include <io/stream_file.h>
#include <io/source_of_stream.h>

namespace macho
{
	class CFAT : public base::Loadable
	{
	public:
		enum { kFAT_MAGIC = 0xCAFEBABEUL };
		enum { kFAT_MAGIC_SWAP = 0xBEBAFECA };
		typedef std::shared_ptr<CFAT> ptr;
		typedef ptr Ptr;
		typedef std::vector<CMachO::Ptr> CMachOCollection;

		struct FindMachOWithUUID
		{
			FindMachOWithUUID(const std::string& uuid)
				:mUuid(uuid)
			{
			}
			bool operator () (const CMachO::Ptr& macho)
			{
				return (macho->hasUUID() && macho->getUUID() == mUuid);
			}
			std::string mUuid;
		};
	private:
		/*old*/
		CMachOCollection		m_machos;
		io::source_of_stream	m_source;
	protected:
		/*************base::Loadable**************/
		virtual base::ERESULT_CODE _validate(base::Loadable::ERESERVED_FLAGS flags) const;
		virtual base::ERESULT_CODE _load(base::Loadable::ERESERVED_FLAGS flags);
		/*************base::Loadable**************/
	public:
		CFAT(const io::source_of_stream& source);
		~CFAT();
		unsigned int		count();
		bool				isExistMachO(const std::string& uuid);
		bool				isExistMachO(const std::wstring& uuid);
		base::ERESULT_CODE	getAllUUIDs(std::vector<std::wstring>& uuid_list);
		base::ERESULT_CODE	getAllUUIDs(std::vector<std::string>& uuid_list);

		/*******************NEW******************/
		CMachO::Ptr			getMachO(ECPU_TYPE type,ECPU_SUB_TYPE sub_type);
		CMachO::Ptr			getMachO(const std::string& uuid);
		CMachO::Ptr			getMachO(const std::wstring& uuid);
		CMachO::Ptr			getMachOAt(unsigned int idx);
		/*******************NEW******************/

		/*******************DEPRECATED******************/
		CMachO&			getMachODeprecated(ECPU_TYPE type,ECPU_SUB_TYPE sub_type);
		CMachO&			getMachODeprecated(const std::string& uuid);
		CMachO&			getMachODeprecated(const std::wstring& uuid);
		CMachO&			getMachOAtDeprecated(unsigned int idx);
		/*******************DEPRECATED******************/
	};
}

#endif //__CFAT_H__

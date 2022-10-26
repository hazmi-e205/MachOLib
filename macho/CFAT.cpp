#include "CFAT.h"
#include <assert.h>
#include <utility/CConsole.h>

namespace macho
{
	CFAT::CFAT(const io::source_of_stream& source)
		:m_machos(),
		m_source(source)
	{
		return;
	}
	CFAT::~CFAT()
	{
		return;
	}
	base::ERESULT_CODE CFAT::_load(base::Loadable::ERESERVED_FLAGS flags)
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		io::stream_base::ptr stream = m_source.open();
		if(stream.get())
		{
			macho::t_uint32 magic;
			macho::t_uint32 count_of_archs;
			stream->le_read(magic);
			switch(magic)
			{
			case kFAT_MAGIC:
			case kFAT_MAGIC_SWAP:
				{
					m_machos.clear();
					stream->enable_big_endian(magic == kFAT_MAGIC_SWAP);
					m_source.set_encoding(stream->encoding());
					stream->t_read(count_of_archs);
					for(macho::t_uint32 i = 0; i < count_of_archs; ++i)
					{
						CFATArch arch;
						if((ret_code = arch.deserialize(stream)) != base::ERESULT_CODE_SUCCESS)
						{
							break;//from loop
						}
						else
						{
							CMachO::Ptr macho(new CMachO(m_source,arch.getOffset()));
							m_machos.push_back(macho);
						}
					}
					break;
				}
			default:
				{
					m_machos.clear();
					stream->seek(stream->position() - sizeof(magic));
					CMachO::ptr macho(new CMachO(m_source,0));
					if(macho->hasUUID() && !macho->getUUID().empty())
					{
						ret_code = base::ERESULT_CODE_SUCCESS;
						m_machos.push_back(macho);
					}
					break;
				}
			}
		}
		else
		{
			TRACE("[CFAT] call preload error stream is not opened!!!");
		}
		return ret_code;
	}
	CMachO::Ptr	CFAT::getMachO(ECPU_TYPE type,ECPU_SUB_TYPE sub_type)
	{
		load_if_notloaded();
		for(CMachOCollection::iterator i = m_machos.begin(), e = m_machos.end();
			i != e;
			++i)
		{
			if((*i)->getTypeOfCPU() == type && (*i)->getSubTypeOfCPU() == sub_type)
			{
				return (*i);
			}
		}
		assert(false);
		return CMachO::Ptr();
	}
	CMachO::Ptr	CFAT::getMachO(const std::string& uuid)
	{
		CMachO::Ptr ret;
		load_if_notloaded();
		std::string temp(uuid);
		std::transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
		CMachOCollection::const_iterator i = std::find_if(m_machos.begin(), m_machos.end(), FindMachOWithUUID(uuid));
		
		if(i != m_machos.end())
		{
			ret = (*i);
		}
		return ret;
	}
	CMachO::Ptr	CFAT::getMachO(const std::wstring& uuid)
	{
		return getMachO(std::string(uuid.begin(),uuid.end()));
	}
	CMachO::Ptr	CFAT::getMachOAt(unsigned int idx)
	{
		CMachO::Ptr ret;
		load_if_notloaded();
		if(idx < m_machos.size())
		{
			ret = m_machos.at(idx);
		}
		return ret;
	}
	CMachO& CFAT::getMachODeprecated(ECPU_TYPE type,ECPU_SUB_TYPE sub_type)
	{
		CMachO::Ptr temp = getMachO(type,sub_type);
		assert(temp.get() != NULL);
		return temp.get()?(*temp.get()):(*(new CMachO()));//!!!! leak need before used isExistMachO
	}
	bool CFAT::isExistMachO(const std::string& uuid)
	{
		load_if_notloaded();
		return (std::find_if(m_machos.begin(), m_machos.end(), FindMachOWithUUID(uuid)) != m_machos.end());
	}
	bool CFAT::isExistMachO(const std::wstring& uuid)
	{
		return isExistMachO(std::string(uuid.begin(),uuid.end()));
	}
	CMachO& CFAT::getMachODeprecated(const std::string& uuid)
	{
		CMachO::Ptr temp = getMachO(uuid);
		assert(temp.get() != NULL);
		return temp.get()?(*temp.get()):(*(new CMachO()));//!!!! leak need before used isExistMachO
	}
	CMachO& CFAT::getMachOAtDeprecated(unsigned int idx)
	{
		CMachO::Ptr temp = getMachOAt(idx);
		assert(temp.get() != NULL);
		return temp.get()?(*temp.get()):(*(new CMachO()));//!!!! leak need before used isExistMachO
	}
	CMachO& CFAT::getMachODeprecated(const std::wstring& uuid)
	{
		return getMachODeprecated(std::string(uuid.begin(),uuid.end()));
	}
	unsigned int CFAT::count()
	{
		load_if_notloaded();
		return m_machos.size();
	}
	base::ERESULT_CODE CFAT::getAllUUIDs(std::vector<std::wstring>& uuid_list)
	{
		std::vector<std::string> transit;
		base::ERESULT_CODE ret_code = getAllUUIDs(transit);
		if(ret_code == base::ERESULT_CODE_SUCCESS)
		{
			for(std::vector<std::string>::iterator i = transit.begin(), e = transit.end();
				i != e;
				++i)
			{
				uuid_list.push_back(std::wstring((*i).begin(),(*i).end()));
			}
		}
		return ret_code;
	}
	base::ERESULT_CODE CFAT::getAllUUIDs(std::vector<std::string>& uuid_list)
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		if(base::is_successful(ret_code = load_if_notloaded()))
		{
			for(CMachOCollection::iterator i = m_machos.begin(), e = m_machos.end();
				i != e;
				++i)
			{
				if((*i)->hasUUID() && !(*i)->getUUID().empty())
				{
					uuid_list.push_back((*i)->getUUID());
					ret_code = base::ERESULT_CODE_SUCCESS;
				}
				else
				{
					ret_code = base::ERESULT_CODE_FAIL;
					break;
				}
			}
		}
		return ret_code;
	}
	base::ERESULT_CODE CFAT::_validate(base::Loadable::ERESERVED_FLAGS flags) const
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		io::stream_base::ptr stream = m_source.open();
		if(stream.get())
		{
			macho::t_uint32 magic;
			macho::t_uint32 count_of_archs;
			stream->le_read(magic);
			switch(magic)
			{
			case kFAT_MAGIC:
			case kFAT_MAGIC_SWAP:
				{
					stream->enable_big_endian(magic == kFAT_MAGIC_SWAP);
					stream->t_read(count_of_archs);
					for(macho::t_uint32 i = 0; i < count_of_archs; ++i)
					{
						CFATArch arch;
						if((ret_code = arch.deserialize(stream)) != base::ERESULT_CODE_SUCCESS)
						{
							break;//from loop
						}
						else
						{
							CMachO macho(m_source,arch.getOffset());
							if((ret_code = macho.validate()) != base::ERESULT_CODE_SUCCESS)
							{
								break;
							}
						}
					}
					break;
				}
			default:
				{
					stream->seek(stream->position() - sizeof(magic));
					CMachO macho(m_source,0);
					ret_code = macho.validate();
					break;
				}
			}
		}
		else
		{
			TRACE("[CFAT] call preload error stream is not opened!!!");
		}
		return ret_code;
	}
}
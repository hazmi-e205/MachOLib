#include "CDWARFAbbreviationTable.h"
#include <utility/CConsole.h>

namespace dwarf
{
	/*
	* CDWARFAbbreviationTable::CDWARFAttribute
	*/
	/*
	* CDWARFAbbreviationTable::CDWARFAbbreviationDeclaration
	*/
	CDWARFAbbreviationTable::CDWARFAbbreviationDeclaration::CDWARFAbbreviationDeclaration()
		:m_code(0),
		m_tag(DW_TAG_invalid),
		m_hasChildren(0),
		m_attributes()
	{
		return;
	}
	bool CDWARFAbbreviationTable::CDWARFAbbreviationDeclaration::parse(io::stream_base::ptr stream)
	{
		bool ret_code = false;
		t_uint64 temp;
		if(stream->read_uleb128(temp) > 0ULL)
		{
			if ((m_code = static_cast<t_uint32>(temp)) != 0) 
			{
				if(stream->read_uleb128(temp) > 0ULL)
				{
					m_tag = static_cast<DW_TAG>(temp);
					if(stream->t_read(m_hasChildren) > 0ULL)
					{
						while(stream->position() < stream->size())
						{
							stream->read_uleb128(temp);
							t_uint16 attr = static_cast<t_uint16>(temp);
							stream->read_uleb128(temp);
							t_uint16 form = static_cast<t_uint16>(temp);
							if (attr && form)
							{
								m_attributes.push_back(CDWARFAttribute(attr, form));
							}
							else
							{
								break;
							}
						}
					}
					ret_code = (m_tag != 0);
				}
			} 
			else 
			{
				m_tag = DW_TAG_invalid;
				m_hasChildren = false;
			}
		}
		return ret_code;
	}
	/*
	* CDWARFAbbreviationTable
	*/
	CDWARFAbbreviationTable::CDWARFAbbreviationTable()
		:m_map()
	{
		return;
	}
	bool CDWARFAbbreviationTable::parse(io::stream_base::ptr stream)
	{
		bool ret_code = true;
		while(stream->position() < stream->size())
		{
			CDWARFAbbreviationDeclaration abbr;
			if((ret_code = abbr.parse(stream)) == true)
			{
				m_map.insert(std::make_pair(abbr.getCode(),abbr));
			}
			else
			{
				break;
			}
		}
		return ret_code;
	}
	const CDWARFAbbreviationTable::CDWARFAbbreviationDeclaration* CDWARFAbbreviationTable::getDeclaration(t_uint32 code) const
	{
		DeclarationMap::const_iterator i = m_map.find(code);
		if(i != m_map.end())
		{
			return &(*i).second;
		}
		TRACE("[CDWARFAbbreviationTable] error get code = %u, map size = %u", code, m_map.size());
		//assert(false);
		return NULL;
	}
}
#ifndef __DWARF_ABBREVIATION_DECLARATION_TABLE_H__
#define __DWARF_ABBREVIATION_DECLARATION_TABLE_H__

#include <map>
#include <vector>
#include <string>
#include <limits>
#include <io/stream_base.h>
#include <io/source_of_stream.h>
#include <memory>
#include <io/stream_memory.h>
#include "DWARFTypes.h"
#include "EDWARF.h"

namespace dwarf
{
	class CDWARFAbbreviationTable
	{
	public:
		typedef std::shared_ptr<CDWARFAbbreviationTable> Ptr;

		class CDWARFAttribute 
		{
		private:
			DW_AT m_attribute;
			DW_FORM m_form;
		public:
			CDWARFAttribute(t_uint16 attr, t_uint16 form)
				:m_attribute(static_cast<DW_AT>(attr)), 
				m_form(static_cast<DW_FORM>(form)) 
			{
				return;
			}
			DW_AT getAttribute() const { 
				return m_attribute; 
			}
			DW_FORM getForm() const { 
				return m_form; 
			}
		};
		class CDWARFAbbreviationDeclaration
		{
		private:
			t_uint32 m_code;
			DW_TAG m_tag;
			t_uint8 m_hasChildren;
			std::vector<CDWARFAttribute> m_attributes;
		public:
			CDWARFAbbreviationDeclaration();
			bool parse(io::stream_base::ptr stream);

			t_uint32 getCode() const {
				return m_code;
			}
			DW_TAG getTag() const {
				return m_tag;
			}
			bool hasChildren() const {
				return (m_hasChildren == DW_CHILDREN_yes);
			}
			t_uint32 getAttributesCount() const {
				return m_attributes.size();
			}
			const CDWARFAttribute& getAttributeAt(t_uint32 idx) const {
				return m_attributes.at(idx);
			}
			bool operator < (const CDWARFAbbreviationDeclaration& other) const
			{
				return (m_code < other.m_code);
			}

		};
		struct SearchDeclarationWithCode
		{
			inline bool operator()(t_uint32 t2,const CDWARFAbbreviationDeclaration& t1) const  { 
				return (t2 < t1.getCode()); 
			}
			inline bool operator()(const CDWARFAbbreviationDeclaration& t1,t_uint32 t2) const  { 
				return (t1.getCode() < t2); 
			} 
			inline bool operator()(const CDWARFAbbreviationDeclaration& t1,const CDWARFAbbreviationDeclaration& t2) const  { 
				return (t1.getCode() < t2.getCode()); 
			} 
		};
	private:
		typedef std::map<t_uint32,CDWARFAbbreviationDeclaration> DeclarationMap;
		DeclarationMap m_map;
	public:
		CDWARFAbbreviationTable();
		bool parse(io::stream_base::ptr stream);
		const CDWARFAbbreviationDeclaration * getDeclaration(t_uint32 code) const;
	};
}

#endif //__DWARF_ABBREVIATION_DECLARATION_TABLE_H__
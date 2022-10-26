#include "CDWARFDebugInfoEntry.h"
#include "CDWARFAbbreviationTable.h"
#include "CDWARFCompileUnit.h"
#include "CDWARFRangeTable.h"
#include "CDWARFFormValue.h"
#include "utility/CDWARFRangeResolver.hpp"

namespace dwarf
{
	CDWARFDebugInfoEntry::CDWARFDebugInfoEntry()
		:m_tag(DW_TAG_invalid),
		m_values(),
		m_abbr_idx(0),
		m_has_child(false)
	{
		return;
	}
	bool CDWARFDebugInfoEntry::parse_all(const CDWARFAbbreviationTable& abbr_table,const CDWARFCompileUnit& unit,io::stream_base::ptr debug_str,io::stream_base::ptr stream)
	{
		bool ret_code = false;
		t_uint64 abbr_idx;
		ret_code = (stream->read_uleb128(abbr_idx) > 0ULL);
		if(ret_code && (m_abbr_idx = abbr_idx) > 0)
		{
			const CDWARFAbbreviationTable::CDWARFAbbreviationDeclaration* decl = abbr_table.getDeclaration(static_cast<t_uint32>(abbr_idx));
			if(decl)
			{
				m_values.clear();
				DW_TAG tag = m_tag = decl->getTag();
				bool hasCildren = m_has_child = decl->hasChildren();
				bool isCompileUnitTag = (tag == DW_TAG_compile_unit);
				t_uint32 count = decl->getAttributesCount();

				for(t_uint32 i = 0; i < count && ret_code; ++i)
				{
					t_int32 size = -1;
					const CDWARFAbbreviationTable::CDWARFAttribute& attr = decl->getAttributeAt(i);
					m_values.push_back(std::make_pair(attr.getAttribute(), CDWARFFormValue(attr.getForm())));
					ret_code = m_values.back().second.parse(unit.getAddrSize(),unit.getDwarfFromat(), unit.getVersion(),debug_str,stream);
				}
			}
		}
		else
		{
			ret_code = false;
		}
		return ret_code;
	}
	bool CDWARFDebugInfoEntry::parse(const CDWARFAbbreviationTable& abbr_table,const CDWARFCompileUnit& unit,io::stream_base::ptr debug_str,io::stream_base::ptr stream)
	{
		bool ret_code = false;
		t_uint64 abbr_idx;
		ret_code = (stream->read_uleb128(abbr_idx) > 0ULL);
		if(ret_code && (m_abbr_idx = abbr_idx) > 0)
		{
			const CDWARFAbbreviationTable::CDWARFAbbreviationDeclaration* decl = abbr_table.getDeclaration(static_cast<t_uint32>(abbr_idx));
			if(decl)
			{
				m_values.clear();
				DW_TAG tag = m_tag = decl->getTag();
				bool hasCildren = m_has_child = decl->hasChildren();
				bool isCompileUnitTag = (tag == DW_TAG_compile_unit);
				t_uint32 count = decl->getAttributesCount();
				for(t_uint32 i = 0; i < count && ret_code; ++i)
				{
					const CDWARFAbbreviationTable::CDWARFAttribute& attr = decl->getAttributeAt(i);
					CDWARFFormValue::Skip(attr.getForm(),unit.getAddrSize(),unit.getDwarfFromat(), unit.getVersion(), stream);
				}
			}
		}
		else
		{
			ret_code = false;
		}
		return ret_code;
	}
	bool CDWARFDebugInfoEntry::parse_if(const CDWARFAbbreviationTable& abbr_table,const CDWARFCompileUnit& unit,io::stream_base::ptr debug_str,io::stream_base::ptr stream,  is_parse delegate_)
	{
		bool ret_code = false;
		t_uint64 abbr_idx;
		ret_code = (stream->read_uleb128(abbr_idx) > 0ULL);
		if(ret_code && (m_abbr_idx = abbr_idx) > 0)
		{
			const CDWARFAbbreviationTable::CDWARFAbbreviationDeclaration* decl = abbr_table.getDeclaration(static_cast<t_uint32>(abbr_idx));
			if(decl)
			{
				m_values.clear();
				DW_TAG tag = m_tag = decl->getTag();
				bool hasCildren = m_has_child = decl->hasChildren();
				bool isCompileUnitTag = (tag == DW_TAG_compile_unit);
				t_uint32 count = decl->getAttributesCount();
				if(delegate_(tag))
				{
					for(t_uint32 i = 0; i < count && ret_code; ++i)
					{
						t_int32 size = -1;
						const CDWARFAbbreviationTable::CDWARFAttribute& attr = decl->getAttributeAt(i);
						m_values.push_back(std::make_pair(attr.getAttribute(), CDWARFFormValue(attr.getForm())));
						ret_code = m_values.back().second.parse(unit.getAddrSize(),unit.getDwarfFromat(), unit.getVersion(),debug_str,stream);
						assert(ret_code);
					}
				}
				else
				{
					for(t_uint32 i = 0; i < count && ret_code; ++i)
					{
						const CDWARFAbbreviationTable::CDWARFAttribute& attr = decl->getAttributeAt(i);
						ret_code = CDWARFFormValue::Skip(attr.getForm(),unit.getAddrSize(),unit.getDwarfFromat(), unit.getVersion(),stream);
						assert(ret_code);
					}
				}
			}
		}
		else
		{
			ret_code = false;
		}
		return ret_code;
	}
	bool CDWARFDebugInfoEntry::isNULL() const
	{
		return (m_abbr_idx == 0);
	}
	bool CDWARFDebugInfoEntry::isEmpty() const
	{
		return m_values.empty();
	}
	bool CDWARFDebugInfoEntry::isCompileUnit() const
	{
		return (m_tag == DW_TAG_compile_unit);
	}
	const CDWARFFormValue* CDWARFDebugInfoEntry::getFormBy(DW_AT attr) const
	{
		const CDWARFFormValue* ret = NULL;
		for(FormsTable::const_iterator i = m_values.begin(), e = m_values.end();
			i != e;
			++i)
		{
			if((*i).first == attr)
			{
				ret = &(*i).second;
				break;
			}
		}
		return ret;
	}
	bool CDWARFDebugInfoEntry::isInRange(t_uint64 addr, t_uint64 base_address_of_cu, t_uint16 dwarf_version, const utility::CDWARFRangeResolver& range_resolver) const
	{
		 
		t_uint64 hipc = getHighPC(dwarf_version);
		t_uint64 lopc = getLowPC();
		bool ret = false;
		if(hipc == kINVALID_ADDRESS || lopc == kINVALID_ADDRESS)
		{
			
			const CDWARFFormValue* attr_ranges = getFormBy(DW_AT_ranges);
			if(attr_ranges)
			{
				t_uint64 off = getRange();
				const CDWARFRangeTable::RangeSet* set = range_resolver(off);
				if(set)
				{
					ret = set->containsAddress(base_address_of_cu, addr);
				}
			}
		}
		else
		{
			ret = (addr >= lopc && addr < hipc);
		}
		return ret;
	}
	t_uint64 CDWARFDebugInfoEntry::getLowPC(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_low_pc);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getHighPC(t_uint16 dwarf_version, t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_high_pc);
		if (!form)
		{
			return default_;
		}
		//According to section "2.17.2 Contiguous Address Range" of the DWARF 4 standrard
		// If the value of the DW_AT_high_pc is of class address, it is an address, 
		// but if it is of class constant it is an offset from the DW_AT_low_pc
		if (dwarf_version >= 4)
		{
			switch(form->formClass())
			{
			case CDWARFFormValue::eFormClassConstant:
				{
					t_uint64 low = getLowPC();
					if (low == kINVALID_ADDRESS)
					{
						return default_;
					}
					return low + form->asUInt64();
				}
			case CDWARFFormValue::eFormClassAddress:
				{
					return form->asUInt64();
				}
			default:
				{
					assert(false && "Unsupported form class");
					return default_;
				}
			}
			
		}
		return form->asUInt64();
	}
	t_uint64 CDWARFDebugInfoEntry::getEnterPC(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_entry_pc);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getCallLine(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_call_line);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getCallFile(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_call_file);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getDeclLine(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_decl_line);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getDeclFile(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_decl_file);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getObjectPointer(t_uint64 default_) const
	{
		const CDWARFFormValue* obj = getFormBy(DW_AT_object_pointer);
		return obj?(obj->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getSpecification(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_specification);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getAbstractOrigin(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_abstract_origin);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getStmtList(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_stmt_list);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getType(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_type);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getRange(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_ranges);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getNamePtr(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_name);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getMIPSLinkageNamePtr(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_MIPS_linkage_name);
		return form?(form->asUInt64()):default_;
	}
	t_uint64 CDWARFDebugInfoEntry::getEncoding(t_uint64 default_) const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_encoding);
		return form?(form->asUInt64()):default_;
	}
	DW_TAG CDWARFDebugInfoEntry::getTag() const
	{
		return m_tag;
	}
	bool CDWARFDebugInfoEntry::hasChild() const
	{
		return m_has_child;
	}
	bool CDWARFDebugInfoEntry::isArtificial() const
	{
		const CDWARFFormValue* form = getFormBy(DW_AT_artificial);
		return form?(form->asUInt64() == 1):(false);
		
	}
	std::string CDWARFDebugInfoEntry::getName(io::stream_base::ptr debug_str) const
	{
		std::string name;
		const CDWARFFormValue* form = getFormBy(DW_AT_name);
		if(form)
		{
			name = form->asString(debug_str);
		}
		return name;
	}
	std::string CDWARFDebugInfoEntry::getMIPSLinkageName(io::stream_base::ptr debug_str) const
	{
		std::string name;
		const CDWARFFormValue* form = getFormBy(DW_AT_MIPS_linkage_name);
		if(form)
		{
			name = form->asString(debug_str);
		}
		return name;
	}
	std::string CDWARFDebugInfoEntry::getCompileDirectory(io::stream_base::ptr debug_str) const
	{
		std::string name;
		const CDWARFFormValue* form = getFormBy(DW_AT_comp_dir);
		if(form)
		{
			name = form->asString(debug_str);
		}
		return name;
	}
}
#ifndef __CDWARF_DEBUG_INFO_ENTRY_H__
#define __CDWARF_DEBUG_INFO_ENTRY_H__

#include <map>
#include <vector>
#include <string>
#include <limits>
#include <io/stream_base.h>
#include <io/source_of_stream.h>
#include <memory>
#include <io/stream_memory.h>
#include "EDWARF.h"
#include "DWARFTypes.h"
#include "CDWARFFormValue.h"

namespace dwarf
{
	namespace utility
	{
		class CDWARFRangeResolver;
	}
	class CDWARFRangeTable;
	class CDWARFCompileUnit;
	class CDWARFAbbreviationTable;
	class CDWARFDebugInfoEntry
	{
	public:
		typedef std::vector<std::pair<DW_AT,CDWARFFormValue> > FormsTable;
		typedef std::vector<DW_TAG> ToExtractList;
		typedef bool (*is_parse)(DW_TAG tag);
	public:
		static const t_uint64 kINVALID_ADDRESS = 0xFFFFFFFFFFFFFFFFULL;
	public:
		CDWARFDebugInfoEntry();
		/*full parse*/
		bool		parse_all(const CDWARFAbbreviationTable& abbr_table,const CDWARFCompileUnit& unit,io::stream_base::ptr debug_str,io::stream_base::ptr stream);
		/*parse only tag and skip other*/
		bool		parse(const CDWARFAbbreviationTable& abbr_table,const CDWARFCompileUnit& unit,io::stream_base::ptr debug_str,io::stream_base::ptr stream);
		bool		parse_if(const CDWARFAbbreviationTable& abbr_table,const CDWARFCompileUnit& unit,io::stream_base::ptr debug_str,io::stream_base::ptr stream, is_parse delegate_);
		
		bool		isNULL() const;
		bool		isEmpty() const;
		bool		isCompileUnit() const;
		const CDWARFFormValue* getFormBy(DW_AT attr) const;
		bool		isInRange(t_uint64 addr, t_uint64 base_address_of_cu, t_uint16 dwarf_version, const utility::CDWARFRangeResolver& range_resolver) const;
		t_uint64	getObjectPointer(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getSpecification(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getAbstractOrigin(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getLowPC(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getHighPC(t_uint16 dwarf_version, t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getEnterPC(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getCallLine(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getCallFile(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getDeclLine(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getDeclFile(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getStmtList(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getType(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getRange(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getNamePtr(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getMIPSLinkageNamePtr(t_uint64 default_ = kINVALID_ADDRESS) const;
		t_uint64	getEncoding(t_uint64 default_ = kINVALID_ADDRESS) const;
		DW_TAG		getTag() const;
		bool		hasChild() const;
		bool		isArtificial() const;
		std::string getName(io::stream_base::ptr debug_str) const;
		std::string getMIPSLinkageName(io::stream_base::ptr debug_str) const;
		std::string getCompileDirectory(io::stream_base::ptr debug_str) const;
		
	private:
		DW_TAG		m_tag;
		FormsTable	m_values;
		t_uint64	m_abbr_idx;
		bool		m_has_child;

	};
}

#endif //__CDWARF_DEBUG_INFO_ENTRY_H__
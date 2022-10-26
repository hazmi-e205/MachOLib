#ifndef __CDWARF_AGGREGATOR_H__
#define __CDWARF_AGGREGATOR_H__

#include <map>
#include <vector>
#include <string>
#include <limits>
#include <memory>
#include "EDWARF.h"
#include "DWARFTypes.h"
#include "CDWARFDebugLine.h"
#include "CDWARFRangeTable.h"
#include "CDWARFArangeTable.h"
#include "CDWARFCompileUnitTable.h"
#include "CDWARFAbbreviationTable.h"

namespace macho
{
	class CMachO;
	class CSection;
}

#define NOT_USE_CHACHED_STREAM 0

namespace dwarf
{
	class CDWARFAggregator
	{
	public:
		typedef std::shared_ptr<CDWARFAggregator> Ptr;
		typedef std::shared_ptr<macho::CSection>	SectionPtr;
		typedef std::shared_ptr<macho::CMachO>	MachOPtr;
		static const t_uint32 kINVALID_LINE = 0xFFFFFFFF;
		enum { kCACHE_SIZE = 4096 };
	public:
		struct SubProgramDescription
		{
			std::string		Name;
			std::string		DeclFile;
			t_uint32		CallLine;

			inline std::string CallLineAsString()
			{
				char buffer[16] = {0};
				sprintf_s<sizeof(buffer)>(buffer,"%u",CallLine);
				return std::string(buffer);
			}
		};
	
		typedef std::vector<SubProgramDescription> SubProgramTable;
		typedef std::vector<const CDWARFDebugInfoEntry*> PDIETable;
	public:
		CDWARFAggregator(MachOPtr macho);
		~CDWARFAggregator();
		SubProgramTable				getInfoByAddress(t_uint64 address);
		void						clean();
		bool						is_valid() const;
	private:
		CDWARFArangeTable&			aranges();
		CDWARFCompileUnitTable&		compileUnits();
		CDWARFCompileUnitTable&		extractCompileUnit(const CDWARFArangeTable::CDWARFRange& offset);
		CDWARFAbbreviationTable&	abbreviations();
		CDWARFRangeTable&			ranges();
		CDWARFDebugLine&			lines();

		io::stream_base::ptr		streamLines();
		io::stream_base::ptr		streamInfo();
		io::stream_base::ptr		streamAbbrev();
		io::stream_base::ptr		streamStr();
		io::stream_base::ptr		streamRanges();
		io::stream_base::ptr		streamARanges();

		bool						hasRanges() const;

		std::string					getNameBy(CDWARFCompileUnitTable::CompileUnitNode& owner, CDWARFCompileUnit::DIEIdx idx);
		SubProgramTable				generateInfoBy(CDWARFCompileUnitTable::CompileUnitNode& owner, CDWARFCompileUnit::DIEIdx subprogram,CDWARFCompileUnit::DIEIdxTable subroutines, t_uint64 address);
		std::string					getInfoBySymbolTable(t_uint64 address);
		std::string					makeFullSubRoutineName(CDWARFCompileUnitTable::CompileUnitNode& owner, CDWARFCompileUnit::DIEIdx die);
		static bool					IsPossibleObjCMethodName (const char *name);
	private:
		t_int8					m_addresByteSize;
		bool					m_isvalid;
		MachOPtr				m_macho;
		SectionPtr				m_sec_lines;
		SectionPtr				m_sec_info;
		SectionPtr				m_sec_abbrev;
		SectionPtr				m_sec_str;
		SectionPtr				m_sec_aranges;
		SectionPtr				m_sec_ranges;

		CDWARFDebugLine*			m_lines;
		CDWARFCompileUnitTable*		m_units;
		CDWARFAbbreviationTable*	m_abbr;
		CDWARFArangeTable*			m_arange;
		CDWARFRangeTable*			m_ranges;
		
		io::stream_base::ptr	m_sm_lines;
		io::stream_base::ptr	m_sm_info;
		io::stream_base::ptr	m_sm_abbrev;
		io::stream_base::ptr	m_sm_str;
		io::stream_base::ptr	m_sm_aranges;
		io::stream_base::ptr	m_sm_ranges;
	};
}

#endif //
#ifndef __CDWARF_COMPILE_UNIT_TABLE_H__
#define __CDWARF_COMPILE_UNIT_TABLE_H__

#include "CDWARFCompileUnit.h"

namespace dwarf
{
	class CDWARFAbbreviationTable;
	class CDWARFCompileUnitTable
	{
	public:
		typedef std::shared_ptr<CDWARFCompileUnitTable> Ptr;
		
		struct CompileUnitNode
		{
			CompileUnitNode(t_uint64 offset)
				:Offset(offset),
				CU()
			{
				return;
			}

			bool operator < (const CompileUnitNode& other) const
			{
				return (Offset < other.Offset);
			}

			t_uint64 Offset;
			CDWARFCompileUnit CU;

		};

		typedef std::vector<CompileUnitNode> CUTable;
	public:
		static const t_uint8 kFORM_SIZES_32[];
		static const t_uint8 kFORM_SIZES_64[];
	public:
		CDWARFCompileUnitTable();
		bool parse(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str,io::stream_base::ptr stream);
		CompileUnitNode* extractCompileUnitBy(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str,io::stream_base::ptr stream, t_uint64 cu_offset);
		CompileUnitNode* getCompileUnitBy(t_uint64 cu_offset);
		const CDWARFDebugInfoEntry* findDeclaration(CompileUnitNode& owner, const CDWARFDebugInfoEntry& die, const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str, io::stream_base::ptr stream);
		
	private:
		CompileUnitNode* extract(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str,io::stream_base::ptr stream);
	private:
		CUTable m_arr;
	};
}

#endif //__CDWARF_COMPILE_UNIT_TABLE_H__
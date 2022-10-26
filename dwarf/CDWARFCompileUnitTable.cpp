#include "CDWARFCompileUnitTable.h"
#include <utility/CConsole.h>

namespace dwarf
{

	/*
	* CDWARFCompileUnitTable
	*/
	const t_uint8 CDWARFCompileUnitTable::kFORM_SIZES_32[] = {
		0, // 0x00 unused
		4, // 0x01 DW_FORM_addr
		0, // 0x02 unused
		0, // 0x03 DW_FORM_block2
		0, // 0x04 DW_FORM_block4
		2, // 0x05 DW_FORM_data2
		4, // 0x06 DW_FORM_data4
		8, // 0x07 DW_FORM_data8
		0, // 0x08 DW_FORM_string
		0, // 0x09 DW_FORM_block
		0, // 0x0a DW_FORM_block1
		1, // 0x0b DW_FORM_data1
		1, // 0x0c DW_FORM_flag
		0, // 0x0d DW_FORM_sdata
		4, // 0x0e DW_FORM_strp
		0, // 0x0f DW_FORM_udata
		4, // 0x10 DW_FORM_ref_addr
		1, // 0x11 DW_FORM_ref1
		2, // 0x12 DW_FORM_ref2
		4, // 0x13 DW_FORM_ref4
		8, // 0x14 DW_FORM_ref8
		0, // 0x15 DW_FORM_ref_udata
		0, // 0x16 DW_FORM_indirect
	};

	const t_uint8 CDWARFCompileUnitTable::kFORM_SIZES_64[] = {
		0, // 0x00 unused
		8, // 0x01 DW_FORM_addr
		0, // 0x02 unused
		0, // 0x03 DW_FORM_block2
		0, // 0x04 DW_FORM_block4
		2, // 0x05 DW_FORM_data2
		4, // 0x06 DW_FORM_data4
		8, // 0x07 DW_FORM_data8
		0, // 0x08 DW_FORM_string
		0, // 0x09 DW_FORM_block
		0, // 0x0a DW_FORM_block1
		1, // 0x0b DW_FORM_data1
		1, // 0x0c DW_FORM_flag
		0, // 0x0d DW_FORM_sdata
		4, // 0x0e DW_FORM_strp
		0, // 0x0f DW_FORM_udata
		8, // 0x10 DW_FORM_ref_addr
		1, // 0x11 DW_FORM_ref1
		2, // 0x12 DW_FORM_ref2
		4, // 0x13 DW_FORM_ref4
		8, // 0x14 DW_FORM_ref8
		0, // 0x15 DW_FORM_ref_udata
		0, // 0x16 DW_FORM_indirect
	};

	CDWARFCompileUnitTable::CDWARFCompileUnitTable()
		:m_arr()
	{
		return;
	}

	bool CDWARFCompileUnitTable::parse(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str,io::stream_base::ptr stream)
	{
		bool ret = true;
		int idx = 0;
		m_arr.clear();
		m_arr.reserve(256);
		while(stream->position() < stream->size()) 
		{
			CompileUnitNode* node = extract(abbr_table, debug_str, stream);
			if(node == NULL)
			{
				break;
			}
			stream->seek(m_arr.back().Offset + m_arr.back().CU.getLength() + sizeof(dwarf::t_uint32));
			TRACE("[CDWARFCompileUnitTable] complete read unit idx = %d, left %lld/%lld",idx, stream->position(), stream->size());
			++idx;
		}
		std::sort(m_arr.begin(),m_arr.end());
		return ret;
	}
	CDWARFCompileUnitTable::CompileUnitNode* CDWARFCompileUnitTable::getCompileUnitBy(t_uint64 cu_offset)
	{
		CompileUnitNode* ret = NULL;
		for(CUTable::iterator i = m_arr.begin(), e = m_arr.end();
			i != e;
			++i)
		{
			if((*i).Offset == cu_offset)
			{
				ret = &(*i);
			}
		}
		return ret;
	}
	CDWARFCompileUnitTable::CompileUnitNode* CDWARFCompileUnitTable::extractCompileUnitBy(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str, io::stream_base::ptr stream, t_uint64 cu_offset)
	{
		CompileUnitNode* cu = getCompileUnitBy(cu_offset);
		if(cu == NULL)
		{
			stream->seek(cu_offset);
			cu = extract(abbr_table, debug_str, stream);
		}
		return cu;
	}
	CDWARFCompileUnitTable::CompileUnitNode* CDWARFCompileUnitTable::extract(const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str,io::stream_base::ptr stream)
	{
		CompileUnitNode* ret = NULL;
		int idx = 0;
		m_arr.reserve(256);
		m_arr.push_back(CompileUnitNode(stream->position()));
		if(!m_arr.back().CU.parse(abbr_table,debug_str,stream))
		{
			m_arr.pop_back();
			ret = NULL;
		}
		else
		{
			ret = &m_arr.back();
		}
		return ret;
	}
	const CDWARFDebugInfoEntry* CDWARFCompileUnitTable::findDeclaration(CompileUnitNode& owner ,const CDWARFDebugInfoEntry& die, const CDWARFAbbreviationTable& abbr_table, io::stream_base::ptr debug_str, io::stream_base::ptr stream)
	{
		const CDWARFDebugInfoEntry *cur = &die, *ret = NULL;
		while(cur)
		{
			switch(cur->getTag())
			{
			case DW_TAG_inlined_subroutine:
				{
					t_uint64 off = cur->getAbstractOrigin() + owner.Offset;
					const CDWARFDebugInfoEntry* entry = owner.CU.extractDIEByOffset(abbr_table, debug_str, stream, off);
					cur = entry;
					break;
				}
			case DW_TAG_subprogram:
				{
					t_uint64 file = cur->getDeclFile();
					if(file != CDWARFDebugInfoEntry::kINVALID_ADDRESS)
					{
						ret = cur;
						cur = NULL;
					}
					else
					{
						t_uint64 obj_pointer = cur->getObjectPointer();
						t_uint64 spec_pointer = cur->getSpecification();
						if(spec_pointer != CDWARFDebugInfoEntry::kINVALID_ADDRESS)
						{
							const CDWARFDebugInfoEntry* entry = owner.CU.extractDIEByOffset(abbr_table, debug_str, stream, spec_pointer + owner.Offset);
							cur = entry;
						}
						else if(obj_pointer != CDWARFDebugInfoEntry::kINVALID_ADDRESS)
						{
							const CDWARFDebugInfoEntry* entry = owner.CU.extractDIEByOffset(abbr_table, debug_str, stream, obj_pointer + owner.Offset);
							cur = entry;
						}
						else
						{
							ret = cur = NULL;
						}
					}
					break;
				}
			default:
				ret = cur;
				cur = NULL;
				break;
			}
		}
		return ret;
	}
}// namespace dwarf
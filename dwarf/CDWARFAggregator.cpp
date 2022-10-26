#include "CDWARFAggregator.h"
#include <utility/demangler.hpp>
#include <io/stream_cache.hpp>
#include <utility/CConsole.h>
#include <macho/CFAT.h>
#include <queue>

namespace dwarf
{

	CDWARFAggregator::CDWARFAggregator(MachOPtr macho)
		:m_addresByteSize(0),
		m_isvalid(false),
		m_macho(macho),
		m_sec_lines(),
		m_sec_info(),
		m_sec_abbrev(),
		m_sec_str(),
		m_sec_aranges(),
		m_sec_ranges(),
		m_lines(NULL),
		m_units(NULL),
		m_abbr(NULL),
		m_arange(NULL),
		m_ranges(NULL),
		m_sm_lines(),
		m_sm_info(),
		m_sm_abbrev(),
		m_sm_str(),
		m_sm_aranges(),
		m_sm_ranges()
	{
		if(macho->isExistSegment(macho::CSegment::kSEGMENT_DWARF))
		{
			m_addresByteSize = macho->getAddressByteSize();
			macho::CSegment::Ptr seg = macho->getSegment(macho::CSegment::kSEGMENT_DWARF);
			m_sec_lines = seg->getSection(macho::CSection::kSECTION_DEBUG_LINE);
			m_sec_info = seg->getSection(macho::CSection::kSECTION_DEBUG_INFO);
			m_sec_abbrev = seg->getSection(macho::CSection::kSECTION_DEBUG_ABBR);
			m_sec_str = seg->getSection(macho::CSection::kSECTION_DEBUG_STR);
			m_sec_aranges = seg->getSection(macho::CSection::kSECTION_DEBUG_ARANGES);
			m_sec_ranges = seg->getSection(macho::CSection::kSECTION_DEBUG_RANGES);
			m_isvalid = (m_sec_lines.get() != NULL
						&& m_sec_info.get() != NULL
						&& m_sec_abbrev.get() != NULL
						&& m_sec_str.get() != NULL
						&& m_sec_aranges.get() != NULL
						//&& m_sec_ranges.get() != NULL
						&& m_sec_aranges.get() != NULL);
		}
		return;
	}
	CDWARFAggregator::~CDWARFAggregator()
	{
		clean();
		return;
	}
	void CDWARFAggregator::clean()
	{
		delete m_lines;
		m_lines = NULL;
		delete m_units;
		m_units = NULL;
		delete m_abbr;
		m_abbr = NULL;
		delete m_arange;
		m_arange = NULL;
		delete m_ranges;
		m_ranges = NULL;
		
		m_sm_lines.reset();
		m_sm_info.reset();
		m_sm_abbrev.reset();
		m_sm_str.reset();
		m_sm_ranges.reset();
		m_sm_aranges.reset();
		return;
	}
	bool CDWARFAggregator::is_valid() const
	{
		return m_isvalid;
	}
	io::stream_base::ptr CDWARFAggregator::streamLines()
	{
#if NOT_USE_CHACHED_STREAM
		if(!m_sm_lines)
		{
			m_sm_lines = m_sec_lines->load();
		}
		return m_sm_lines;
#else
		return io::make_cache<kCACHE_SIZE>(m_sec_lines->read());
#endif
	}
	io::stream_base::ptr CDWARFAggregator::streamInfo()
	{
#if NOT_USE_CHACHED_STREAM
		if(!m_sm_info)
		{
			m_sm_info = m_sec_info->load();
		}
		return m_sm_info;
#else
		return io::make_cache<kCACHE_SIZE>(m_sec_info->read());
#endif
	}
	io::stream_base::ptr CDWARFAggregator::streamAbbrev()
	{
		if(!m_sm_abbrev)
		{
			m_sm_abbrev = m_sec_abbrev->load();
		}
		return m_sm_abbrev;
	}
	io::stream_base::ptr CDWARFAggregator::streamStr()
	{
#if NOT_USE_CHACHED_STREAM
		if(!m_sm_str)
		{
			m_sm_str = m_sec_str->load();
		}
		return m_sm_str;
#else
		return io::make_cache<kCACHE_SIZE>(m_sec_str->read());
#endif
	}
	io::stream_base::ptr CDWARFAggregator::streamARanges()
	{
		if(!m_sm_aranges)
		{
			m_sm_aranges = m_sec_aranges->load();
		}
		return m_sm_aranges;
	}
	io::stream_base::ptr CDWARFAggregator::streamRanges()
	{
#if NOT_USE_CHACHED_STREAM
		if(!m_sm_ranges)
		{
			m_sm_ranges = m_sec_ranges->load();
		}
		return m_sm_ranges;
#else
		return io::make_cache<kCACHE_SIZE>(m_sec_ranges->read());
#endif
	}
	bool CDWARFAggregator::hasRanges() const
	{
		return (m_sec_ranges.get() != NULL);
	}
	CDWARFArangeTable& CDWARFAggregator::aranges()
	{
		if(m_arange == NULL)
		{
			m_arange = new CDWARFArangeTable();
			/*PARSE*/
			m_arange->parse(streamARanges());
		}
		return *m_arange;
	}
	CDWARFAbbreviationTable& CDWARFAggregator::abbreviations()
	{
		if(m_abbr == NULL)
		{
			m_abbr = new CDWARFAbbreviationTable();
			/*PARSE*/
			m_abbr->parse(streamAbbrev());
		}
		return *m_abbr;
	}
	CDWARFRangeTable& CDWARFAggregator::ranges()
	{
		if(m_ranges == NULL)
		{
			m_ranges = new CDWARFRangeTable();
		}
		return *m_ranges;
	}
	CDWARFCompileUnitTable&	CDWARFAggregator::compileUnits()
	{
		if(m_units == NULL)
		{
			m_units = new CDWARFCompileUnitTable();
		}
		return *m_units;
	}
	CDWARFDebugLine& CDWARFAggregator::lines()
	{
		if(m_lines == NULL)
		{
			m_lines = new CDWARFDebugLine();
		}
		return *m_lines;
	}
	CDWARFCompileUnitTable& CDWARFAggregator::extractCompileUnit(const CDWARFArangeTable::CDWARFRange& offset)
	{
		compileUnits().extractCompileUnitBy(abbreviations(), streamStr(), streamInfo(), offset.CUOffset);
		return *m_units;
	}
	CDWARFAggregator::SubProgramTable CDWARFAggregator::getInfoByAddress(t_uint64 address)
	{
		const CDWARFArangeTable::CDWARFRange* range = aranges().getRangesBy(address);
		if(range)
		{
			//extract by CU offset
			CDWARFCompileUnitTable& units = extractCompileUnit(*range);
			//get subprograms and inlined subroutines of compile unit
			CDWARFCompileUnitTable::CompileUnitNode* cu = units.getCompileUnitBy(range->CUOffset);
			if(cu)
			{
				dwarf::CDWARFCompileUnit::DIEIdxTable itab;
				if(hasRanges())
				{
					utility::CDWARFRangeResolver range_resolver(ranges(), m_addresByteSize, streamRanges());
					itab = cu->CU.getDIEIdxsBy(address, range_resolver, abbreviations(), streamStr(), streamInfo());
				}
				else
				{
					TRACE("[CDWARFAggregator] ERROR getInfoByAddress (__debug_range section is unavailable)");
					itab = cu->CU.getDIEIdxsBy(address, utility::CDWARFRangeResolver(), abbreviations(), streamStr(), streamInfo());
				}

				CDWARFCompileUnit::DIEIdxTable subroutines;
				CDWARFCompileUnit::DIEIdx subprogram = NULL;
				for(size_t i = 0; i < itab.size(); ++i)
				{
					if(cu->CU.getDIEByIdx(itab[i])->getTag() == DW_TAG_subprogram)
					{
						subprogram = itab[i];
						break;
					}
				}

				for(size_t i = 0; i < itab.size(); ++i)
				{
					if(cu->CU.getDIEByIdx(itab[i])->getTag() == DW_TAG_inlined_subroutine)
					{
						subroutines.push_back(itab[i]);
					}
				}
				return generateInfoBy(*cu, subprogram, subroutines, address);
			}
			else
			{
				TRACE("[CDWARFAggregator] ERROR getInfoByAddress (Compile Unit by offset %llu is unavailable)",range->CUOffset);
			}
		}
		else
		{
			if(m_macho && m_macho->isExistSegment(macho::CSegment::kSEGMENT_TEXT))
			{
				TRACE("[CDWARFAggregator] ERROR getInfoByAddress (ARange by address %llu is unavailable (in crash log it %llu - %llu = %llu))", address,address, m_macho->getSegment(macho::CSegment::kSEGMENT_TEXT)->getVmAddress(), address - m_macho->getSegment(macho::CSegment::kSEGMENT_TEXT)->getVmAddress());
			}
			else
			{
				TRACE("[CDWARFAggregator] ERROR getInfoByAddress (ARange by address %llu is unavailable)", address);
			}
		}
		CDWARFAggregator::SubProgramTable ret;
		SubProgramDescription des;
		des.Name = getInfoBySymbolTable(address);
		des.DeclFile = "";
		des.CallLine = 0;
		ret.push_back(des);
		return ret;
	}
	CDWARFAggregator::SubProgramTable CDWARFAggregator::generateInfoBy(CDWARFCompileUnitTable::CompileUnitNode& owner, CDWARFCompileUnit::DIEIdx subprogram,CDWARFCompileUnit::DIEIdxTable subroutines, t_uint64 address)
	{
		CDWARFAggregator::SubProgramTable t;
		t.push_back(SubProgramDescription());
		std::vector<std::string> subroutine_names;
		t.back().Name = demangle(getNameBy(owner, subprogram).c_str());
		const CDWARFDebugLine::CDWARFLineTable* lines_table = lines().extractLineTableByOffset(streamLines(), owner.CU.getStmtList());
		if(lines_table)
		{
			for(CDWARFCompileUnit::DIEIdxTable::const_iterator i = subroutines.begin(), e = subroutines.end();
				i != e;
				++i)
			{
				const CDWARFDebugInfoEntry& cur = *owner.CU.getDIEByIdx(*i);
				t.push_back(SubProgramDescription());
				t.back().Name = demangle(getNameBy(owner, *i).c_str());
				(*(t.end() - 2)).CallLine = static_cast<t_uint32>(cur.getCallLine());
				(*(t.end() - 2)).DeclFile = lines_table->getFile(static_cast<t_uint16>(cur.getCallFile()));
			}
			unsigned int idx = lines_table->lookupAddress(address,0);
			t.back().CallLine = lines_table->at(idx).Line;
			t.back().DeclFile = lines_table->getFile(lines_table->at(idx).File);
		}
		else
		{
			assert(false && "Line table is not found");
		}
		return t;
	}
	std::string CDWARFAggregator::getNameBy(CDWARFCompileUnitTable::CompileUnitNode& owner, CDWARFCompileUnit::DIEIdx idx)
	{
		std::string ret_name;
		CDWARFCompileUnit::DIEIdx icur = idx, iret = CDWARFCompileUnit::kINVALID_IDX;
		const CDWARFDebugInfoEntry *cur = owner.CU.getDIEByIdx(idx), *ret = NULL;
		t_uint64 lowpc = cur->getLowPC();
		const CDWARFArangeTable::CDWARFRange* range = aranges().getRangesBy(lowpc);
		if(lowpc == CDWARFDebugInfoEntry::kINVALID_ADDRESS || (range && owner.Offset == range->CUOffset))
		{
			while(cur)
			{
				switch(cur->getTag())
				{
				case DW_TAG_inlined_subroutine:
					{
						t_uint64 off = cur->getAbstractOrigin() + owner.Offset;
						icur = owner.CU.getIdxByOffset(off);
						cur = owner.CU.getDIEByIdx(icur);
						break;
					}
				case DW_TAG_subprogram:
					{
						t_uint64 name = cur->getNamePtr(), mips_name = cur->getMIPSLinkageNamePtr();
						if(name != CDWARFDebugInfoEntry::kINVALID_ADDRESS || mips_name != CDWARFDebugInfoEntry::kINVALID_ADDRESS)
						{
							ret = cur;
							cur = NULL;
							iret = icur;
							icur = CDWARFCompileUnit::kINVALID_IDX;
						}
						else
						{
							t_uint64 spec_pointer = cur->getSpecification();
							t_uint64 abstract_origin = cur->getAbstractOrigin();
							t_uint64 obj_pointer = cur->getObjectPointer();
							if(spec_pointer != CDWARFDebugInfoEntry::kINVALID_ADDRESS)
							{
								icur = owner.CU.getIdxByOffset(spec_pointer + owner.Offset);
								cur = owner.CU.getDIEByIdx(icur);
							}
							else if(abstract_origin != CDWARFDebugInfoEntry::kINVALID_ADDRESS)
							{
								icur = owner.CU.getIdxByOffset(abstract_origin + owner.Offset);
								cur = owner.CU.getDIEByIdx(icur);
							}
							else if(obj_pointer != CDWARFDebugInfoEntry::kINVALID_ADDRESS)
							{
								icur = owner.CU.getIdxByOffset(obj_pointer + owner.Offset);
								cur = owner.CU.getDIEByIdx(icur);
							}
							else
							{
								ret = cur = NULL;
								iret = icur = CDWARFCompileUnit::kINVALID_IDX;
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
			if(iret != CDWARFCompileUnit::kINVALID_IDX && ret)
			{
				ret_name = ret->getMIPSLinkageName(streamStr());
				if(ret_name.empty())
				{
					ret_name = makeFullSubRoutineName(owner, iret);
				}
			}
		}
		return ret_name;
	}
	std::string CDWARFAggregator::makeFullSubRoutineName(CDWARFCompileUnitTable::CompileUnitNode& owner, CDWARFCompileUnit::DIEIdx idie)
	{
		std::string parents_names, function, parameters, result;
		const CDWARFDebugInfoEntry& die = *owner.CU.getDIEByIdx(idie);
		function = die.getName(streamStr());
		if(IsPossibleObjCMethodName(function.c_str()) == false)
		{
			CDWARFCompileUnit::DIEIdxTable parents = owner.CU.getParentsIdxs(idie);
			CDWARFCompileUnit::DIEIdxTable childrens = owner.CU.getChildrensIdxs(idie);
			for(CDWARFCompileUnit::DIEIdxTable::reverse_iterator i = parents.rbegin(), e = parents.rend();
				i != e;
				++i)
			{
				const CDWARFDebugInfoEntry& cur = *owner.CU.getDIEByIdx(*i);
				if(cur.getTag() == DW_TAG_structure_type
					|| cur.getTag() == DW_TAG_class_type
					|| cur.getTag() == DW_TAG_namespace)
				{
					owner.CU.fullExtract(*i, abbreviations(), streamStr(), streamInfo());
					parents_names.append(cur.getName(streamStr()));
					parents_names.append("::");
				}
			}
			for(CDWARFCompileUnit::DIEIdxTable::reverse_iterator i = childrens.rbegin(), e = childrens.rend();
				i != e;
				++i)
			{
				owner.CU.fullExtract(*i, abbreviations(), streamStr(), streamInfo());
				const CDWARFDebugInfoEntry& cur = *owner.CU.getDIEByIdx(*i);
				if(cur.getTag() == DW_TAG_formal_parameter && !cur.isArtificial())
				{
					const CDWARFDebugInfoEntry* entry = &cur;
					std::string full_name;
					std::queue<DW_TAG> tags;
					do
					{
						t_uint64 toff = entry->getType();
						if(toff != CDWARFDebugInfoEntry::kINVALID_ADDRESS)
						{
							entry = owner.CU.getDIEByOffset(toff + owner.Offset);
							if(entry)
							{
								owner.CU.fullExtract(entry, abbreviations(), streamStr(), streamInfo());
								switch (entry->getTag())
								{
								case DW_TAG_array_type:			tags.push(DW_TAG_array_type);		break;
									//case DW_TAG_base_type:          full_name.append("base ");		break;
									//case DW_TAG_class_type:         full_name.append("class ");		break;
								case DW_TAG_const_type:         full_name.append("const ");		break;
									//case DW_TAG_enumeration_type:   full_name.append("enum ");		break;
									//case DW_TAG_file_type:          full_name.append("file ");		break;
									//case DW_TAG_interface_type:     full_name.append("interface ");	break;
									//case DW_TAG_packed_type:        full_name.append("packed ");	break;
								case DW_TAG_pointer_type:		tags.push(DW_TAG_pointer_type);		break;
								case DW_TAG_ptr_to_member_type:	tags.push(DW_TAG_ptr_to_member_type); break;
								case DW_TAG_reference_type:		tags.push(DW_TAG_reference_type);	break;
									//case DW_TAG_restrict_type:      full_name.append("restrict ");	break;
									//case DW_TAG_set_type:           full_name.append("set ");		break;
									//case DW_TAG_shared_type:        full_name.append("shared ");	break;
									//case DW_TAG_string_type:        full_name.append("string ");	break;
									//case DW_TAG_structure_type:     full_name.append("struct ");	break;
									//case DW_TAG_subrange_type:      full_name.append("subrange ");	break;
									//case DW_TAG_subroutine_type:    full_name.append("function ");	break;
									//case DW_TAG_thrown_type:        full_name.append("thrown ");	break;
									//case DW_TAG_union_type:         full_name.append("union ");		break;
									//case DW_TAG_unspecified_type:   full_name.append("unspecified ");	break;
								case DW_TAG_volatile_type:      full_name.append("volatile ");		break;
								default:
									break;
								}
							}
						}
						else
						{
							entry = NULL;
						}
					} while(entry && entry->getNamePtr() == CDWARFDebugInfoEntry::kINVALID_ADDRESS);

					if(entry)
					{
						full_name += entry->getName(streamStr());
						while(!tags.empty())
						{
							switch (tags.front())
							{
							case DW_TAG_array_type:         full_name += "[]";	break;
							case DW_TAG_pointer_type:       full_name += "*";	break;
							case DW_TAG_ptr_to_member_type: full_name += "*";	break;
							case DW_TAG_reference_type:     full_name += "&";	break;
							default:
								break;
							}
							tags.pop();
						}
					}
					parameters.insert(0,full_name + ", ");
				}
			}
			if(!parameters.empty())
			{
				std::string::size_type p = parameters.find_last_of(", ");
				if(p != std::string::npos && p > 0)
				{
					parameters.erase(p - 1, 2);
				}
			}
			result = parents_names + function + "(" + parameters + ")";
		}
		else
		{
			result = function;
		}
		return result;
	}
	std::string CDWARFAggregator::getInfoBySymbolTable(t_uint64 address)
	{
		std::string ret;
		macho::CSymbolTable& symbols = m_macho->getSymbolTable();
		macho::t_uint32 sym_idx = symbols.getSymbolIdxFirstAtRange(address);
		if(sym_idx != macho::CSymbolTable::kINVALID_SYMBLO_IDX) 
		{
			macho::CSymbol& sym = symbols.at(sym_idx);
			if(sym.isDefined() && sym.section() != macho::CSymbol::kNO_SECT)
			{
				std::string magled = m_macho->getSymbolAt(sym.strIdx());
				if(sym.demagled().empty())
				{
					ret = sym.demagled() = macho::CMachO::Demangle(magled.c_str());
				}
				else
				{
					ret = sym.demagled();
				}
			}
		}
		return ret;
	}
	bool CDWARFAggregator::IsPossibleObjCMethodName(const char *name)
	{
		if (!name && strlen(name) >= 3)
		{
			return false;
		}
		bool starts_right = (name[0] == '+' || name[0] == '-') && name[1] == '[';
		bool ends_right = (name[strlen(name) - 1] == ']');
		return (starts_right && ends_right);
	}
}
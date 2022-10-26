#include "CDWARFDebugLine.h"

//#include <sstream>// for dump
#include "IDWARFNameResolverByAddress.h"// for dump

namespace dwarf
{
	/*
	* CDWARFDebugLine::CDWARFPrologue
	*/
	CDWARFDebugLine::CDWARFPrologue::CDWARFPrologue()
		:m_totalLength(0),
		m_version(0),
		m_prologueLength(0), 
		m_minInstLength(0),
		m_defaultIsStmt(0),
		m_lineBase(0),
		m_lineRange(0),
		m_opcodeBase(0) 
	{
		return;
	}
	void CDWARFDebugLine::CDWARFPrologue::push_file(const CDWARFFileNameEntry& entry)
	{
		m_fileNames.push_back(entry);
		return;
	}
	unsigned int CDWARFDebugLine::CDWARFPrologue::getLength() const 
	{
		return m_prologueLength + sizeof(m_totalLength) + sizeof(m_version) + sizeof(m_prologueLength);
	}
	unsigned int CDWARFDebugLine::CDWARFPrologue::getTotalLength() const
	{
		return m_totalLength;
	}
	unsigned int CDWARFDebugLine::CDWARFPrologue::getStatementTableLength() const 
	{
		return m_totalLength + sizeof(m_totalLength) - getLength();
	}
	signed int CDWARFDebugLine::CDWARFPrologue::getMaxLineIncrementForSpecialOpcode() const 
	{
		return m_lineBase + (signed char)m_lineRange - 1;
	}
	unsigned char CDWARFDebugLine::CDWARFPrologue::getOpcodeBase() const
	{
		return m_opcodeBase;
	}
	unsigned char CDWARFDebugLine::CDWARFPrologue::getMinInstLength() const
	{
		return m_minInstLength;
	}
	unsigned char CDWARFDebugLine::CDWARFPrologue::getLineRange() const
	{
		return m_lineRange;
	}
	signed char CDWARFDebugLine::CDWARFPrologue::getLineBase() const
	{
		return m_lineBase;
	}
	const std::vector<unsigned char>& CDWARFDebugLine::CDWARFPrologue::getStandardOpcodeLengths() const
	{
		return m_standardOpcodeLengths;
	}
	unsigned short CDWARFDebugLine::CDWARFPrologue::getVersion() const
	{
		return m_version;
	}
	bool CDWARFDebugLine::CDWARFPrologue::getDefaultIsStmt() const
	{
		return (m_defaultIsStmt != 0);
	}
	const std::string& CDWARFDebugLine::CDWARFPrologue::file(unsigned int idx) const
	{
		return m_fileNames[idx].Name;
	}
	void CDWARFDebugLine::CDWARFPrologue::clear() 
	{
		m_totalLength = m_version = m_prologueLength = 0;
		m_minInstLength = m_lineBase = m_lineRange = m_opcodeBase = 0;
		m_standardOpcodeLengths.clear();
		m_includeDirectories.clear();
		m_fileNames.clear();
		return;
	}
	CDWARFDebugLine::CDWARFPrologue CDWARFDebugLine::CDWARFPrologue::ParsePrologue(io::stream_base::ptr stream)
	{
		CDWARFPrologue oprologue;
		{
			unsigned char dummy_MaxOpsPerInstruction;

			io::t_uint32& total_length = reinterpret_cast<io::t_uint32&>(oprologue.m_totalLength);
			io::t_uint16& version = reinterpret_cast<io::t_uint16&>(oprologue.m_version);
			io::t_uint32& prologue_length = reinterpret_cast<io::t_uint32&>(oprologue.m_prologueLength);
			io::t_uint8& min_inst_length = reinterpret_cast<io::t_uint8&>(oprologue.m_minInstLength);
			io::t_uint8& max_ops_per_instruction = reinterpret_cast<io::t_uint8&>(dummy_MaxOpsPerInstruction);
			io::t_uint8& default_is_stmt = reinterpret_cast<io::t_uint8&>(oprologue.m_defaultIsStmt);
			io::t_uint8& line_base = reinterpret_cast<io::t_uint8&>(oprologue.m_lineBase);
			io::t_uint8& line_range = reinterpret_cast<io::t_uint8&>(oprologue.m_lineRange);
			io::t_uint8& opcode_base = reinterpret_cast<io::t_uint8&>(oprologue.m_opcodeBase);


			stream->t_read(total_length);
			stream->t_read(version);
			if(version < 2 || version > 4)
			{ 
				assert(false && "unsupported prologue version!!!");
				return oprologue; 
			}
			stream->t_read(prologue_length);
			macho::t_uint32 end_prologue = static_cast<macho::t_uint32>(stream->position()) + prologue_length;
			assert((end_prologue < static_cast<macho::t_uint32>(stream->size())) && "invalid length of prologue!!!");
			stream->t_read(min_inst_length);
			if (version >= 4)
			{
				stream->t_read(max_ops_per_instruction);
			}
			stream->t_read(default_is_stmt);
			stream->t_read(line_base);
			stream->t_read(line_range);
			stream->t_read(opcode_base);

			for (io::t_uint8 i = 1; i < opcode_base; ++i)
			{
				io::t_uint8 op_len;
				stream->t_read(op_len);
				oprologue.m_standardOpcodeLengths.push_back(op_len);
			}

			while (stream->position() < end_prologue)
			{
				char sz[4096] = {0};
				stream->read_sz(sz,4096);
				if(sz[0])
				{
					oprologue.m_includeDirectories.push_back(sz);
				}
				else
				{
					break;
				}
			}
			while (stream->position() < end_prologue)
			{
				char sz[4096] = {0};
				stream->read_sz(sz,4096);
				if(sz[0])
				{
					CDWARFFileNameEntry fileEntry;
					fileEntry.Name = sz;
					io::t_uint64 &dirIdx = reinterpret_cast<io::t_uint64&>(fileEntry.DirIdx);
					io::t_uint64 &modTime = reinterpret_cast<io::t_uint64&>(fileEntry.ModTime);
					io::t_uint64 &length = reinterpret_cast<io::t_uint64&>(fileEntry.Length);
					stream->read_uleb128(dirIdx);
					stream->read_uleb128(modTime);
					stream->read_uleb128(length);
					oprologue.m_fileNames.push_back(fileEntry);
				}
				else
				{
					break;
				}
			}
		}
		return oprologue;
	}
	/*
	* CDWARFDebugLine::CDWARFLineTable
	*/
	const unsigned int CDWARFDebugLine::CDWARFLineTable::kBAD_ROW_IDX = std::numeric_limits<unsigned int>::max();
	CDWARFDebugLine::CDWARFLineTable::CDWARFLineTable()
		:m_prologue(),
		m_rows(),
		m_min_addr(std::numeric_limits<unsigned long long int>::max()),
		m_max_addr(0)
	{
		return;
	}
	unsigned int CDWARFDebugLine::CDWARFLineTable::lookupAddress(unsigned long long int address, unsigned long long int cu_high_pc) const
	{
		unsigned int index = std::numeric_limits<unsigned int>::max();
		std::vector<CDWARFRow>::const_iterator pos = std::lower_bound(m_rows.begin(), m_rows.end(), address, SearchRow());
		if(pos != m_rows.end())
		{
			index = static_cast<unsigned int>(pos - m_rows.begin());
            if ((*pos).Address > address)
            {
				if (index > 0) {
                    --index;
				}
				else {
                    index = std::numeric_limits<unsigned int>::max();
				}
            }
		}
		return index;
	}
	unsigned int CDWARFDebugLine::CDWARFLineTable::count() const
	{
		return m_rows.size();
	}
	unsigned long long int CDWARFDebugLine::CDWARFLineTable::minAddr() const
	{
		return m_min_addr;
	}
	unsigned long long int CDWARFDebugLine::CDWARFLineTable::maxAddr() const
	{
		return m_max_addr;
	}
	const CDWARFDebugLine::CDWARFRow& CDWARFDebugLine::CDWARFLineTable::at(unsigned int idx) const
	{
		return m_rows[idx];
	}
	std::string CDWARFDebugLine::CDWARFLineTable::getFile(const CDWARFRow& row) const
	{
		return getFile(row.File);
	}
	std::string CDWARFDebugLine::CDWARFLineTable::getFile(t_uint16 row) const
	{
		return m_prologue.file(row - 1);
	}
	bool CDWARFDebugLine::CDWARFLineTable::operator < (const CDWARFLineTable& other) const
	{
		return (m_max_addr < other.m_max_addr);
	}
	bool CDWARFDebugLine::CDWARFLineTable::parse(io::stream_base::ptr stream)
	{
		bool ret = false;
		const io::bstreamsize start_pos = stream->position();
		m_prologue = CDWARFPrologue::ParsePrologue(stream);
		CDWARFPrologue& prologue = m_prologue;
		unsigned short version = m_prologue.getVersion();
		if(version >= 2 && version <= 4)
		{
			ret = true;
			unsigned int commands = 0;
			CDWARFRow row;
			const io::bstreamsize end_offset = start_pos + prologue.getTotalLength() + sizeof(unsigned int);
			while (stream->position() < end_offset) 
			{
				io::t_uint8 opcode;
				io::t_uint64 len, arg_size;
				io::t_uint8 sub_opcode;

				stream->t_read(opcode);
				if (opcode == 0) {
					io::bstreamsize pos = stream->position();
					stream->read_uleb128(len);
					stream->t_read(sub_opcode);
					arg_size = len - sizeof(sub_opcode);
					switch (sub_opcode) {
					case DW_LNE_end_sequence:
						{
							row.EndSequence = true;
							m_min_addr = std::min(m_min_addr,row.Address);
							m_max_addr = std::max(m_max_addr,row.Address);
							m_rows.push_back(row);
							row.reset(prologue.getDefaultIsStmt());
							break;
						}

					case DW_LNE_set_address:
						{
							io::t_uint64 addr64;
							io::t_uint32 addr32;
							switch(arg_size)
							{
							case 4:
								stream->t_read(addr32);
								row.Address = addr32;
								break;
							case 8:
								stream->t_read(addr64);
								row.Address = addr64;
								break;
							default:
								assert(false && "bad size of address");
								break;
							}
							break;
						}

					case DW_LNE_define_file:
						{							
							CDWARFFileNameEntry fileEntry;
							char sz[4096] = {0};
							io::t_uint64 &dirIdx = reinterpret_cast<io::t_uint64&>(fileEntry.DirIdx);
							io::t_uint64 &modTime = reinterpret_cast<io::t_uint64&>(fileEntry.ModTime);
							io::t_uint64 &length = reinterpret_cast<io::t_uint64&>(fileEntry.Length);
							stream->read_sz(sz,4096);
							fileEntry.Name = sz;
							stream->read_uleb128(dirIdx);
							stream->read_uleb128(modTime);
							stream->read_uleb128(length);
							m_prologue.push_file(fileEntry);
							break;
						}

					default:
						stream->seek(pos + len);
						break;
					}
				} 
				else if (opcode < m_prologue.getOpcodeBase()) 
				{
					switch (opcode) {
					  case DW_LNS_copy:
						  m_rows.push_back(row);
						  row.BasicBlock = false;
						  row.PrologueEnd = false;
						  row.EpilogueBegin = false;
						  break;
					  case DW_LNS_advance_pc:
						  {
							  macho::t_uint64 addr;
							  stream->read_uleb128(addr);
							  row.Address += addr * ((macho::t_uint64)m_prologue.getMinInstLength());
							  break;
						  }

					  case DW_LNS_advance_line:
						  {
							  io::t_int64 line;
							  stream->read_sleb128(line);
							  row.Line += static_cast<unsigned int>(line);
							  break;
						  }
					  case DW_LNS_set_file:
						  {
							  io::t_uint64 ufile;
							  stream->read_uleb128(ufile);
							  row.File = static_cast<unsigned short>(ufile);
							  break;
						  }

					  case DW_LNS_set_column:
						  {
							  io::t_uint64 ucolomn;
							  stream->read_uleb128(ucolomn);
							  row.Column = static_cast<unsigned short>(ucolomn);
							  break;
						  }

					  case DW_LNS_negate_stmt:
						  row.IsStmt = !row.IsStmt;
						  break;

					  case DW_LNS_set_basic_block:
						  row.BasicBlock = true;
						  break;

					  case DW_LNS_const_add_pc:
						  {
							  io::t_uint8 adjust_opcode = 255 - m_prologue.getOpcodeBase();
							  io::t_uint64 addr_offset = (adjust_opcode / m_prologue.getLineRange()) * m_prologue.getMinInstLength();
							  row.Address += addr_offset;
							  break;
						  }

					  case DW_LNS_fixed_advance_pc:
						  {
							  io::t_uint16 addr;
							  stream->t_read(addr);
							  row.Address += addr;
							  break;
						  }

					  case DW_LNS_set_prologue_end:
						  row.PrologueEnd = true;
						  break;

					  case DW_LNS_set_epilogue_begin:
						  row.EpilogueBegin = true;
						  break;

					  case DW_LNS_set_isa:
						  {
							  io::t_uint64 isa;
							  stream->read_uleb128(isa);
							  row.Isa = static_cast<unsigned char>(isa);
							  break;
						  }

					  default:
						  {
							  assert(opcode - 1U < m_prologue.getStandardOpcodeLengths().size());
							  io::t_uint8 opcode_length = m_prologue.getStandardOpcodeLengths()[opcode - 1];
							  for (io::t_uint8 i = 0; i< opcode_length; ++i) {
								  io::t_uint64 idle;
								  stream->read_uleb128(idle);
							  }
							  break;
						  }
					}
				} 
				else 
				{
					io::t_uint8 adjust_opcode = opcode - prologue.getOpcodeBase();
					io::t_uint64 addr_offset = (adjust_opcode / prologue.getLineRange()) * prologue.getMinInstLength();
					io::t_int32 line_offset = prologue.getLineBase() + (adjust_opcode % prologue.getLineRange());
					row.Line += line_offset;
					row.Address += addr_offset;
					m_min_addr = std::min(m_min_addr,row.Address);
					m_max_addr = std::max(m_max_addr,row.Address);
					m_rows.push_back(row);
					row.BasicBlock = false;
					row.PrologueEnd = false;
					row.EpilogueBegin = false;
				}
				++commands;
			}
		}
		else
		{
			assert(false);
		}
		std::sort(m_rows.begin(), m_rows.end());
		return ret;
	}
	std::string CDWARFDebugLine::CDWARFLineTable::dump(IDWARFNameResolverByAddress* resolver) const
	{
		std::string ret;
		for(std::vector<CDWARFRow>::const_iterator i = m_rows.begin(), e = m_rows.end();
			i != e;
			++i)
		{
			char buffer[32];
			ret.append("\t-------------row[");
			sprintf_s<sizeof(buffer)>(buffer,"%d",i - m_rows.begin());
			ret.append(buffer);
			ret.append("]------------\n");
			if(resolver)
			{
				ret.append("\t");
				ret.append(getFile(*i));
				ret.append(":");
				sprintf_s<sizeof(buffer)>(buffer,"%d",(*i).Line);
				ret.append(buffer); 
				ret.append(" Addr: 0x");
				sprintf_s<sizeof(buffer)>(buffer,"%x",(*i).Address);
				ret.append(buffer); 
				ret.append(" Name: ");
				ret.append(resolver->resolve((*i).Address));
				ret.append("\n");
			}
			else
			{
				ret.append("\t");
				ret.append(getFile(*i));
				ret.append(":");
				sprintf_s<sizeof(buffer)>(buffer,"%d",(*i).Line);
				ret.append(buffer); 
				ret.append(" Addr: 0x");
				sprintf_s<sizeof(buffer)>(buffer,"%x",(*i).Address);
				ret.append(buffer);
			}
		}
		return ret;
	}
	/*
	* CDWARFDebugLine
	*/
	const CDWARFDebugLine::CDWARFLineTable CDWARFDebugLine::kBAD_LINE_TABLE;

	CDWARFDebugLine::CDWARFDebugLine()
		:m_table()
	{
		return;
	}
	bool CDWARFDebugLine::parse(io::stream_base::ptr stream)
	{
		bool ret = true;
		m_table.clear();
		while(stream->position() < stream->size()) {
			m_table.push_back(LineTableNode(stream->position()));
			if((ret = m_table.back().LineTable.parse(stream)) == false)
			{
				m_table.clear();
				ret = false;
				break;
			}
		}
		std::sort(m_table.begin(),m_table.end());
		return ret;
	}
	CDWARFDebugLine::MapOfLineTableIterator CDWARFDebugLine::insert(const LineTableNode& node)
	{
		MapOfLineTableIterator it = std::lower_bound(m_table.begin(), m_table.end(), node);
		it = m_table.insert(it, node);
		return it;
	}
	CDWARFDebugLine::LineTableNode* CDWARFDebugLine::extract(io::stream_base::ptr stream)
	{
		CDWARFDebugLine::LineTableNode* ret = NULL;
		LineTableNode temp(stream->position());
		if(temp.LineTable.parse(stream))
		{
			MapOfLineTableIterator it = insert(temp);
			ret = &(*it);
		}
		return ret;
	}
	const CDWARFDebugLine::CDWARFLineTable& CDWARFDebugLine::getLineTable(const unsigned long long int& offset) const
	{
		MapOfLineTableConstIterator pos = std::lower_bound(m_table.begin(), m_table.end(), offset, SearchTable());
		if(pos != m_table.end())
		{
			return (*pos).LineTable;
		}
		return kBAD_LINE_TABLE;
	}
	const CDWARFDebugLine::CDWARFLineTable* CDWARFDebugLine::getLineTableByOffset(t_uint64 offset) const
	{
		MapOfLineTableConstIterator pos = std::find_if(m_table.begin(), m_table.end(), SearchTableByOffset(offset));
		if(pos != m_table.end())
		{
			return &((*pos).LineTable);
		}
		return NULL;
	}
	const CDWARFDebugLine::CDWARFLineTable* CDWARFDebugLine::extractLineTableByOffset(io::stream_base::ptr stream, t_uint64 stmt_offset)
	{
		const CDWARFLineTable* ret = getLineTableByOffset(stmt_offset);
		if(ret == NULL)
		{
			stream->seek(stmt_offset);
			LineTableNode* node = extract(stream);
			ret = (node)?(&(node->LineTable)):NULL;
		}
		return ret;
	}
	std::string CDWARFDebugLine::dump(IDWARFNameResolverByAddress* resolve) const
	{
		char buffer[32] = {0};
		std::string ret;
		for(MapOfLineTableConstIterator i = m_table.begin(), e = m_table.end();
			i != e;
			++i)
		{
			ret.append(">>>table[");
			sprintf_s<sizeof(buffer)>(buffer,"%d",i - m_table.begin());
			ret.append(buffer);
			ret.append("]>>>\n");
			ret.append((*i).LineTable.dump(resolve));
		}
		return ret;
	}
}
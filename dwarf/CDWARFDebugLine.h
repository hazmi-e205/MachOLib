#ifndef __CDWARF_DEBUG_LINE_H__
#define __CDWARF_DEBUG_LINE_H__

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

namespace dwarf
{
	//forward declarations
	class IDWARFNameResolverByAddress;

	class CDWARFDebugLine
	{
	public:
		typedef std::shared_ptr<CDWARFDebugLine> Ptr;

		class CDWARFFileNameEntry
		{
		public:
			CDWARFFileNameEntry() : Name(), DirIdx(0), ModTime(0), Length(0) {}
			std::string				Name;
			unsigned long long int	DirIdx;
			unsigned long long int	ModTime;
			unsigned long long int	Length;
		};
		class CDWARFPrologue 
		{		
		private:
			typedef std::vector<CDWARFFileNameEntry> FilesTable;
		private:
			unsigned int						m_totalLength;
			unsigned short						m_version;
			unsigned int						m_prologueLength;
			unsigned char						m_minInstLength;
			unsigned char						m_defaultIsStmt;
			signed char							m_lineBase;
			unsigned char						m_lineRange;
			unsigned char						m_opcodeBase;
			std::vector<unsigned char>			m_standardOpcodeLengths;
			std::vector<std::string>			m_includeDirectories;
			std::vector<CDWARFFileNameEntry>	m_fileNames;
		public:
			CDWARFPrologue();
			unsigned int getLength() const;
			unsigned int getTotalLength() const;
			unsigned int getStatementTableLength() const;
			signed int getMaxLineIncrementForSpecialOpcode() const;
			void clear();
			void push_file(const CDWARFFileNameEntry& entry);
			unsigned char getOpcodeBase() const;
			unsigned char getMinInstLength() const;
			unsigned char getLineRange() const;
			signed char getLineBase() const;
			unsigned short getVersion() const;
			bool getDefaultIsStmt() const;
			const std::string& file(unsigned int idx) const;
			const std::vector<unsigned char>& getStandardOpcodeLengths() const;
			static CDWARFPrologue ParsePrologue(io::stream_base::ptr stream);
		};
		struct CDWARFRow {
			CDWARFRow() 
				:Address(0), 
				Line(1), 
				Column(0), 
				File(1), 
				Isa(0), 
				IsStmt(0), 
				BasicBlock(0), 
				EndSequence(0), 
				PrologueEnd(0), 
				EpilogueBegin(0)
			{
				return;
			}
			unsigned long long int	Address;
			unsigned int			Line;
			unsigned short			Column;
			unsigned short			File;
			unsigned char			Isa;
			unsigned char			IsStmt:1, BasicBlock:1, EndSequence:1, PrologueEnd:1, EpilogueBegin:1;
			
			inline bool operator < (const CDWARFRow& other) const
			{
				return (Address < other.Address);
			}

			inline void reset(bool default_is_stmt) {
				Address = 0;
				Line = 1;
				Column = 0;
				File = 1;
				Isa = 0;
				IsStmt = default_is_stmt;
				BasicBlock = false;
				EndSequence = false;
				PrologueEnd = false;
				EpilogueBegin = false;
			}
		};
		class CDWARFLineTable
		{
		private:
			CDWARFPrologue				m_prologue;
			std::vector<CDWARFRow>		m_rows;
			unsigned long long int		m_min_addr;
			unsigned long long int		m_max_addr;
			struct SearchRow 
			{
				inline bool operator()(unsigned long long int t2,const CDWARFRow& t1) const  { 
					return (t2 < t1.Address); 
				}
				inline bool operator()(const CDWARFRow& t1,unsigned long long int t2) const  { 
					return (t1.Address < t2); 
				} 
				inline bool operator()(const CDWARFRow& t1,const CDWARFRow& t2) const  { 
					return (t1.Address < t2.Address); 
				} 
			};
		public:
			static const unsigned int kBAD_ROW_IDX;
		public:
			CDWARFLineTable();
			unsigned int lookupAddress(unsigned long long int address, unsigned long long int cu_high_pc) const;
			unsigned long long int minAddr() const;
			unsigned long long int maxAddr() const;
			const CDWARFRow& at(unsigned int) const;
			std::string getFile(const CDWARFRow& row) const;
			std::string getFile(t_uint16 row) const;
			unsigned int count() const;
			bool operator < (const CDWARFLineTable& other) const;
			bool parse(io::stream_base::ptr stream);
			//can receive parameter "resolve" as NULL
			std::string dump(IDWARFNameResolverByAddress* resolver) const;
		};
		struct LineTableNode
		{
			LineTableNode(t_uint64 offset)
				:Offset(offset),
				LineTable()
			{
				return;
			}
			
			bool operator < (const LineTableNode& other) const
			{
				return (LineTable < other.LineTable);
			}

			t_uint64 Offset;
			CDWARFLineTable LineTable;
		};
		struct SearchTable 
		{
			inline bool operator()(unsigned long long int t2,const LineTableNode& t1) const  { 
				return (t2 < t1.LineTable.maxAddr()); 
			}
			inline bool operator()(const LineTableNode& t1,unsigned long long int t2) const  { 
				return (t1.LineTable.maxAddr() < t2); 
			} 
			inline bool operator()(const LineTableNode& t1,const LineTableNode& t2) const  { 
				return (t1.LineTable.maxAddr() < t2.LineTable.maxAddr()); 
			} 
		};
		struct SearchTableByOffset
		{
			SearchTableByOffset(t_uint64 offset)
				:Offset(offset)
			{
				return;
			}
			inline bool operator()(const LineTableNode& table) const  { 
				return (table.Offset == Offset); 
			}
			t_uint64 Offset;
		};
	private:
		typedef std::vector<LineTableNode> MapOfLineTables;
		typedef MapOfLineTables::iterator MapOfLineTableIterator;
		typedef MapOfLineTables::const_iterator MapOfLineTableConstIterator;
	public:
		static const CDWARFLineTable kBAD_LINE_TABLE;
	public:
		CDWARFDebugLine();
		const CDWARFLineTable& getLineTable(const unsigned long long int& offset) const;
		const CDWARFLineTable* getLineTableByOffset(t_uint64 stmt_offset) const;
		const CDWARFLineTable* extractLineTableByOffset(io::stream_base::ptr stream, t_uint64 stmt_offset);
		bool parse(io::stream_base::ptr stream);
		//can receive parameter "resolve" as NULL
		std::string dump(IDWARFNameResolverByAddress* resolve) const;
	private:
		LineTableNode* extract(io::stream_base::ptr stream);
		MapOfLineTableIterator insert(const LineTableNode& node);
	private:
		MapOfLineTables m_table;
	};
}

#endif //__CDWARF_DEBUG_LINE_H__
#ifndef __CMACH_O_H__
#define __CMACH_O_H__

#include <map>
#include <types.h>
#include <memory>
#include <base/base.h>
#include <io/source_of_stream.h>
#include <io/stream_base.h>
#include <dwarf/CDWARFDebugLine.h>
#include <dwarf/CDWARFAbbreviationTable.h>
#include <dwarf/CDWARFCompileUnitTable.h>
#include <utility/CConsole.h>
#include "ECPUTypes.h"
#include "EFileTypes.h"
#include "ECPUSubTypes.h"
#include "ELoadCommands.h"
#include "CSegment.h"
#include "CSymbolTable.h"
#include "CDyLib.h"
#include "MachO.h"

namespace macho
{
	class CMachO : public base::Loadable
	{
	public:
		struct MachHeader 
		{
			t_uint32		magic;		/* mach magic number identifier */
			union {
				t_uint32		d_cputype;
				ECPU_TYPE		cputype;	/* cpu specifier */
			};
			union {
				t_uint32		d_cpusubtype;
				ECPU_SUB_TYPE	cpusubtype;	/* machine specifier */
			};
			union {
				t_uint32		d_filetype;
				EFILE_TYPE		filetype;	/* type of file */
			};
			t_uint32		ncmds;		/* number of load commands */
			t_uint32		sizeofcmds;	/* the size of all the load commands */
			t_uint32		flags;		/* flags */
			t_uint32		reserved;	/* reserved (only for mach_header64)*/

			static base::ERESULT_CODE Read(io::stream_base::ptr stream,MachHeader& res);
		};
	public:
		//x32
		enum { kMACHO_MAGIC = 0xFEEDFACE };
		enum { kMACHO_MAGIC_SWAP = 0xCEFAEDFE };
		//x64
		enum { kMACHO_MAGIC_64 = 0xFEEDFACF };
		enum { kMACHO_MAGIC_SWAP_64 = 0xCFFAEDFE };

		typedef std::shared_ptr<CMachO> ptr;
		typedef std::shared_ptr<CMachO> Ptr;
		typedef std::vector<CDyLib> DyLibTable;
		typedef std::vector< std::pair<t_uint32,t_uint32> > LoadCommandsTable; /*first - type, second - size of command*/

		struct lc_iterator//load command iterator
		{
			lc_iterator(const io::source_of_stream& source, t_uint32 off, t_uint32 cmd_count);
			t_uint32				data_size() const;
			bool					efo() const;
			const lc_iterator&		operator ++(void);
			io::stream_base::ptr&	operator *();

			t_uint32			 mIdx;
			union {
				t_uint32			 mDCmd;
				ELOAD_COMMAND		 mCmd;
			};
			t_uint32			 mCmdSize;
			t_uint32			 mCmdOff;
			t_uint32			 mEfo;
			io::stream_base::ptr mStream;
		};
		struct MachOInfoUnit : public base::Loadable
		{
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOInfoUnit(const io::source_of_stream& source, t_uint32 off);
			ECPU_TYPE						getCpuType();
			EFILE_TYPE						getFileType();
			ECPU_SUB_TYPE					getCpuSubType();
			t_uint32						getCommandOffset();
			t_uint32						getCommandCount();
			t_uint32						getStartOffset() const;
			const io::source_of_stream&		getSource() const;
			io::EENCODING					getEncoding();
		private:
			t_uint32				mStartOff;
			t_uint32				mCmdOff;
			t_uint32				mCmdCount;
			ECPU_TYPE				mCpuType;
			ECPU_SUB_TYPE			mCpuSubType;
			EFILE_TYPE				mFileType;
			io::EENCODING			mEncoding;
			io::source_of_stream	mSource;
		};
		struct MachOUnitUUID : public base::Loadable
		{
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOUnitUUID(MachOInfoUnit& info);
			std::string getUUID();
			bool hasUUID() const;
		private:
			MachOInfoUnit& mInfo;//dependency with other macho unit
			std::string	mUuid;
		};
		struct MachOUnitDependencies : public base::Loadable
		{
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOUnitDependencies(MachOInfoUnit& info);
			DyLibTable& getRequiredDyLibs();
			DyLibTable& getWeakDyLibs();
		private:
			MachOInfoUnit&	mInfo;//dependency with other macho unit
			DyLibTable		mRequiredDyLibs;
			DyLibTable		mWeakDyLibs;
		};
		struct MachOUnitSegments : public base::Loadable
		{
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOUnitSegments(MachOInfoUnit& info);
			t_uint32		getSegmentCount();
			CSegment::Ptr	getSegment(const char* name);
			CSegment::Ptr	getSegmentAt(int idx);
			bool			hasSegment(const char* name);
		private:
			MachOInfoUnit&						mInfo;//dependency with other macho unit
			std::map<std::string,CSegment::Ptr>	mSegments;
		};
		struct MachOUnitSymbolTable : public base::Loadable
		{
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOUnitSymbolTable(MachOInfoUnit& info);
			CSymbolTable&	getSymTab();
			t_uint32		getStringsOff();
			t_uint32		getStringsSize();
		private:
			MachOInfoUnit&	mInfo;//dependency with other macho unit
			CSymbolTable	mSymtab;
			t_uint32		mStringsOff;//offset of strings table
			t_uint32		mStringsSize;//size of string table
		};
		struct MachOUnitDwarf : public base::Loadable
		{
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOUnitDwarf(MachOUnitSegments& segments);
			bool						hasDebugLines();
			dwarf::CDWARFDebugLine::Ptr getDebugLines();
		private:
			MachOUnitSegments&					mSegments;//dependency with other macho unit
			dwarf::CDWARFDebugLine::Ptr			mDebugLines;

		};
		struct MachOUnitLoadCommands : public base::Loadable
		{
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOUnitLoadCommands(MachOInfoUnit& info);
			const LoadCommandsTable&	getCommands();
		private:
			MachOInfoUnit&				mInfo;//dependency with other macho unit
			LoadCommandsTable			mCommands;
		};
		struct MachOUnitMinOSVersion : public base::Loadable
		{
		public:
			enum OS_TYPE { UNKNOWN_OS, MAC_OS, IPHONE_OS };
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOUnitMinOSVersion(MachOInfoUnit& info);
			const bool			hasMinVersion();
			const t_uint32		getVersion();
			const t_uint32		getSdk();
			const std::string	getVersionEx();
			const std::string	getSdkEx();
		private:
			MachOInfoUnit&		mInfo;//dependency with other macho unit
			t_uint32			mVersion;
			t_uint32			mSdk;
			OS_TYPE				mOsType;
		};
		struct MachOUnitCodeSignature : public base::Loadable
		{
		private:
			struct BlobIndex {
				t_uint32 type;					/* type of entry */
				t_uint32 offset;				/* offset of entry */
			};
			struct BlobInfo {
				t_uint32 magic;					/* magic number */
				t_uint32 length;				/* total length of SuperBlob */
				t_uint32 count;					/* number of index entries following */
				/* BlobIndex index[];			(count) entries */
				/* followed by Blobs in no particular order as indicated by offsets in index */
			};
			static const t_uint32 CSMAGIC_REQUIREMENT = 0xfade0c00;		/* single Requirement blob */
			static const t_uint32 CSMAGIC_REQUIREMENTS = 0xfade0c01;		/* Requirements vector (internal requirements) */
			static const t_uint32 CSMAGIC_CODEDIRECTORY = 0xfade0c02;		/* CodeDirectory blob */
			static const t_uint32 CSMAGIC_EMBEDDED_SIGNATURE = 0xfade0cc0; /* embedded form of signature data */
			static const t_uint32 CSMAGIC_EMBEDDED_SIGNATURE_OLD = 0xfade0b02;	/* XXX */
			static const t_uint32 CSMAGIC_DETACHED_SIGNATURE = 0xfade0cc1; /* multi-arch collection of embedded signatures */
			static const t_uint32 CSSLOT_CODEDIRECTORY = 0;				/* slot index for CodeDirectory */
		protected:
			virtual base::ERESULT_CODE _load(ERESERVED_FLAGS flags);
			virtual base::ERESULT_CODE _validate(ERESERVED_FLAGS flags) const;
		public:
			MachOUnitCodeSignature(MachOInfoUnit& info);
		private:
			MachOInfoUnit&				mInfo;//dependency with other macho unit
		};
	private:
		/*****************NEW_MODEL*****************/
		MachOInfoUnit			m_info_unit;
		MachOUnitUUID			m_uuid_unit;
		MachOUnitDependencies	m_dependencies_unit;
		MachOUnitSegments		m_segments_unit;
		MachOUnitSymbolTable	m_symbols_unit;
		MachOUnitDwarf			m_dwarf_unit;
		MachOUnitLoadCommands	m_commands_unit;
		MachOUnitMinOSVersion	m_version_unit;
		MachOUnitCodeSignature	m_code_signature;
		/*****************NEW_MODEL*****************/
	private:
		base::ERESULT_CODE	preload();
	protected:
		/*************base::Loadable**************/
		virtual base::ERESULT_CODE _validate(base::Loadable::ERESERVED_FLAGS flags) const;
		virtual base::ERESULT_CODE _load(base::Loadable::ERESERVED_FLAGS flags);
		/*************base::Loadable**************/
	public:
		CMachO();
		CMachO(const io::source_of_stream& source,macho::t_uint32 offset);
		std::string						getUUID();
		bool							hasUUID() const;
		CSymbolTable&					getSymbolTable();
		t_uint32						getSegmentCount();
		CSegment::Ptr					getSegment(const char* name);
		CSegment::Ptr					getSegmentAt(int idx);
		bool							isExistSegment(const char* name);
		ECPU_TYPE						getTypeOfCPU();
		bool							IsABI64();
		ECPU_SUB_TYPE					getSubTypeOfCPU();
		EFILE_TYPE						getTypeOfFile();
		std::string						getSymbolAt(macho::t_uint32 idx);
		io::stream_base::ptr			loadStringTable();
		bool							hasDebugLines();
		dwarf::CDWARFDebugLine::Ptr		getDebugLines();
		const DyLibTable&				getWeakDyLibs();
		const DyLibTable&				getRequiredDyLibs();
		const LoadCommandsTable&		getLoadCommands();
		std::string						getMinOSVersion();
		std::string						getMinSDKVersion();
		bool							hasMinVersion();
		t_uint8							getAddressByteSize();
		io::EENCODING					getEncoding();

		static std::string Demangle(const char* symbol);
		static std::string FileTypeOF(EFILE_TYPE type);
		static std::string CPUTypeOF(ECPU_TYPE type);
		static std::string CPUSubTypeOF(ECPU_TYPE type,ECPU_SUB_TYPE sub);
	};
}

#endif //__CMACH_O_H__
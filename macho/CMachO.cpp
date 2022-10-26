#include "CMachO.h"
#include <utility/demangler.hpp>

namespace macho
{
	/*
	* CMachO::lc_iterator (Load Command Iterator)
	*/
	CMachO::lc_iterator::lc_iterator(const io::source_of_stream& source, t_uint32 off, t_uint32 cmd_count)
		:mStream(source.open()),
		mIdx(0),
		mDCmd(0),
		mCmdSize(0),
		mCmdOff(off),
		mEfo(cmd_count)
	{
		if(mStream.get() != NULL)
		{
			mStream->seek(mCmdOff);
			mStream->t_read(mDCmd);
			mStream->t_read(mCmdSize);
		}
		return;
	}

	const CMachO::lc_iterator& CMachO::lc_iterator::operator ++(void)
	{
		if(!efo())
		{
			mCmdOff += mCmdSize;
			mStream->seek(mCmdOff);
			mStream->t_read(mDCmd);
			mStream->t_read(mCmdSize);
			++mIdx;
		}
		return *this;
	}
	t_uint32 CMachO::lc_iterator::data_size() const
	{
		return (mCmdSize==0UL)?0UL:(mCmdSize - sizeof(mDCmd) - sizeof(mCmdSize));
	}
	bool CMachO::lc_iterator::efo() const
	{
		return (mStream.get() == NULL || mIdx >= mEfo);
	}
	io::stream_base::ptr& CMachO::lc_iterator::operator *()
	{
		return mStream;
	}
	/*
	* CMachO::MachOInfoUnit
	*/
	CMachO::MachOInfoUnit::MachOInfoUnit(const io::source_of_stream& source, t_uint32 off)
		:mStartOff(off),
		mCmdCount(0),
		mCpuType(ECPU_TYPE_ANY),
		mCpuSubType(ECPU_SUB_TYPE_ALL),
		mFileType(EFILE_TYPE_UNKNOWN),
		mEncoding(io::EENCODING_UNKNOWN),
		mSource(source)
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOInfoUnit::_load(ERESERVED_FLAGS flags)
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		io::stream_base::ptr stream = mSource.open();
		if(stream.get() != NULL)
		{
			stream->seek(mStartOff);
			MachHeader header;
			if(MachHeader::Read(stream,header) == base::ERESULT_CODE_SUCCESS)
			{
				mSource.set_encoding(mEncoding = stream->encoding());
				if(header.ncmds > 0)
				{
					ret_code = base::ERESULT_CODE_SUCCESS;
					mCmdOff = static_cast<t_uint32>(stream->position());
					mCmdCount = header.ncmds;
					mCpuType = header.cputype;
					mCpuSubType = header.cpusubtype;
					mFileType = header.filetype;
				}
			}
		}
		return ret_code;
	}
	base::ERESULT_CODE CMachO::MachOInfoUnit::_validate(ERESERVED_FLAGS flags) const
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		io::stream_base::ptr stream = mSource.open();
		if(stream.get() != NULL)
		{
			stream->seek(mStartOff);
			MachHeader header;
			if(MachHeader::Read(stream,header) == base::ERESULT_CODE_SUCCESS)
			{
				ret_code = (header.ncmds > 0)?base::ERESULT_CODE_SUCCESS:ret_code;
			}
		}
		return ret_code;
	}
	ECPU_TYPE CMachO::MachOInfoUnit::getCpuType()
	{
		load_if_notloaded();
		return mCpuType;
	}
	ECPU_SUB_TYPE CMachO::MachOInfoUnit::getCpuSubType()
	{
		load_if_notloaded();
		return mCpuSubType;
	}
	EFILE_TYPE CMachO::MachOInfoUnit::getFileType()
	{
		load_if_notloaded();
		return mFileType;
	}
	t_uint32 CMachO::MachOInfoUnit::getCommandOffset()
	{
		load_if_notloaded();
		return mCmdOff;
	}
	t_uint32 CMachO::MachOInfoUnit::getCommandCount()
	{
		load_if_notloaded();
		return mCmdCount;
	}
	t_uint32 CMachO::MachOInfoUnit::getStartOffset() const
	{
		return mStartOff;
	}
	io::EENCODING CMachO::MachOInfoUnit::getEncoding()
	{
		load_if_notloaded();
		return mEncoding;
	}
	const io::source_of_stream& CMachO::MachOInfoUnit::getSource() const
	{
		return mSource;
	}
	/*
	* CMachO::MachOUnitUUID
	*/
	CMachO::MachOUnitUUID::MachOUnitUUID(MachOInfoUnit& info)
		:mInfo(info)
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOUnitUUID::_load(ERESERVED_FLAGS flags)
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		lc_iterator it(mInfo.getSource(),mInfo.getCommandOffset(),mInfo.getCommandCount());
		while(!it.efo())
		{
			if(it.mCmd == ELOAD_COMMAND_UUID)
			{
				std::vector<char> buffer(it.data_size());
				mUuid.clear();
				it.mStream->read(&buffer[0],it.data_size());
				const char num[] = "0123456789ABCDEF";
				for(t_uint32 i = 0; i < it.data_size(); ++i)
				{
					unsigned char c = static_cast<unsigned char>(buffer[i]);
					int h = (c>>4);
					int l = (c&0xf);
					mUuid.append(1,num[h]);
					mUuid.append(1,num[l]);
				}
				ret_code = (!mUuid.empty())?base::ERESULT_CODE_SUCCESS:base::ERESULT_CODE_FAIL;
				break;
			}
			++it;
		}
		return ret_code;
	}
	base::ERESULT_CODE CMachO::MachOUnitUUID::_validate(ERESERVED_FLAGS flags) const
	{
		return base::Loadable::_validate(flags);
	}
	std::string CMachO::MachOUnitUUID::getUUID()
	{
		load_if_notloaded();
		return mUuid;
	}
	bool CMachO::MachOUnitUUID::hasUUID() const
	{
		bool ret = false;
		lc_iterator it(mInfo.getSource(),mInfo.getCommandOffset(),mInfo.getCommandCount());
		while(!it.efo())
		{
			if(it.mCmd == ELOAD_COMMAND_UUID)
			{
				ret = true;
				break;
			}
			++it;
		}
		return ret;
	}
	/*
	* CMachO::MachOUnitDependencies
	*/
	CMachO::MachOUnitDependencies::MachOUnitDependencies(MachOInfoUnit& info)
		:mInfo(info)
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOUnitDependencies::_load(ERESERVED_FLAGS flags)
	{
		lc_iterator it(mInfo.getSource(),mInfo.getCommandOffset(),mInfo.getCommandCount());
		while(!it.efo())
		{
			if(it.mCmd == ELOAD_COMMAND_LOAD_DYLIB ||
				it.mCmd == ELOAD_COMMAND_LOAD_WEAK_DYLIB)
			{
				TRACE("[CMachO::MachOUnitDependencies] _load new dependencies");
				enum {kSIZE = 4096};
				char buffer[kSIZE] = {0};
				CDyLib temp;
				t_uint32 name_offset = 0;
				it.mStream->t_read(name_offset);
				it.mStream->t_read(temp.mTimestamp);
				it.mStream->t_read(temp.mCurrentVersion);
				it.mStream->t_read(temp.mCompatibilityVersion);
				it.mStream->seek(it.mCmdOff + name_offset);
				it.mStream->read_sz(buffer,kSIZE);
				temp.mPath = buffer;
				if(it.mCmd == ELOAD_COMMAND_LOAD_DYLIB)
				{
					mRequiredDyLibs.push_back(temp);
				}
				else
				{
					mWeakDyLibs.push_back(temp);
				}
			}
			++it;
		}
		return base::ERESULT_CODE_SUCCESS;
	}
	base::ERESULT_CODE CMachO::MachOUnitDependencies::_validate(ERESERVED_FLAGS flags) const
	{
		return base::Loadable::_validate(flags);
	}
	CMachO::DyLibTable& CMachO::MachOUnitDependencies::getRequiredDyLibs()
	{
		load_if_notloaded();
		return mRequiredDyLibs;
	}
	CMachO::DyLibTable& CMachO::MachOUnitDependencies::getWeakDyLibs()
	{
		load_if_notloaded();
		return mWeakDyLibs;
	}
	/*
	* CMachO::MachOUnitSegments
	*/
	CMachO::MachOUnitSegments::MachOUnitSegments(MachOInfoUnit& info)
		:mInfo(info)
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOUnitSegments::_load(ERESERVED_FLAGS flags)
	{
		lc_iterator it(mInfo.getSource(),mInfo.getCommandOffset(),mInfo.getCommandCount());
		while(!it.efo())
		{
			if(it.mCmd == ELOAD_COMMAND_SEGMENT ||
				it.mCmd == ELOAD_COMMAND_SEGMENT_64)
			{
				t_uint32 offset = static_cast<t_uint32>(it.mStream->position());
				char segname[17] = {0};
				it.mStream->read(segname,sizeof(segname)-1);
				TRACE("[CMachO::MachOUnitSegments] _load new segment of name = %s",segname);
				CSegment::Ptr seg(new CSegment(mInfo.getSource(),offset,mInfo.getStartOffset(),((mInfo.getCpuType()&ECPU_ABI64_FLAG) != 0)));
				mSegments.insert(std::make_pair(segname,seg));
			}
			++it;
		}
		return base::ERESULT_CODE_SUCCESS;
	}
	base::ERESULT_CODE CMachO::MachOUnitSegments::_validate(ERESERVED_FLAGS flags) const
	{
		return base::Loadable::_validate(flags);
	}
	t_uint32 CMachO::MachOUnitSegments::getSegmentCount()
	{
		load_if_notloaded();
		return mSegments.size();
	}
	CSegment::Ptr CMachO::MachOUnitSegments::getSegment(const char* name)
	{
		load_if_notloaded();
		return mSegments[name];
	}
	CSegment::Ptr CMachO::MachOUnitSegments::getSegmentAt(int idx)
	{
		load_if_notloaded();
		int chk = 0;
		for(std::map<std::string,CSegment::Ptr>::const_iterator i = mSegments.begin(), e = mSegments.end();
			i != e;
			++i)
		{
			if(chk == idx)
			{
				return (*i).second;
			}
			++chk;
		}
		return CSegment::Ptr();
	}
	bool CMachO::MachOUnitSegments::hasSegment(const char* name)
	{
		load_if_notloaded();
		return (mSegments.find(name) != mSegments.end());
	}
	/*
	* CMachO::MachOUnitSymbolTable
	*/
	CMachO::MachOUnitSymbolTable::MachOUnitSymbolTable(MachOInfoUnit& info)
		:mInfo(info)
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOUnitSymbolTable::_load(ERESERVED_FLAGS flags)
	{
		lc_iterator it(mInfo.getSource(),mInfo.getCommandOffset(),mInfo.getCommandCount());
		while(!it.efo())
		{
			if(it.mCmd == ELOAD_COMMAND_SYMTAB)
			{
				t_uint32 symoffset, nsyms, stroffset, strsize;
				it.mStream->t_read(symoffset);
				it.mStream->t_read(nsyms);
				it.mStream->t_read(stroffset);
				it.mStream->t_read(strsize);
				mSymtab = CSymbolTable(mInfo.getSource(),(mInfo.getStartOffset() + symoffset),nsyms,((mInfo.getCpuType()&ECPU_ABI64_FLAG) != 0));
				mStringsOff = (mInfo.getStartOffset() + stroffset);
				mStringsSize = strsize;
			}
			++it;
		}
		return base::ERESULT_CODE_SUCCESS;
	}
	base::ERESULT_CODE CMachO::MachOUnitSymbolTable::_validate(ERESERVED_FLAGS flags) const
	{
		return base::Loadable::_validate(flags);
	}
	CSymbolTable& CMachO::MachOUnitSymbolTable::getSymTab()
	{
		load_if_notloaded();
		return mSymtab;
	}
	t_uint32 CMachO::MachOUnitSymbolTable::getStringsOff()
	{
		load_if_notloaded();
		return mStringsOff;
	}
	t_uint32 CMachO::MachOUnitSymbolTable::getStringsSize()
	{
		load_if_notloaded();
		return mStringsSize;
	}
	/*
	* CMachO::MachOUnitDwarf
	*/
	CMachO::MachOUnitDwarf::MachOUnitDwarf(MachOUnitSegments& segments)
		:mSegments(segments),
		mDebugLines()
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOUnitDwarf::_load(ERESERVED_FLAGS flags)
	{
		if(mSegments.hasSegment(CSegment::kSEGMENT_DWARF))
		{
			CSegment::Ptr segment = mSegments.getSegment(CSegment::kSEGMENT_DWARF);
			if(segment.get() != NULL && segment->isExistSection(CSection::kSECTION_DEBUG_LINE))
			{
				CSection::Ptr debug_lines = segment->getSection(CSection::kSECTION_DEBUG_LINE);
				if(debug_lines.get() != NULL)
				{
					io::stream_base::ptr buffer = debug_lines->load();
					mDebugLines.reset(new dwarf::CDWARFDebugLine());
					bool ret = mDebugLines->parse(buffer);
					//assert(ret);
				}
				/*CSection::Ptr sec_abbrev = segment->getSection(macho::CSection::kSECTION_DEBUG_ABBR);
				CSection::Ptr sec_str = segment->getSection(macho::CSection::kSECTION_DEBUG_STR);
				CSection::Ptr sec_info = segment->getSection(macho::CSection::kSECTION_DEBUG_INFO);
				if(sec_info.get() != NULL && sec_abbrev.get() != NULL && sec_str.get() != NULL)
				{
					mDebugAbbrevs.reset(new dwarf::CDWARFAbbreviationTable());
					io::stream_base::ptr buffer = sec_abbrev->load();
					bool ret = mDebugAbbrevs->parse(buffer);
					//assert(ret);

					mDebugStrings = sec_str->load();
					
					mDebugInfo.reset(new dwarf::CDWARFCompileUnitTable());
					buffer = sec_info->load();
					ret = mDebugInfo->parse(*mDebugAbbrevs, mDebugStrings, buffer);
					//assert(ret);
				}*/
			}
		}
		else
		{
			TRACE("[CMachO::MachOUnitDwarf] _load ... dwarf segment not found");
		}
		return base::ERESULT_CODE_SUCCESS;
	}
	base::ERESULT_CODE CMachO::MachOUnitDwarf::_validate(ERESERVED_FLAGS flags) const
	{
		return base::Loadable::_validate(flags);
	}
	bool CMachO::MachOUnitDwarf::hasDebugLines()
	{
		bool ret = false;
		if(mSegments.hasSegment(CSegment::kSEGMENT_DWARF))
		{
			CSegment::Ptr segment = mSegments.getSegment(CSegment::kSEGMENT_DWARF);
			ret = (segment.get() != NULL && segment->isExistSection(CSection::kSECTION_DEBUG_LINE));
		}
		return ret;
	}
	dwarf::CDWARFDebugLine::Ptr CMachO::MachOUnitDwarf::getDebugLines()
	{
		load_if_notloaded();
		return mDebugLines;
	}
	/*
	* CMachO::MachOUnitLoadCommands
	*/
	CMachO::MachOUnitLoadCommands::MachOUnitLoadCommands(MachOInfoUnit& info)
		:mInfo(info)
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOUnitLoadCommands::_load(ERESERVED_FLAGS flags)
	{
		lc_iterator it(mInfo.getSource(),mInfo.getCommandOffset(),mInfo.getCommandCount());
		while(!it.efo())
		{
			mCommands.push_back(std::make_pair(it.mCmd,it.data_size()));
			++it;
		}
		return base::ERESULT_CODE_SUCCESS;
	}
	base::ERESULT_CODE CMachO::MachOUnitLoadCommands::_validate(ERESERVED_FLAGS flags) const
	{
		return base::Loadable::_validate(flags);
	}
	const CMachO::LoadCommandsTable& CMachO::MachOUnitLoadCommands::getCommands()
	{
		load_if_notloaded();
		return mCommands;
	}
	/*
	* CMachO::MachOUnitMinOSVersion
	*/
	CMachO::MachOUnitMinOSVersion::MachOUnitMinOSVersion(MachOInfoUnit& info)
		:mInfo(info),
		mVersion(0),
		mSdk(0),
		mOsType(UNKNOWN_OS)
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOUnitMinOSVersion::_load(ERESERVED_FLAGS flags)
	{
		lc_iterator it(mInfo.getSource(),mInfo.getCommandOffset(),mInfo.getCommandCount());
		while(!it.efo())
		{
			if(it.mCmd == ELOAD_COMMAND_VERSION_MIN_MACOSX ||
				it.mCmd == ELOAD_COMMAND_VERSION_MIN_IPHONE_OS)
			{
				it.mStream->t_read(mVersion);
				it.mStream->t_read(mSdk);
				mOsType = (it.mCmd == ELOAD_COMMAND_VERSION_MIN_MACOSX?(MAC_OS):(IPHONE_OS));
				break;
			}
			++it;
		}
		return base::ERESULT_CODE_SUCCESS;
	}
	base::ERESULT_CODE CMachO::MachOUnitMinOSVersion::_validate(ERESERVED_FLAGS flags) const
	{
		return base::Loadable::_validate(flags);
	}
	const bool CMachO::MachOUnitMinOSVersion::hasMinVersion()
	{
		load_if_notloaded();
		return (mOsType != UNKNOWN_OS);
	}
	const t_uint32 CMachO::MachOUnitMinOSVersion::getVersion()
	{
		load_if_notloaded();
		return mVersion;
	}
	const t_uint32 CMachO::MachOUnitMinOSVersion::getSdk()
	{
		load_if_notloaded();
		return mSdk;
	}
	const std::string CMachO::MachOUnitMinOSVersion::getVersionEx()
	{
		/* EXAMPLE:
		uint32_t	version;	// X.Y.Z is encoded in nibbles xxxx.yy.zz
		uint32_t	sdk;		// X.Y.Z is encoded in nibbles xxxx.yy.zz

		if ( libSystemVers < 0x006F0010 )       // libSystem 111.0.16 in 3.0
		return 0x00020000;		// 2.0  
		else if ( libSystemVers < 0x006F0201 )	// libSystem 111.2.1 in 3.1
		return 0x00030000;		// 3.0  
		else if ( libSystemVers < 0x007D020B )	// libSystem 125.2.11 in 4.0
		return 0x00030100;		// 3.1  
		else if ( libSystemVers < 0x007D0400 )	// libSystem 125.4 in 4.1 and in 4.2
		return 0x00040000;		// 4.0  
		else if ( libSystemVers < 0x009F0000 )	// libSystem 159 in 4.3
		return 0x00040100;		// 4.1  
		else if ( libSystemVers < 0x00A10000 )	// libSystem 161 in 5.0
		return 0x00040300;		// 4.3
		*/
		load_if_notloaded();
		char buffer[64] = {0};
		sprintf_s<sizeof(buffer)>(buffer,"%u.%u.%u",((mVersion&0xFFFF0000)>>16),((mVersion&0x0000FF00)>>8),((mVersion&0x000000FF)));
		return std::string(buffer);
	}
	const std::string CMachO::MachOUnitMinOSVersion::getSdkEx()
	{
		load_if_notloaded();
		char buffer[64] = {0};
		sprintf_s<sizeof(buffer)>(buffer,"%u.%u.%u",((mSdk&0xFFFF0000)>>16),((mSdk&0x0000FF00)>>8),((mSdk&0x000000FF)));
		return std::string(buffer);
	}
	/*
	* CMachO::MachOUnitCodeSignature
	*/
	CMachO::MachOUnitCodeSignature::MachOUnitCodeSignature(MachOInfoUnit& info)
		:mInfo(info)
	{
		return;
	}
	base::ERESULT_CODE CMachO::MachOUnitCodeSignature::_load(ERESERVED_FLAGS flags)
	{
		lc_iterator it(mInfo.getSource(),mInfo.getCommandOffset(),mInfo.getCommandCount());
		while(!it.efo())
		{
			if(it.mCmd == ELOAD_COMMAND_CODE_SIGNATURE)
			{
				linkedit_data_command codesignature;
				it.mStream->t_read(codesignature.dataoff);
				it.mStream->t_read(codesignature.datasize);
				if(codesignature.datasize > 0)
				{
					std::vector<char> bf;
					BlobInfo blob_info;
					bf.resize(codesignature.datasize);
					it.mStream->seek(mInfo.getStartOffset() + codesignature.dataoff);
					it.mStream->enable_big_endian(true);//???????
					it.mStream->t_read(blob_info.magic);
					if(blob_info.magic == CSMAGIC_EMBEDDED_SIGNATURE)
					{
						it.mStream->t_read(blob_info.length);
						it.mStream->t_read(blob_info.count);
					}
				}
				break;
			}
			++it;
		}
		return base::ERESULT_CODE_SUCCESS;
	}
	base::ERESULT_CODE CMachO::MachOUnitCodeSignature::_validate(ERESERVED_FLAGS flags) const
	{
		return base::Loadable::_validate(flags);
	}
	/*
	* CMachO::MachHeader
	*/
	base::ERESULT_CODE CMachO::MachHeader::Read(io::stream_base::ptr stream,MachHeader& res)
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		stream->le_read(res.magic);
		switch(res.magic)
		{
		case CMachO::kMACHO_MAGIC:
		case CMachO::kMACHO_MAGIC_SWAP:
			stream->enable_big_endian(res.magic == kMACHO_MAGIC_SWAP);
			stream->t_read(res.d_cputype);
			stream->t_read(res.d_cpusubtype);
			stream->t_read(res.d_filetype);
			stream->t_read(res.ncmds);
			stream->t_read(res.sizeofcmds);
			stream->t_read(res.flags);
			ret_code = base::ERESULT_CODE_SUCCESS;
			break;
		case CMachO::kMACHO_MAGIC_64:
		case CMachO::kMACHO_MAGIC_SWAP_64:
			stream->enable_big_endian(res.magic == kMACHO_MAGIC_SWAP_64);
			stream->t_read(res.d_cputype);
			stream->t_read(res.d_cpusubtype);
			stream->t_read(res.d_filetype);
			stream->t_read(res.ncmds);
			stream->t_read(res.sizeofcmds);
			stream->t_read(res.flags);	
			stream->t_read(res.reserved);
			ret_code = base::ERESULT_CODE_SUCCESS;
			break;
		default:
			break;
		}
		return ret_code;
	}
	/*
	* CMachO
	*/
	CMachO::CMachO()
		:m_info_unit(io::source_of_stream(),0),
		m_uuid_unit(m_info_unit),
		m_dependencies_unit(m_info_unit),
		m_segments_unit(m_info_unit),
		m_symbols_unit(m_info_unit),
		m_dwarf_unit(m_segments_unit),
		m_commands_unit(m_info_unit),
		m_version_unit(m_info_unit),
		m_code_signature(m_info_unit)
	{
		return;
	}
	CMachO::CMachO(const io::source_of_stream& source,macho::t_uint32 offset)
		:m_info_unit(source, offset),
		m_uuid_unit(m_info_unit),
		m_dependencies_unit(m_info_unit),
		m_segments_unit(m_info_unit),
		m_symbols_unit(m_info_unit),
		m_dwarf_unit(m_segments_unit),
		m_commands_unit(m_info_unit),
		m_version_unit(m_info_unit),
		m_code_signature(m_info_unit)
	{
		return;
	}
	base::ERESULT_CODE CMachO::_load(base::Loadable::ERESERVED_FLAGS flags)
	{
		base::ERESULT_CODE ret_code = base::ERESULT_CODE_FAIL;
		if(base::is_successful(m_info_unit.load(flags)))
		{
			//try load other
			ret_code = base::ERESULT_CODE_SUCCESS;
			m_uuid_unit.load(flags);
			m_dependencies_unit.load(flags);
			m_segments_unit.load(flags);
			m_symbols_unit.load(flags);
			m_dwarf_unit.load(flags);
			m_commands_unit.load(flags);
			m_version_unit.load(flags);
			m_code_signature.load(flags);
		}
		return ret_code;
	}
	base::ERESULT_CODE CMachO::_validate(base::Loadable::ERESERVED_FLAGS flags) const
	{
		return m_info_unit.validate(flags);
	}
	std::string CMachO::getUUID()
	{
		return m_uuid_unit.getUUID();
	}
	bool CMachO::hasUUID() const
	{
		return m_uuid_unit.hasUUID();
	}
	CSymbolTable& CMachO::getSymbolTable()
	{
		return m_symbols_unit.getSymTab();
	}
	t_uint32 CMachO::getSegmentCount()
	{
		return m_segments_unit.getSegmentCount();
	}
	CSegment::Ptr CMachO::getSegment(const char* name)
	{
		return m_segments_unit.getSegment(name);
	}
	CSegment::Ptr CMachO::getSegmentAt(int idx)
	{
		return m_segments_unit.getSegmentAt(idx);
	}
	bool CMachO::isExistSegment(const char* name)
	{
		return m_segments_unit.hasSegment(name);
	}
	ECPU_TYPE CMachO::getTypeOfCPU()
	{
		return m_info_unit.getCpuType();
	}
	bool CMachO::IsABI64()
	{
		return ((getTypeOfCPU()&ECPU_ABI64_FLAG)!=0);
	}
	ECPU_SUB_TYPE CMachO::getSubTypeOfCPU()
	{
		return m_info_unit.getCpuSubType();
	}
	EFILE_TYPE CMachO::getTypeOfFile()
	{
		return m_info_unit.getFileType();
	}
	std::string CMachO::getSymbolAt(macho::t_uint32 idx)
	{
		std::string symbol;
		t_uint32 str_off = m_symbols_unit.getStringsOff();
		io::stream_base::ptr stream = m_info_unit.getSource().open();
		if(stream.get())
		{
			stream->seek(str_off + idx);
			bool next = false;
			do
			{
				char buffer[512] = {0};
				io::bstreamsize readen = stream->read(buffer,sizeof(buffer) - 1);
				symbol.append(buffer, strnlen(buffer, sizeof(buffer) - 1));
				int len = symbol.size();
				next = (readen > 0 && (memchr(buffer,'\0',std::min(sizeof(buffer), (size_t)readen)) == NULL));
			}
			while(next);
		}
		return symbol;
	}
	io::stream_base::ptr CMachO::loadStringTable()
	{
		t_uint32 str_off = m_symbols_unit.getStringsOff();
		t_uint32 str_size = m_symbols_unit.getStringsSize();
		io::stream_base::ptr oret;
		io::stream_base::ptr stream = m_info_unit.getSource().open();
		if(stream.get())
		{
			stream->seek(str_off);
			oret.reset(new io::stream_memory(stream, str_size));
		}
		return oret;
	}
	bool CMachO::hasDebugLines()
	{
		return m_dwarf_unit.hasDebugLines();
	}
	dwarf::CDWARFDebugLine::Ptr CMachO::getDebugLines()
	{
		return m_dwarf_unit.getDebugLines();
	}
	const CMachO::DyLibTable& CMachO::getWeakDyLibs()
	{
		return m_dependencies_unit.getWeakDyLibs();
	}
	const CMachO::DyLibTable& CMachO::getRequiredDyLibs()
	{
		return m_dependencies_unit.getRequiredDyLibs();
	}
	const CMachO::LoadCommandsTable& CMachO::getLoadCommands()
	{
		return m_commands_unit.getCommands();
	}
	bool CMachO::hasMinVersion()
	{
		return m_version_unit.hasMinVersion();
	}
	std::string CMachO::getMinOSVersion()
	{
		return m_version_unit.getVersionEx();
	}
	std::string CMachO::getMinSDKVersion()
	{
		return m_version_unit.getSdkEx();
	}
	t_uint8 CMachO::getAddressByteSize()
	{
		if((m_info_unit.getCpuType() & ECPU_ABI64_FLAG) != 0)
		{
			return 8;
		}
		else
		{
			return 4;
		}
	}
	io::EENCODING CMachO::getEncoding()
	{
		return m_info_unit.getEncoding();
	}
	std::string CMachO::Demangle(const char* symbol)
	{
		return demangle(symbol);
	}
	std::string CMachO::FileTypeOF(EFILE_TYPE type)
	{
		std::string oname;
		switch(type)
		{
		case EFILE_TYPE_UNKNOWN:
			oname = "UNKNOWN";
			break;
		case EFILE_TYPE_OBJECT:
			oname = "OBJECT";
			break;
		case EFILE_TYPE_EXECUTE:
			oname = "EXECUTABLE";
			break;
		case EFILE_TYPE_FVMLIB:
			oname = "FVMLIB";
			break;
		case EFILE_TYPE_CORE:
			oname = "CORE";
			break;
		case EFILE_TYPE_PRELOAD:
			oname = "PRELOAD";
			break;
		case EFILE_TYPE_DYLIB:
			oname = "DYLIB";
			break;
		case EFILE_TYPE_DYLINKER:
			oname = "DYLINKER";
			break;
		case EFILE_TYPE_BUNDLE:
			oname = "BUNDLE";
			break;
		case EFILE_TYPE_DYLIB_STUB:
			oname = "DYLIB_STUB";
			break;
		case EFILE_TYPE_DSYM:
			oname = "DSYM";
			break;
		case EFILE_TYPE_KEXT_BUNDLE:
			oname = "KEXT_BUNDLE";
			break;
		default:
			oname = "UNKNOWN";
			break;
		}
		return oname;
	}
	std::string CMachO::CPUTypeOF(ECPU_TYPE type)
	{
		std::string oname;
		switch(type)
		{
		case ECPU_TYPE_ANY:
			oname = "UNKNOWN";
			break;
		case ECPU_TYPE_VAX:
			oname = "VAX";
			break;
		case ECPU_TYPE_X86:
			oname = "I386";
			break;
		case ECPU_TYPE_X86_64:
			oname = "I386 (64-bit)";
			break;
		case ECPU_TYPE_MC98000:
			oname = "MC98000";
			break;
		case ECPU_TYPE_HPPA:
			oname = "HPPA";
			break;
		case ECPU_TYPE_ARM:
			oname = "ARM (32-bit)";
			break;
		case ECPU_TYPE_ARM_X64:
			oname = "ARM (64-bit)";
			break;
		case ECPU_TYPE_MC88000:
			oname = "MC88000";
			break;
		case CPU_TYPE_SPARC:
			oname = "SPARC";
			break;
		case CPU_TYPE_I860:
			oname = "I860";
			break;
		case CPU_TYPE_POWERPC:
			oname = "POWERPC (32-bit)";
			break;
		case CPU_TYPE_POWERPC64:
			oname = "POWERPC (64-bit)";
			break;
		default:
			oname = "UNKNOWN";
			break;
		}
		return oname;
	}
	std::string CMachO::CPUSubTypeOF(ECPU_TYPE type,ECPU_SUB_TYPE sub)
	{
		std::string oname;
		switch(type)
		{
		case ECPU_TYPE_ARM_X64:
		case ECPU_TYPE_ARM:
			switch(sub)
			{
			case ECPU_SUB_TYPE_ALL:
				oname = "generic";
				break;
			case ECPU_SUB_TYPE_ARM_V4T:
				oname = "armv4T";
				break;
			case ECPU_SUB_TYPE_ARM_V5:
				oname = "armv5";
				break;
			case ECPU_SUB_TYPE_ARM_V6:
				oname = "armv6";
				break;
			case ECPU_SUB_TYPE_ARM_V7:
				oname = "armv7";
				break;
			case ECPU_SUB_TYPE_ARM_V7F:
				oname = "armv7f";
				break;
			case ECPU_SUB_TYPE_ARM_V7S:
				oname = "armv7s";
				break;
			case ECPU_SUB_TYPE_ARM_V7K:
				oname = "armv7k";
				break;
			case ECPU_SUB_TYPE_ARM_V6M:
				oname = "armv6m";
				break;
			case ECPU_SUB_TYPE_ARM_V7M:
				oname = "armv7m";
				break;
			case ECPU_SUB_TYPE_ARM_V7EM:
				oname = "armv7em";
				break;
			default:
				oname = "UNKNOWN";
				break;
			}
			break;
		default:
			oname = "UNKNOWN";
			break;
		}
		return oname;
	}
}
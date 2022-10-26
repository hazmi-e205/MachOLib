#include "CMachOW.h"
#include <utility/demangler.hpp>
#include <utility/crypto/sha/sha.h>
#include <utility/string_tools.hpp>

namespace MachO 
{
		inline std::string ToStr(unsigned long long int p)
		{
			char buffer[16] = {0};
			sprintf_s<sizeof(buffer)>(buffer,"%llu",p);
			return std::string(buffer);
		}

		CMachOW::CMachOW(const macho::CMachO::Ptr& other)
			:m_macho(other),
			m_debug(dwarf::CDWARFAggregator::Ptr(new dwarf::CDWARFAggregator(other)))
		{
		}
        std::wstring CMachOW::GetUUID()
		{
            return std::wstring(m_macho->getUUID().begin(), m_macho->getUUID().end());
		}
        bool CMachOW::HasMinVersion()
		{
			return m_macho->hasMinVersion();
		}
        std::wstring CMachOW::GetMinOSVersion()
		{
            return std::wstring(m_macho->getMinOSVersion().begin(), m_macho->getMinOSVersion().end());
		}
        std::wstring CMachOW::GetMinSDKVersion()
		{
            return std::wstring(m_macho->getMinSDKVersion().begin(), m_macho->getMinSDKVersion().end());
		}
        int32_t CMachOW::FileType()
		{
			return m_macho->getTypeOfFile();
		}
        int32_t CMachOW::CPUType()
		{
			return m_macho->getTypeOfCPU();
		}
        int32_t CMachOW::CPUSubType()
		{
			return m_macho->getSubTypeOfCPU();
		}
        std::wstring CMachOW::GetStrOfFileType(int32_t type)
		{
            auto temp = macho::CMachO::FileTypeOF(static_cast<macho::EFILE_TYPE>(type));
            return std::wstring(temp.begin(), temp.end());
		}
        std::wstring CMachOW::GetStrOfCPUType(int32_t type)
		{
            auto temp = macho::CMachO::CPUTypeOF(static_cast<macho::ECPU_TYPE>(type));
            return std::wstring(temp.begin(), temp.end());
		}
        std::wstring CMachOW::GetStrOfCPUSubType(int32_t type, int32_t sub)
		{
            auto temp = macho::CMachO::CPUSubTypeOF(static_cast<macho::ECPU_TYPE>(type),static_cast<macho::ECPU_SUB_TYPE>(sub));
            return std::wstring(temp.begin(), temp.end());
		}
        std::wstring CMachOW::Demangle(std::wstring indata)
        {
            auto temp = demangle(std::string(indata.begin(), indata.end()).c_str());
            return std::wstring(temp.begin(), temp.end());
		}
        std::list<CDyLibW*> CMachOW::GetRequiredDyLibs()
		{
            std::list<CDyLibW*> libs;
			const macho::CMachO::DyLibTable& table = m_macho->getRequiredDyLibs();
			for(macho::CMachO::DyLibTable::const_iterator i = table.begin(),e = table.end();
				i != e;
				++i)
			{
                libs.push_back(new CDyLibW((*i).mPath,(*i).mTimestamp,(*i).mCurrentVersion,(*i).mCompatibilityVersion));
			}
			return libs;
		}
        std::list<CDyLibW*> CMachOW::GetWeakDyLibs()
		{
            std::list<CDyLibW*> libs;
			const macho::CMachO::DyLibTable& table = m_macho->getWeakDyLibs();
			for(macho::CMachO::DyLibTable::const_iterator i = table.begin(),e = table.end();
				i != e;
				++i)
			{
                libs.push_back(new CDyLibW((*i).mPath,(*i).mTimestamp,(*i).mCurrentVersion,(*i).mCompatibilityVersion));
			}
			return libs;
		}
        std::list<CMachOW::CCmdInf*> CMachOW::GetLoadCommands()
		{
            std::list<CCmdInf*> commands;
			const macho::CMachO::LoadCommandsTable& table = m_macho->getLoadCommands();
			for(macho::CMachO::LoadCommandsTable::const_iterator i = table.begin(),e = table.end();
				i != e;
				++i)
			{
                commands.push_back(new CCmdInf((*i).first,(*i).second));
			}
			return commands;
		}
        CMachOSegmentW* CMachOW::GetSegmentAt(int idx)
		{
            return new CMachOSegmentW(m_macho->getSegmentAt(idx));
		}
		int CMachOW::GetSegmentCount()
		{
			return m_macho->getSegmentCount();
		}
        std::wstring CMachOW::GetInliningInfo(uint64_t address)
		{
            std::wstring inlining_info = L"<invalid>";
			if(m_debug->is_valid() && m_macho->isExistSegment(macho::CSegment::kSEGMENT_TEXT))
			{
				macho::CSegment::Ptr tseg = m_macho->getSegment(macho::CSegment::kSEGMENT_TEXT);
				dwarf::CDWARFAggregator::SubProgramTable subprograms =  m_debug->getInfoByAddress(address + tseg->getVmAddress());
				if(!subprograms.empty())
				{
					std::string result;
					for(size_t i = 0; i < subprograms.size(); ++i)
					{
						result.append(subprograms[i].Name);
						if(!subprograms[i].DeclFile.empty())
						{
							result.append(" (" + subprograms[i].DeclFile + ":" + subprograms[i].CallLineAsString() + ")");
						}
						if(i != (subprograms.size() - 1))
						{
							result.append(" => ");
						}
					}
                    inlining_info = std::wstring(result.begin(), result.end());
				}
			}
			return inlining_info;
		}
        uint64_t CMachOW::GetVMAddress()
		{
            uint64_t addr = 0;
			if(m_macho->isExistSegment(macho::CSegment::kSEGMENT_TEXT))
			{
				macho::CSegment::Ptr segment = m_macho->getSegment(macho::CSegment::kSEGMENT_TEXT);
				addr = segment->getVmAddress();
			}
			return addr;
		}
		void CMachOW::CleanUpInliningInfo()
		{
			m_debug->clean();
		}
        std::wstring CMachOW::GetSHA256BySectionText()
		{
            std::wstring outres = L"";
			if(m_macho->isExistSegment(macho::CSegment::kSEGMENT_TEXT))
			{
				macho::CSegment::Ptr segment = m_macho->getSegment(macho::CSegment::kSEGMENT_TEXT);
				if(segment->isExistSection(macho::CSection::kSECTION_TEXT))
				{
					unsigned char h[32] = {0};
					sha256_context ctx = {0};
					sha256_starts(&ctx);
					macho::CSection::Ptr section = segment->getSection(macho::CSection::kSECTION_TEXT);
					io::stream_base::ptr sectmem = section->load();
					sectmem->seek(0);
					while(sectmem->position() < sectmem->size())
					{
						enum {kSIZE = 4096};
						char buffer[kSIZE] = {0};
						io::bstreamsize readen = sectmem->read(buffer, kSIZE);
						if(readen > 0)
						{
							sha256_update(&ctx, reinterpret_cast<unsigned char*>(&buffer[0]), static_cast<unsigned long>(readen));
						}
						else
						{
							break;
						}
					}
					sha256_finish(&ctx, h);
					std::string hash = bin2hex((const char*)h, sizeof(h));
                    outres = std::wstring(hash.begin(), hash.end());
				}
			}
			return outres;
		}
}

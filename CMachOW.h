#pragma once
#include <dwarf/CDWARFAggregator.h>
#include <utility/CSearchMachO.h>
#include <utility/CConsole.h>
#include "CFATW.h"
#include "CDyLibW.h"
#include "CMachOSegmentW.h"

namespace MachO
{
    class CMachOW
	{
	public:
        class CCmdInf
		{
		public:
			CCmdInf(unsigned int cmd, unsigned int size)
				:Cmd(cmd),
				CmdSize(size)
			{
				return;
			}
            uint32_t Cmd;
            uint32_t CmdSize;
		};
	public:
		CMachOW(const macho::CMachO::Ptr& other);
        int32_t									FileType();
        int32_t									CPUType();
        int32_t									CPUSubType();
        std::wstring							GetUUID();
        bool									HasMinVersion();
        std::wstring									GetMinOSVersion();
        std::wstring									GetMinSDKVersion();
        std::list<CDyLibW*>	GetRequiredDyLibs();
        std::list<CDyLibW*>	GetWeakDyLibs();
        std::list<CCmdInf*>	GetLoadCommands();
        CMachOSegmentW*									GetSegmentAt(int idx);
		int												GetSegmentCount();
        static std::wstring							GetStrOfFileType(int32_t type);
        static std::wstring							GetStrOfCPUType(int32_t type);
        static std::wstring							GetStrOfCPUSubType(int32_t type, int32_t sub);
        static std::wstring							Demangle(std::wstring indata);
        std::wstring									GetInliningInfo(uint64_t address);
        uint64_t									GetVMAddress();
		void											CleanUpInliningInfo();
        std::wstring									GetSHA256BySectionText();
	private:
        std::shared_ptr<macho::CMachO> m_macho;
        std::shared_ptr<dwarf::CDWARFAggregator> m_debug;
	};
}

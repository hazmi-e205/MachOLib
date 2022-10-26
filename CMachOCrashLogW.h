#pragma once
#ifndef __CMACHO_CRASH_LOG_H__
#define __CMACHO_CRASH_LOG_H__
#include <utility/CSearchMachO.h>

namespace MachO
{
    class CMachOCrashLogW
	{
	public:
		enum class eCrashTypeW : int
		{
			eCrashTypeUnknownW,
			eCrashTypeUnknownWithCrashedThreadW,
			eCrashTypeInvalidAddressW,//SIGSEGV (Segmentation fault)
			eCrashTypeAddressNotExistOrNotAlignedW,//SIGBUS (Bus error)
			eCrashTypeInvalidArithmeticOperationW,//SIGFPE (Floating point exception)
			eCrashTypeBrokenPipeW,
			eCrashTypeIllegalInstructionW,//SIGILL
			eCrashTypeBreakPointW,
			/*
			The exception code 0xbaaaaaad indicates that the log is a stackshot of the entire system, not a crash report. To take a stackshot, push the home button and any volume button. Often these logs are accidentally created by users, and do not indicate an error.
			*/
			eCrashTypeExceptionStackshotW,
			/*
			The exception code 0xbad22222 indicates that a VoIP application has been terminated by iOS because it resumed too frequently
			*/
			eCrashTypeVoIPfrequentlyW,
			/*
			The exception code 0x8badf00d indicates that an application has been terminated by iOS because a watchdog timeout occurred. The application took too long to launch, terminate, or respond to system events. One common cause of this is doing synchronous networking on the main thread. Whatever operation is on Thread 0: needs to be moved to a background thread, or processed differently, so that it does not block the main thread.
			*/
			eCrashTypeVoIPWatchdogTimeoutW, 
			/*
			The exception code 0xc00010ff indicates the app was killed by the operating system in response to a thermal event. This may be due to an issue with the particular device that this crash occurred on, or the environment it was operated in. For tips on making your app run more efficiently, see iOS Performance and Power Optimization with Instruments WWDC session.
			*/
			eCrashTypeThermalEventW,
			/*
			The exception code 0xdead10cc indicates that an application has been terminated by iOS because it held on to a system resource (like the address book database) while running in the background.
			*/
			eCrashTypeSystemResourceAccessW,
			/*
			The exception code 0xdeadfa11 indicated that an application has been force quit by the user. Force quits occur when the user first holds down the On/Off button until "slide to power off" appears, then holds down the Home button. It's reasonable to assume that the user has done this because the application has become unresponsive, but it's not guaranteed - force quit will work on any application.
			*/
			eCrashTypeUserForceQuitW,
			eCrashTypeUnhandledExceptionW,
			eCrashTypeAbnormalExitW,//SIGABRT
		};
	public:
		CMachOCrashLogW(const CMachOCrashLog& clog)
		{
			m_crashlog = new CMachOCrashLog(clog);
		}
		~CMachOCrashLogW()
		{
			delete m_crashlog;
			m_crashlog = NULL;
		}
        /*!CMachOCrashLogW()
		{
			delete m_crashlog;
			m_crashlog = NULL;
        }*/
        std::wstring GetName()
		{
            return std::wstring(m_crashlog->name().begin(), m_crashlog->name().end());
		}
        std::wstring GetPath()
		{
            return std::wstring(m_crashlog->path().begin(), m_crashlog->path().end());
		}
        std::wstring GetUUID()
		{
            return std::wstring(m_crashlog->uuid().begin(), m_crashlog->uuid().end());
		}
        std::map<std::wstring,std::wstring> GetLinkMap()
		{
            std::map<std::wstring,std::wstring> map;
			const std::map<std::wstring,std::wstring>& cmap = m_crashlog->links();
			for(std::map<std::wstring,std::wstring>::const_iterator i = cmap.begin(), e = cmap.end();
				i != e;
				++i)
			{
                map[std::wstring((*i).first.begin(), (*i).first.end())] = std::wstring((*i).second.begin(), (*i).second.end());
			}
			return map;
		}
        uint64_t Start()
		{
			return m_crashlog->addr();
		}
        uint64_t End()
		{
			return m_crashlog->end_addr();
		}
        std::wstring GetID()
		{
            return std::wstring(m_crashlog->id().begin(), m_crashlog->id().end());
		}
        std::wstring GetImageName()
		{
            return std::wstring(m_crashlog->image_name().begin(), m_crashlog->image_name().end());
		}
        std::wstring GetContent()
		{
            return std::wstring(m_crashlog->content.begin(), m_crashlog->content.end());
		}
		eCrashTypeW GetType()
		{
			eCrashTypeW ores = (eCrashTypeW)m_crashlog->type();
			return ores;
		}
        bool IsJsonFile()
		{
			return m_crashlog->isJsonFile();
		}
	private:
		CMachOCrashLog* m_crashlog;
	};
}

#endif //__CMACHO_CRASH_LOG_H__

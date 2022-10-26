#ifndef __CCRASH_LOG_APPLICATON_H__
#define __CCRASH_LOG_APPLICATON_H__

#include <string>
#include <map>

class CMachOCrashLog
{
public:
	enum eCrashType
	{
		eCrashTypeUnknown,
		eCrashTypeUnknownWithCrashedThread,
		eCrashTypeInvalidAddress,//SIGSEGV (Segmentation fault)
		eCrashTypeAddressNotExistOrNotAligned,//SIGBUS (Bus error)
		eCrashTypeInvalidArithmeticOperation,//SIGFPE (Floating point exception)
		eCrashTypeBrokenPipe,
		eCrashTypeIllegalInstruction,//SIGILL
		eCrashTypeBreakPoint,
		/*
		The exception code 0xbaaaaaad indicates that the log is a stackshot of the entire system, not a crash report. To take a stackshot, push the home button and any volume button. Often these logs are accidentally created by users, and do not indicate an error.
		*/
		eCrashTypeExceptionStackshot,
		/*
		The exception code 0xbad22222 indicates that a VoIP application has been terminated by iOS because it resumed too frequently
		*/
		eCrashTypeVoIPfrequently,
		/*
		The exception code 0x8badf00d indicates that an application has been terminated by iOS because a watchdog timeout occurred. The application took too long to launch, terminate, or respond to system events. One common cause of this is doing synchronous networking on the main thread. Whatever operation is on Thread 0: needs to be moved to a background thread, or processed differently, so that it does not block the main thread.
		*/
		eCrashTypeVoIPWatchdogTimeout, 
		/*
		The exception code 0xc00010ff indicates the app was killed by the operating system in response to a thermal event. This may be due to an issue with the particular device that this crash occurred on, or the environment it was operated in. For tips on making your app run more efficiently, see iOS Performance and Power Optimization with Instruments WWDC session.
		*/
		eCrashTypeThermalEvent,
		/*
		The exception code 0xdead10cc indicates that an application has been terminated by iOS because it held on to a system resource (like the address book database) while running in the background.
		*/
		eCrashTypeSystemResourceAccess,
		/*
		The exception code 0xdeadfa11 indicated that an application has been force quit by the user. Force quits occur when the user first holds down the On/Off button until "slide to power off" appears, then holds down the Home button. It's reasonable to assume that the user has done this because the application has become unresponsive, but it's not guaranteed - force quit will work on any application.
		*/
		eCrashTypeUserForceQuit,
		eCrashTypeUnhandledException,
		/*
		The process exited abnormally. The most common cause of crashes with this exception type are uncaught Objective-C/C++ exceptions.
		App Extensions will be terminated with this exception type if they take too much time to initialize (a watchdog termination). If an extension is killed due to a hang at launch, the Exception Subtype of the generated crash report will be LAUNCH_HANG. Because extensions do not have a main function, any time spent initializing occurs within static constructors and +load methods present in your extension and dependent libraries. You should defer as much of this work as possible.
		*/
		eCrashTypeAbnormalExit,
	};
	static const wchar_t kTOKEN_BINARY_IMAGES[];
	static const wchar_t kTOKEN_INCIDENT_IDENTIFIER[];
	static const wchar_t kTOKEN_EXCEPTION_TYPE[];
	static const wchar_t kTOKEN_EXCEPTION_CODES[];
	static const wchar_t kTOKEN_Path[];
	static const wchar_t kCRASH_INVALID_ADDRESS[];
	static const wchar_t kCRASH_ABNORMAL_EXIT[];
	static const wchar_t kCRASH_ADDRESS_NOT_EXIST_OR_NOT_ALIGNED[];
	static const wchar_t kCRASH_INVALID_ARITHMETIC_OPERATION[];
	static const wchar_t kCRASH_BROKEN_PIPE[];
	static const wchar_t kCRASH_ILLEGAL_INSTRUCTION[];
	static const wchar_t kCRASH_BREAK_POINT[];
private:
	std::wstring						m_path;
	std::wstring						m_name;
	std::wstring						m_uuid;
	std::wstring						m_id;
	std::wstring						m_report_id;
	std::wstring						m_image_name;
	unsigned long long int				m_img_addr;
	unsigned long long int				m_img_end_addr;
	std::map<std::wstring,std::wstring>	m_linkLibWithUUID;
	eCrashType							m_type;
	bool								m_isJsonFile;
public:
	CMachOCrashLog();
	CMachOCrashLog(const std::wstring& path, const std::wstring& name, const std::wstring& uuid, const std::wstring& id, const std::wstring& report_id,const unsigned long long int& addr, bool isJson);

	std::wstring& path();
	const std::wstring& path() const;
	std::wstring& name();
	const std::wstring& name() const;
	const std::wstring& image_name() const;
	std::wstring& uuid();
	const std::wstring& uuid() const;
	std::wstring& id();
	const std::wstring& id() const;
	std::wstring& report_id();
	const std::wstring& report_id() const;
	unsigned long long int addr() const;
	unsigned long long int end_addr() const;
	const std::map<std::wstring,std::wstring>& links() const;
	eCrashType type() const;
	bool isJsonFile() const;
	bool& isJsonFile();
public:
	std::string content;


	static CMachOCrashLog Parse(const std::wstring& report);
	static CMachOCrashLog ParseJson(const std::wstring& report);
	static bool ParseImagesLine(const wchar_t* report,unsigned long long int& oaddr,unsigned long long int& oend_addr,std::wstring& app_name, std::wstring& app_uuid, std::wstring& oapp_path);
	static eCrashType ParseType(const std::wstring& report);
};

#endif //__CCRASH_LOG_APPLICATON_H__
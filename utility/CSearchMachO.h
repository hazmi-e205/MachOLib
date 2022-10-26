#ifndef __CSEARCH_H__
#define __CSEARCH_H__

#include <string>
#include <vector>
#include <algorithm>
#include <macho/CFAT.h>
#include "CMachOCrashLog.h"
#include "CMachOApplication.h"
#include "CMachODyLib.h"
#include "CMachODSym.h"

class CSearchMachO
{
public:
	static const wchar_t kPOSTFIX_OF_DSYM[];
	static const wchar_t kPOSTFIX_OF_APPLICATION[];
	static const wchar_t kEXTENSION_OF_CRASH_LOG[];
	static const wchar_t kEXTENSION_OF_CRASH_LOG2[];
	static const wchar_t kKEY_WORD1_OF_CRASH_LOG[];
	static const wchar_t kRELATIVE_PATH_OF_MACHO_DSYM[];
	static const wchar_t kEXTENSION_OF_DYLIB[];
public:
	class ISender
	{
	protected:
		virtual ~ISender(){}
	public:
		virtual void onFind(CMachOCrashLog& folder){}
		virtual void onFind(CMachODSym& folder){}
		virtual void onFind(CMachOApplication& folder){}
		virtual void onFind(CMachODyLib& folder){}
		virtual void onCurrentSearchFolder(std::wstring& folder){}
		virtual bool isBreak(){ return false; }
	};
public:
    static bool Search(const wchar_t* dir, ISender& observer);
	static bool ReadCrashLog(const std::wstring& extension,const std::wstring& path,CMachOCrashLog& out_);
	static bool ReadCrashLogJson(const std::wstring& extension,const std::wstring& path,CMachOCrashLog& out_);
	static bool FindMachOInDSYM(const std::wstring& path,CMachODSym& out_);
	static bool FindMachOInDSYMEx(const std::wstring& path,CMachODSym& out_);
	static bool FindMachOInApplication(const std::wstring& path,CMachOApplication& out_);
	static bool FindMachOInApplicationEx(const std::wstring& path,CMachOApplication& out_);
	static bool ReadDyLib(const std::wstring& path,CMachODyLib& out_);
	static bool IsApplication(const std::wstring& dir);
	static bool IsDSYM(const std::wstring& dir);
	static bool IsCrashLog(const std::wstring& path);
	static bool IsDyLib(const std::wstring& path);
};

#endif //__CSEARCH_H__

#include "CSearchMachO.h"
#include "CDirectory.h"
#include "string_tools.hpp"
#include <queue>
#include <io/stream_file.h>
#include <io/source_of_stream.h>
#include <utility/CConsole.h>

const wchar_t CSearchMachO::kPOSTFIX_OF_DSYM[] = L".app.dSYM";
const wchar_t CSearchMachO::kPOSTFIX_OF_APPLICATION[] = L".app";
const wchar_t CSearchMachO::kEXTENSION_OF_CRASH_LOG[] = L".crash";
const wchar_t CSearchMachO::kEXTENSION_OF_CRASH_LOG2[] = L".ips";
const wchar_t CSearchMachO::kKEY_WORD1_OF_CRASH_LOG[] = L"Incident Identifier:";
const wchar_t CSearchMachO::kRELATIVE_PATH_OF_MACHO_DSYM[] = L"\\Contents\\Resources\\DWARF\\";
const wchar_t CSearchMachO::kEXTENSION_OF_DYLIB[] = L".dylib";

bool CSearchMachO::Search(const wchar_t* cpath, ISender& observer)
{
	auto TryReadCrashLog = [](const std::wstring& path, CMachOCrashLog& out_)
	{
		return ReadCrashLog(kEXTENSION_OF_CRASH_LOG, path, out_)
			|| ReadCrashLog(kEXTENSION_OF_CRASH_LOG2, path, out_)
            /*|| ReadCrashLogJson(kEXTENSION_OF_CRASH_LOG, path, out_)
            || ReadCrashLogJson(kEXTENSION_OF_CRASH_LOG2, path, out_)*/;
	};

	unsigned int ret = 0;
	if(cpath)
	{
		std::wstring wpath(cpath), current_folder;
		std::vector<std::wstring> lpath = io::CDirectory::GetLevelsOf(wpath);
		std::queue<std::wstring> to_search;
		std::list<std::wstring> folders;
		std::list<std::wstring> files;

		if((lpath.size() > 1) && IsCrashLog(lpath.back()))
		{
			CMachOCrashLog log;
			if(TryReadCrashLog(wpath, log))
			{
				observer.onFind(log);
				++ret;
			}
		}
		if((lpath.size() > 1) && IsDyLib(lpath.back()))
		{
			CMachODyLib dylib;
			if(ReadDyLib(wpath,dylib))
			{
				observer.onFind(dylib);
				++ret;
			}
		}
		else if((lpath.size() > 1) && IsDSYM(lpath.back()))
		{
			CMachODSym dsym;
			if(FindMachOInDSYM(wpath,dsym))
			{
				observer.onFind(dsym);
				++ret;
			}
		}
		else if((lpath.size() > 1) && IsApplication(lpath.back()))
		{
			CMachOApplication app;
			if(FindMachOInApplication(wpath,app))
			{
				observer.onFind(app);
				++ret;
			}
		}
		else if(io::CDirectory::IsExistDir(wpath))
		{
			to_search.push(wpath);
			while(!to_search.empty() && !observer.isBreak())
			{
				folders.clear();
				files.clear();
				current_folder = to_search.front();
				observer.onCurrentSearchFolder(current_folder);
				to_search.pop();
				io::CDirectory::GetDirList(current_folder,folders);
				io::CDirectory::GetFileList(current_folder,L"",files);

				for(std::list<std::wstring>::iterator i = files.begin(), e = files.end();
					i != e && (!observer.isBreak());
					++i)
				{
					std::wstring path = current_folder + L'\\' + (*i);
					CMachOCrashLog log;
					CMachODyLib dylib;
					if(IsCrashLog(path) && TryReadCrashLog(path, log))
					{
						TRACE("[CSearchMachO] ReadCrashLog path = %s",std::string(log.path().begin(),log.path().end()).c_str());
						observer.onFind(log);
						++ret;
					}
					else if(ReadDyLib(path,dylib))
					{
						TRACE("[CSearchMachO] ReadDyLib path = %s",std::string(dylib.path().begin(),dylib.path().end()).c_str());
						observer.onFind(dylib);
						++ret;
					}
				}
				for(std::list<std::wstring>::iterator i = folders.begin(), e = folders.end();
					i != e && (!observer.isBreak());
					++i)
				{
					if(IsDSYM(*i))
					{
						CMachODSym dsym;
						if(FindMachOInDSYM(current_folder + L'\\' + (*i),dsym))
						{
							TRACE("[CSearchMachO] FindMachOInDSYM path = %s",std::string(dsym.path().begin(),dsym.path().end()).c_str());
							observer.onFind(dsym);
							++ret;
						}
					}
					else if(IsApplication(*i))
					{
						CMachOApplication app;
						if(FindMachOInApplication(current_folder + L'\\' + (*i),app))
						{
							TRACE("[CSearchMachO] FindMachOInApplication path = %s",std::string(app.path().begin(),app.path().end()).c_str());
							observer.onFind(app);
							++ret;
						}
					}
					else
					{
						to_search.push(current_folder + L'\\' + (*i));
					}
				}
			}
		}
	}
	return (ret != 0);
}
bool CSearchMachO::ReadCrashLog(const std::wstring& extension,const std::wstring& path, CMachOCrashLog& out_)
{
	bool ret = false, error = true;
	if(path.find(extension) != std::wstring::npos)
	{
		io::stream_file file(path.c_str(),true,false,error);
		if(!error && (file.size() > 0))
		{
			std::vector<char> buffer(static_cast<unsigned int>(file.size()));
			if(file.read(&buffer[0],file.size()) > 0)
			{
				std::wstring document(buffer.begin(),buffer.end());
				if(document.find(kKEY_WORD1_OF_CRASH_LOG) != std::wstring::npos)
				{
					out_ = CMachOCrashLog::Parse(document);
					std::wstring::size_type name_it = path.find_last_of(L"/\\");
					out_.name() = L"unknown";
					out_.path() = path;
					if(name_it != std::wstring::npos) {
						out_.name() = path.substr(name_it+1);
					}
					ret = true;
				}
			}
			file.close();
		}
	}
	return ret;
}
//bool CSearchMachO::ReadCrashLogJson(const std::wstring& extension, const std::wstring& path, CMachOCrashLog& out_)
//{
//	bool ret = false, error = true;
//	if (path.find(extension) != std::wstring::npos)
//	{
//		io::stream_file file(path.c_str(), true, false, error);
//		if (!error && (file.size() > 0))
//		{
//			std::vector<char> buffer(static_cast<unsigned int>(file.size()));
//			if (file.read(&buffer[0], file.size()) > 0)
//			{
//				std::wstring document(buffer.begin(), buffer.end());
//				if (document.find(L"\"crashReporterKey\"") != std::string::npos)
//				{
//					out_ = CMachOCrashLog::ParseJson(document);
//					std::wstring::size_type name_it = path.find_last_of(L"/\\");
//					out_.name() = L"unknown";
//					out_.path() = path;
//					if (name_it != std::wstring::npos) {
//						out_.name() = path.substr(name_it + 1);
//					}
//					ret = true;
//				}
//			}
//			file.close();
//		}
//	}
//	return ret;
//}
bool CSearchMachO::FindMachOInDSYM(const std::wstring& path,CMachODSym& out_)
{
	bool ret = false;
	std::wstring::size_type it = path.find(kPOSTFIX_OF_DSYM);
	if(it != std::wstring::npos)
	{
		std::wstring before_postfix(path,0,it);
		std::wstring::size_type last_separator = before_postfix.find_last_of(L"/\\");
		std::wstring dsym_name = before_postfix.substr(last_separator+1);
		std::wstring dsym_macho = path + kRELATIVE_PATH_OF_MACHO_DSYM + dsym_name;
		if(io::CDirectory::IsExistFile(dsym_macho))
		{
			out_.setPath(path);
			out_.setName(dsym_name);
			ret = true;
		}
	}
	if(!ret)
	{
		ret = FindMachOInDSYMEx(path, out_);
	}
	return ret;
}
bool CSearchMachO::FindMachOInDSYMEx(const std::wstring& path,CMachODSym& out_)
{
	bool ret = false;
	std::wstring::size_type it = path.find(kPOSTFIX_OF_DSYM);
	if(it != std::wstring::npos)
	{
		std::wstring dsym_macho_dir = path + kRELATIVE_PATH_OF_MACHO_DSYM;
		io::CDirectory::WList files = io::CDirectory::GetFileList(dsym_macho_dir, L"");
		//search to first mach-o file
		for(io::CDirectory::WList::iterator i = files.begin(), e = files.end(); i != e; ++i)
		{
			CMachODSym dSYM;
			dSYM.setPath(path);
			dSYM.setName(*i);
			if(dSYM.fat()->validate() == base::ERESULT_CODE_SUCCESS)
			{
				out_ = dSYM;
				ret = true;
				break;
			}
		}
	}
	return ret;
}
bool CSearchMachO::FindMachOInApplication(const std::wstring& path,CMachOApplication& out_)
{
	bool ret = false;
	std::wstring::size_type it = path.find(kPOSTFIX_OF_APPLICATION);
	if(it != std::wstring::npos)
	{
		std::wstring before_postfix(path,0,it);
		std::wstring::size_type last_separator = before_postfix.find_last_of(L"/\\");
		std::wstring app_name = before_postfix.substr(last_separator+1);
		std::wstring app_macho = path + L'\\' + app_name;
		if(io::CDirectory::IsExistFile(app_macho))
		{
			out_.setName(app_name);
			out_.setPath(path);
			ret = true;
		}
	}
	if(!ret)
	{
		ret = FindMachOInApplicationEx(path, out_);
	}
	return ret;
}
bool CSearchMachO::FindMachOInApplicationEx(const std::wstring& path,CMachOApplication& out_)
{
	bool ret = false;
	std::wstring::size_type it = path.find(kPOSTFIX_OF_APPLICATION);
	if(it != std::wstring::npos)
	{
		std::wstring app_macho_dir = path + L'\\';
		io::CDirectory::WList files = io::CDirectory::GetFileList(app_macho_dir, L"");
		for(io::CDirectory::WList::iterator i = files.begin(), e = files.end(); i != e; ++i)
		{
			CMachOApplication app;
			app.setPath(app_macho_dir);
			app.setName(*i);
			if(app.fat()->validate() == base::ERESULT_CODE_SUCCESS)
			{
				out_ = app;
				ret = true;
				break;
			}
		}
	}
	return ret;
}
bool CSearchMachO::ReadDyLib(const std::wstring& path,CMachODyLib& out_)
{
	bool ret = false, error = true;
	std::wstring::size_type it = path.find(kEXTENSION_OF_DYLIB);
	std::wstring::size_type name_it = path.find_last_of(L"/\\");
	if(name_it != std::wstring::npos ) {
		std::wstring n = path.substr(name_it+1);
		if(n.find(L'.') == std::wstring::npos
			|| it != std::wstring::npos)
		{
			out_.setName(path.substr(name_it+1));
			out_.setPath(path);
			ret = (out_.fat()->validate() == base::ERESULT_CODE_SUCCESS);
		}
	}
	return ret;
}
bool CSearchMachO::IsApplication(const std::wstring& dir)
{
	return has_suffix(dir.c_str(), kPOSTFIX_OF_APPLICATION);
}
bool CSearchMachO::IsDSYM(const std::wstring& dir)
{
	return has_suffix(dir.c_str(), kPOSTFIX_OF_DSYM);
}
bool CSearchMachO::IsCrashLog(const std::wstring& path)
{
	bool ret = false;
	std::wstring::size_type it = path.find_last_of(L"/\\");
	std::wstring name(path);
	if(it != std::wstring::npos && (it + 1) < path.size())
	{
		name = path.substr(it + 1);
	}
	if(has_suffix(name.c_str(), kEXTENSION_OF_CRASH_LOG) || has_suffix(name.c_str(), kEXTENSION_OF_CRASH_LOG2))
	{
		ret = true;
	}
	return ret;
}
bool CSearchMachO::IsDyLib(const std::wstring& path)
{
	bool ret = false;
	std::wstring::size_type it = path.find_last_of(L"/\\");
	std::wstring name(path);
	if(it != std::wstring::npos && (it + 1) < path.size())
	{
		name = path.substr(it + 1);
	}
	if(has_suffix(name.c_str(), kEXTENSION_OF_DYLIB))
	{
		ret = true;
	}
	return ret;
}

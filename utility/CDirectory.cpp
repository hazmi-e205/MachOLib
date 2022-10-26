#include "CDirectory.h"

#ifdef WIN32
#include <windows.h>
#include <algorithm>
#elif OS_IPHONE
#import <Foundation/Foundation.h>
#include <dirent.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <algorithm>
#endif

namespace io
{
	/*********************************************************/
	bool CDirectory::IsExistDir(const std::string &path)
	{
		std::string dir = "\0\\\\?\\";
		dir.append(NormalizeEndOfDir(path));
#ifdef WIN32
		DWORD flags = GetFileAttributesA(dir.c_str());
		return ((flags&FILE_ATTRIBUTE_DIRECTORY)!=0 && flags != INVALID_FILE_ATTRIBUTES);
#elif OS_IPHONE
        return false;
#endif
	}
	/*********************************************************/
	bool CDirectory::IsExistDir(const std::wstring &path)
	{
		std::wstring dir = L"\0\\\\?\\";
		dir.append(NormalizeEndOfDir(path));
#ifdef WIN32
		DWORD flags = GetFileAttributesW(dir.c_str());
		return ((flags&FILE_ATTRIBUTE_DIRECTORY)!=0 && flags != INVALID_FILE_ATTRIBUTES);
#elif OS_IPHONE
        return false;
#endif
	}
	/*********************************************************/
	bool CDirectory::IsExistFile(const std::string &path)
	{
#ifdef WIN32
		std::string spath = "\0\\\\?\\";
		spath.append(path.c_str());
		DWORD flags = GetFileAttributesA(spath.c_str());
		return ((flags&FILE_ATTRIBUTE_DIRECTORY)==0 && flags != INVALID_FILE_ATTRIBUTES);
#elif OS_IPHONE
        return false;
#endif
	}
	/*********************************************************/
	bool CDirectory::IsExistFile(const std::wstring &path)
	{
#ifdef WIN32
		std::wstring spath = L"\0\\\\?\\";
		spath.append(path.c_str());
		DWORD flags = GetFileAttributesW(spath.c_str());
		return ((flags&FILE_ATTRIBUTE_DIRECTORY)==0 && flags != INVALID_FILE_ATTRIBUTES);
#elif OS_IPHONE
        return false;
#endif
	}
	/*********************************************************/
	std::string CDirectory::GetCurrent()
	{
#ifdef WIN32
		enum { kSIZE = 8192 };
		char buffer[kSIZE] = {0};
		GetCurrentDirectoryA(kSIZE,buffer);
		strncat(buffer,"/",1);
		return std::string(buffer);
#elif OS_IPHONE
		char buffer[1024] = {0};
        const char* c_dir = [[[NSBundle mainBundle] resourcePath] UTF8String];
        strncat(buffer,c_dir,strlen(c_dir));
        strncat(buffer,"/",1);
		return std::string(buffer);
#else
        return std::string("");
#endif
	}
	/*********************************************************/
	CDirectory::WList CDirectory::GetDirList(const std::wstring &dir)
	{
		std::list<std::wstring> dirs;
		if(!GetDirList(dir,dirs))
		{
			dirs.clear();
		}
		return WList(dirs.begin(), dirs.end());
	}
	/*********************************************************/
	bool CDirectory::GetDirList(const std::string &rawdir,std::list<std::string> &out_l)
	{
#ifdef WIN32
		WIN32_FIND_DATAA f_data;
		HANDLE h_find = INVALID_HANDLE_VALUE;
		BOOL ret = TRUE;
		std::string tmp, dir("\0\\\\?\\" + NormalizeEndOfDir(rawdir)), search_dir(NormalizeEndOfDir(dir) + '*');
		memset(&f_data,0,sizeof(f_data));

		h_find = FindFirstFileA(reinterpret_cast<LPCSTR>(search_dir.c_str()),&f_data);
		if(h_find == INVALID_HANDLE_VALUE){ return false; }
		while(ret)
		{
			tmp.clear();
			tmp.append(std::string(f_data.cFileName));
			std::string path(dir);
			path+=f_data.cFileName;
			if((GetFileAttributesA(path.c_str())&FILE_ATTRIBUTE_DIRECTORY)!=0 && 
				strlen(f_data.cFileName) > 0 &&
				strcmp(f_data.cFileName,".")!=0 &&
				strcmp(f_data.cFileName,"..")!=0)
			{
				out_l.push_back(tmp);			
			}
			ret = FindNextFileA(h_find,&f_data);
		}
		FindClose(h_find);
		return true;
#elif OS_IPHONE
		/*std::string tmp;
		NSString *nsdir = [NSString stringWithCString:dir.c_str() encoding:NSASCIIStringEncoding];
		NSString *nsextension = [NSString stringWithCString:extension.c_str() encoding:NSASCIIStringEncoding];
		NSArray *nsfiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:nsdir error:nil];
		for(int i=0;i<[nsfiles count];++i)
		{
			if([[nsfiles objectAtIndex:i] hasSuffix:nsextension])
			{
				tmp.clear();
				tmp.append([[nsfiles objectAtIndex:i] UTF8String]);
				out_l.push_back(tmp);
			}
		}*/
		return false;
#else
        return false;
#endif
	}
	/*********************************************************/
	bool CDirectory::GetDirList(const std::wstring &rawdir,std::list<std::wstring> &out_l)
	{
		WIN32_FIND_DATAW f_data;
		HANDLE h_find = INVALID_HANDLE_VALUE;
		BOOL ret = TRUE;
		std::wstring tmp, dir(L"\0\\\\?\\" + NormalizeEndOfDir(rawdir)), search_dir(dir + L'*');
		memset(&f_data,0,sizeof(f_data));

		h_find = FindFirstFileW(reinterpret_cast<LPCWSTR>(search_dir.c_str()),&f_data);
		if(h_find == INVALID_HANDLE_VALUE){ return false; }
		while(ret)
		{
			tmp.clear();
			tmp.append(std::wstring(f_data.cFileName));
			std::wstring path(dir);
			path+=f_data.cFileName;
			if((GetFileAttributesW(path.c_str())&FILE_ATTRIBUTE_DIRECTORY)!=0 && 
				lstrlen(f_data.cFileName) > 0 &&
				lstrcmp(f_data.cFileName,L".")!=0 &&
				lstrcmp(f_data.cFileName,L"..")!=0)
			{
				out_l.push_back(tmp);			
			}
			ret = FindNextFileW(h_find,&f_data);
		}
		FindClose(h_find);
		return true;
	}
	/*********************************************************/
	CDirectory::WList CDirectory::GetFileList(const std::wstring &dir,const std::wstring &extension)
	{
		std::list<std::wstring> files;
		if(!GetFileList(dir, extension, files))
		{
			files.clear();
		}
		return WList(files.begin(), files.end());
	}
	/*********************************************************/
	bool CDirectory::GetFileList(const std::string &rawdir,const std::string &extension,std::list<std::string> &out_l)
	{
#ifdef WIN32
		WIN32_FIND_DATAA f_data;
		HANDLE h_find = INVALID_HANDLE_VALUE;
		BOOL ret = TRUE;
		std::string tmp, dir("\0\\\\?\\" + NormalizeEndOfDir(rawdir)), find_string((extension.empty()?(dir + '*'):(dir + "*." + extension)));
		memset(&f_data,0,sizeof(f_data));

		h_find = FindFirstFileA(reinterpret_cast<LPCSTR>(find_string.c_str()),&f_data);
		if(h_find == INVALID_HANDLE_VALUE){ return false; }
		while(ret)
		{
			tmp.clear();
			tmp.append(std::string(f_data.cFileName));
			std::string path(dir);
			path+=f_data.cFileName;
			if((GetFileAttributesA(path.c_str())&FILE_ATTRIBUTE_DIRECTORY)==0)
			{
				out_l.push_back(tmp);			
			}
			ret = FindNextFileA(h_find,&f_data);
		}
		FindClose(h_find);
		return true;
#elif OS_IPHONE
		std::string tmp;
		NSString *nsdir = [NSString stringWithCString:dir.c_str() encoding:NSASCIIStringEncoding];
		NSString *nsextension = [NSString stringWithCString:extension.c_str() encoding:NSASCIIStringEncoding];
		NSArray *nsfiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:nsdir error:nil];
		for(int i=0;i<[nsfiles count];++i)
		{
			if([[nsfiles objectAtIndex:i] hasSuffix:nsextension])
			{
				tmp.clear();
				tmp.append([[nsfiles objectAtIndex:i] UTF8String]);
				out_l.push_back(tmp);
			}
		}
		return true;
#else
        return false;
#endif
	}
	/*********************************************************/
	bool CDirectory::GetFileList(const std::wstring &rawdir,const std::wstring &extension,std::list<std::wstring> &out_l)
	{
		WIN32_FIND_DATAW f_data;
		HANDLE h_find = INVALID_HANDLE_VALUE;
		BOOL ret = TRUE;
		std::wstring tmp, dir(L"\0\\\\?\\" + NormalizeEndOfDir(rawdir)), find_string(((extension.empty())?(NormalizeEndOfDir(dir) + L'*'):(NormalizeEndOfDir(dir) + L"*." + extension)));
		memset(&f_data,0,sizeof(f_data));

		h_find = FindFirstFileW(reinterpret_cast<LPCWSTR>(find_string.c_str()),&f_data);
		if(h_find == INVALID_HANDLE_VALUE){ return false; }
		while(ret)
		{
			tmp.clear();
			tmp.append(std::wstring(f_data.cFileName));
			std::wstring path(dir);
			path+=f_data.cFileName;
			if((GetFileAttributesW(path.c_str())&FILE_ATTRIBUTE_DIRECTORY)==0)
			{
				out_l.push_back(tmp);			
			}
			ret = FindNextFileW(h_find,&f_data);
		}
		FindClose(h_find);
		return true;
	}
	bool CDirectory::RemoveFile(const std::string &path)
	{
#ifdef  WIN32
		std::string spath = "\0\\\\?\\";
		spath.append(path);
		return (DeleteFileA(spath.c_str()) == TRUE);
#elif   OS_IPHONE
		return (remove(path.c_str()) == 0);
#else
#       error include macros in your project
#endif
	}
	std::vector<std::wstring> CDirectory::GetLevelsOf(const std::wstring &wpath)
	{
		std::vector<std::wstring> level;
		std::wstring::size_type level_it = 0,level_prev = 0;
		/* D:/123/1.txt */
		/* D:/123/ */
		do
		{
			level_it = wpath.find_first_of(L"/\\",level_it);
			if(level_it < wpath.size() && level_it != std::wstring::npos) {
				level.push_back(wpath.substr(level_prev,(level_it - level_prev)));
				level_prev = ++level_it;
			}
			else {
				if(level_prev < wpath.size()) {
					level.push_back(wpath.substr(level_prev,(wpath.size() - level_prev)));
				}
			}
		}
		while(level_it != std::wstring::npos && level_it < wpath.size());
		return level;
	}
	std::wstring CDirectory::MakePathOf(const std::vector<std::wstring> &levels)
	{
		return MakePathOf(levels.begin(),levels.end());
	}
	std::wstring CDirectory::MakePathOf(std::vector<std::wstring>::const_iterator &bg,std::vector<std::wstring>::const_iterator &end)
	{
		std::wstring path;
		if(bg != end)
		{
			std::vector<std::wstring>::const_iterator i = bg, e = (end - 1);
			while(i != e)
			{
				if(!(*i).empty())
				{
					path.append(*i);
#ifdef  WIN32
					path.append(1,'\\');
#elif   OS_IPHONE
					path.append(1,'/');
#endif
				}
				++i;
			}
			path.append(*e);
		}
		return path;
	}
	std::string CDirectory::NormalizePath(const std::string &path)
	{
		std::string temp;
		std::transform(path.begin(),path.end(), temp.begin(), tolower);
		return temp;
	}
	std::wstring CDirectory::NormalizePath(const std::wstring &path)
	{
		std::wstring temp;
		std::transform(path.begin(),path.end(), temp.begin(), tolower);
		return temp;
	}
	int CDirectory::tonormal_path_separator(int c)
	{
		return (c == '/')?'\\':c;
	}
	std::string CDirectory::NormalizeEndOfDir(const std::string& dir)
	{
		std::string odir(dir);
		if(!((*(odir.end() - 1)) == '\\' || (*(odir.end() - 1)) == '/'))
		{
			odir.append(1,'\\');
		}
		return odir;
	}
	std::wstring CDirectory::NormalizeEndOfDir(const std::wstring& dir)
	{
		std::wstring odir(dir);
		if(!((*(odir.end() - 1)) == L'\\' || (*(odir.end() - 1)) == L'/'))
		{
			odir.append(1,L'\\');
		}
		return odir;
	}
}
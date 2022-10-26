#ifndef __CDIRECTORY_H__
#define __CDIRECTORY_H__

#include <list>
#include <string>
#include <vector>

namespace io
{
	class CDirectory
	{
	public:
		typedef std::vector<std::wstring> WList;
	public:
		static std::string GetCurrent();
		static bool IsExistDir(const std::string &path);
		static bool IsExistDir(const std::wstring &path);
		static bool IsExistFile(const std::string &path);
		static bool IsExistFile(const std::wstring &path);
		static WList GetDirList(const std::wstring &dir);
		static bool GetDirList(const std::string &dir,std::list<std::string> &out_l);
		static bool GetDirList(const std::wstring &dir,std::list<std::wstring> &out_l);
		static WList GetFileList(const std::wstring &dir,const std::wstring &extension);
		static bool GetFileList(const std::string &dir,const std::string &extension,std::list<std::string> &out_l);
		static bool GetFileList(const std::wstring &dir,const std::wstring &extension,std::list<std::wstring> &out_l);
		static bool RemoveFile(const std::string &path);
		static std::string NormalizePath(const std::string &path);
		static std::wstring NormalizePath(const std::wstring &path);
		static std::vector<std::wstring> GetLevelsOf(const std::wstring &path);
		static std::wstring MakePathOf(const std::vector<std::wstring> &levels);
        static std::wstring MakePathOf(const std::vector<std::wstring>::const_iterator &bg, const std::vector<std::wstring>::const_iterator &end);
		static int tonormal_path_separator(int c);
		static std::string NormalizeEndOfDir(const std::string& dir);
		static std::wstring NormalizeEndOfDir(const std::wstring& dir);
	};
}
#endif //__CDIRECTORY_H__

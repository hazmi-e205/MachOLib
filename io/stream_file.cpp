#include "stream_file.h"
#include <string>

namespace io
{
	stream_file::stream_file(const char* path,bool readonly,bool create_always,bool &error)
	{
		std::string spath = "\0\\\\?\\";
		spath.append(path);
		DWORD access = (readonly)?GENERIC_READ:(GENERIC_READ | GENERIC_WRITE);
		DWORD creationDisposition = (create_always)?CREATE_ALWAYS:OPEN_EXISTING;
		m_hfile = CreateFileA(spath.c_str(), access, FILE_SHARE_READ,NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		error = (m_hfile == INVALID_HANDLE_VALUE);
	}
	stream_file::stream_file(const wchar_t* path,bool readonly,bool create_always,bool &error)
	{
		std::wstring spath = L"\0\\\\?\\";
		spath.append(path);
		DWORD access = (readonly)?GENERIC_READ:(GENERIC_READ | GENERIC_WRITE);
		DWORD creationDisposition = (create_always)?CREATE_ALWAYS:OPEN_EXISTING;
		m_hfile = CreateFileW(spath.c_str(), access, FILE_SHARE_READ,NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
		error = (m_hfile == INVALID_HANDLE_VALUE);
	}
	stream_file::~stream_file()
	{
		close();
		return ;
	}
	void stream_file::close()
	{
		if(m_hfile != INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(m_hfile);
			CloseHandle(m_hfile);
			m_hfile = INVALID_HANDLE_VALUE;
		}
		return;
	}

	bstreamsize stream_file::size() const
	{
		if(m_hfile != INVALID_HANDLE_VALUE)
		{
			t_uint32 high;
			bstreamsize low = GetFileSize(m_hfile,&high),bighigh = high;
			return ((bighigh << (sizeof(high)*8)) | low);
		}
		else
		{
			return 0;
		}
	}
	bstreamsize stream_file::seek(bstreamsize offset)
	{
		if(m_hfile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER off,ret;
			off.QuadPart = offset;
			ret.QuadPart = 0;
			if(SetFilePointerEx(m_hfile,off,&ret,FILE_BEGIN)) {
				return static_cast<bstreamsize>(ret.QuadPart);
			}
		}
		return 0;
	}
	bstreamsize stream_file::position()
	{
		if(m_hfile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER off,ret;
			off.QuadPart = ret.QuadPart = 0;
			if(SetFilePointerEx(m_hfile,off,&ret,FILE_CURRENT)) {
				return static_cast<bstreamsize>(ret.QuadPart);
			}
		}
		return 0;
	}
	bstreamsize stream_file::read(char* dst,bstreamsize size)
	{
		if(m_hfile != INVALID_HANDLE_VALUE)
		{
			DWORD readen=0;
			BOOL ret = ReadFile(m_hfile,
								reinterpret_cast<LPVOID>(dst),
								(DWORD)size,
								&readen,
								NULL);
			return (ret)?static_cast<bstreamsize>(readen):0;
		}
		else
		{
			return 0;
		}
	}
	bstreamsize stream_file::write(const char* src,bstreamsize size)
	{
		if(m_hfile != INVALID_HANDLE_VALUE)
		{
			DWORD writen=0;
			BOOL ret = WriteFile(m_hfile,
				reinterpret_cast<LPCVOID>(src),
				(DWORD)size,
				&writen,
				NULL);
			return (ret)?static_cast<unsigned int>(writen):0;
		}
		else
		{
			return 0;
		}
	}
}

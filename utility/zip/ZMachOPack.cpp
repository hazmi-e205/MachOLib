#include "ZMachOPack.h"
#include <string>

ZMachOPack::ZMachOPack(const char* path)
	:m_zip(NULL)
{
	m_zip = unzOpen(path);
}
ZMachOPack::~ZMachOPack()
{
	unzClose(m_zip);
}
bool ZMachOPack::is_open() const
{
	return (m_zip != NULL);
}
bool ZMachOPack::exist(const char* fileName)
{
	return (unzLocateFile(m_zip, fileName, 0)==UNZ_OK);
}
std::vector<char> ZMachOPack::extract(const char* fileName)
{
	std::vector<char> oret;
	if(exist(fileName))
	{
		if(unzOpenCurrentFile(m_zip) == UNZ_OK)
		{
			int err = 0;
			enum { kSIZE = 8192 };
			char buf[kSIZE];
			unz_file_info info = {0};
			unzGetCurrentFileInfo(m_zip, &info, NULL, 0, 0, 0, 0, 0);
			oret.reserve(info.uncompressed_size);
			do
			{
				err = unzReadCurrentFile(m_zip, buf, kSIZE);
				if (err < 0)
				{
					oret.clear();
					break;
				}
				if (err > 0)
				{
					int copyto = oret.size();
					oret.resize(oret.size() + err);
					memcpy(&oret[copyto], buf, err);
				}
			}
			while (err>0);
			unzCloseCurrentFile(m_zip);
		}
	}
	return oret;
}
std::vector<std::wstring> ZMachOPack::files()
{
	std::vector<std::wstring> oret;

	if(unzGoToFirstFile(m_zip) == UNZ_OK)
	{
		do
		{
			unz_file_info info = {0};
			char buffer[1024] = {0};
			if(unzGetCurrentFileInfo(m_zip, &info, buffer, 1024, 0, 0, 0, 0) == UNZ_OK)
			{
				oret.push_back(std::wstring(&buffer[0], &buffer[info.size_filename]));
			}
		}while(unzGoToNextFile(m_zip) == UNZ_OK);
	}
	return oret;
}

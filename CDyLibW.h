#pragma once
#include <string>

namespace MachO
{
    class CDyLibW
	{		
	private:
        std::wstring m_path;
        uint32_t m_timestamp;
        uint32_t m_version;
        uint32_t m_compatibility_version;
	public:
		CDyLibW(const std::string& path, unsigned int timestamp,unsigned int version, unsigned int compatibility_version)
		{
            m_path = std::wstring(path.begin(), path.end());
			m_timestamp = timestamp;
			m_version = version;
			m_compatibility_version = compatibility_version;
			return;
		}
        CDyLibW(std::wstring path, uint32_t timestamp, uint32_t version, uint32_t compatibility_version)
		{
			m_path = path;
			m_timestamp = timestamp;
			m_version = version;
			m_compatibility_version = compatibility_version;
			return;
		}
		
        std::wstring GetPath()
		{
			return m_path;
		}
        uint32_t GetTimeStamp()
		{
			return m_timestamp;
		}
        uint32_t GetVersion()
		{
			return m_version;
		}
        uint32_t GetCompatibilityVersion()
		{
			return m_compatibility_version;
		}
	};
}

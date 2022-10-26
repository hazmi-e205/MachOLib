#include "CMachOCrashLog.h"
#include <macho/CFAT.h>
#include <algorithm>
#include <rapidjson/document.h>

const wchar_t CMachOCrashLog::kTOKEN_BINARY_IMAGES[] = L"Binary Images:";
const wchar_t CMachOCrashLog::kTOKEN_INCIDENT_IDENTIFIER[] = L"Incident Identifier: ";
const wchar_t CMachOCrashLog::kTOKEN_EXCEPTION_TYPE[] = L"Exception Type:";
const wchar_t CMachOCrashLog::kTOKEN_EXCEPTION_CODES[] = L"Exception Codes:";
const wchar_t CMachOCrashLog::kTOKEN_Path[] = L"Path:";
const wchar_t CMachOCrashLog::kCRASH_INVALID_ADDRESS[] = L"SIGSEGV";
const wchar_t CMachOCrashLog::kCRASH_ABNORMAL_EXIT[] = L"SIGABRT";
const wchar_t CMachOCrashLog::kCRASH_ADDRESS_NOT_EXIST_OR_NOT_ALIGNED[] = L"SIGBUS";
const wchar_t CMachOCrashLog::kCRASH_INVALID_ARITHMETIC_OPERATION[] = L"SIGFPE";
const wchar_t CMachOCrashLog::kCRASH_BROKEN_PIPE[] = L"SIGPIPE";
const wchar_t CMachOCrashLog::kCRASH_ILLEGAL_INSTRUCTION[] = L"SIGILL";
const wchar_t CMachOCrashLog::kCRASH_BREAK_POINT[] = L"SIGTRAP";

std::wstring& MakeUppercase(std::wstring inout)
{
	std::transform(inout.begin(),inout.end(), inout.begin(),::toupper);
	return inout;
}

std::wstring& DropDashes(std::wstring inout)
{
	inout.erase(std::remove_if(inout.begin(), inout.end(), [](const wchar_t& c){return c == L'-';}), inout.end());
	return inout;
}

template <typename JsonValue>
std::wstring ReadJsonString(JsonValue& v, const wchar_t* k)
{
	if (!v.HasMember(k))
	{
		return std::wstring();
	}
	auto& m = v[k];
	if (!m.IsString())
	{
		return std::wstring();
	}
	std::wstring res(m.GetString(), m.GetStringLength());
	return res;
};

//detection based on the "Exception Type:"
CMachOCrashLog::eCrashType DeduceTypeFromExceptionTypeString(const std::wstring& typeString)
{
	if (typeString.find(CMachOCrashLog::kCRASH_INVALID_ADDRESS) != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeInvalidAddress;
	}
	else if (typeString.find(CMachOCrashLog::kCRASH_ABNORMAL_EXIT) != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeAbnormalExit;
	}
	else if (typeString.find(CMachOCrashLog::kCRASH_ADDRESS_NOT_EXIST_OR_NOT_ALIGNED) != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeAddressNotExistOrNotAligned;
	}
	else if (typeString.find(CMachOCrashLog::kCRASH_BROKEN_PIPE) != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeBrokenPipe;
	}
	else if (typeString.find(CMachOCrashLog::kCRASH_ILLEGAL_INSTRUCTION) != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeIllegalInstruction;
	}
	else if (typeString.find(CMachOCrashLog::kCRASH_BREAK_POINT) != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeBreakPoint;
	}
	return CMachOCrashLog::eCrashTypeUnknown;
}

//detection based on the "Exception Codes:"
//It seems like we can check the ["termination"]["reasons"] in json formatted crashes
CMachOCrashLog::eCrashType DeduceTypeFromExceptionCode(const std::wstring& code)
{
	std::wstring uppercase(code);
	MakeUppercase(uppercase);

	if (code.find(L"8BADF00D") != std::wstring::npos)
	{
		return  CMachOCrashLog::eCrashTypeVoIPWatchdogTimeout;
	}
	else if (code.find(L"BAAAAAAD") != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeExceptionStackshot;
	}
	else if (code.find(L"BAD22222") != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeVoIPfrequently;
	}
	else if (code.find(L"C00010FF") != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeThermalEvent;
	}
	else if (code.find(L"DEAD10CC") != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeSystemResourceAccess;
	}
	else if (code.find(L"DEADFA11") != std::wstring::npos)
	{
		return CMachOCrashLog::eCrashTypeUserForceQuit;
	}
	return CMachOCrashLog::eCrashTypeUnknown;
}

CMachOCrashLog::CMachOCrashLog()
	:m_path(),
	m_name(),
	m_uuid(),
	m_id(),
	m_report_id(),
	m_image_name(),
	m_img_addr(),
	m_img_end_addr(),
	m_type(eCrashTypeUnknown),
	m_isJsonFile(false)
{
}
CMachOCrashLog::CMachOCrashLog(const std::wstring& path, const std::wstring& name, const std::wstring& uuid, const std::wstring& id, const std::wstring& report_id,const unsigned long long int& addr, bool isJson)
	:m_path(path),
	m_name(name),
	m_uuid(uuid),
	m_id(id),
	m_report_id(report_id),
	m_image_name(),
	m_img_addr(addr),
	m_img_end_addr(),
	m_type(eCrashTypeUnknown),
	m_isJsonFile(isJson)
{
}

std::wstring& CMachOCrashLog::path() {
	return m_path;
}
const std::wstring& CMachOCrashLog::path() const {
	return m_path;
}
std::wstring& CMachOCrashLog::name() {
	return m_name;
}
const std::wstring& CMachOCrashLog::name() const {
	return m_name;
}
const std::wstring& CMachOCrashLog::image_name() const {
	return m_image_name;
}
std::wstring& CMachOCrashLog::uuid() {
	return m_uuid;
}
const std::wstring& CMachOCrashLog::uuid() const {
	return m_uuid;
}
std::wstring& CMachOCrashLog::id() {
	return m_id;
}
const std::wstring& CMachOCrashLog::id() const {
	return m_id;
}
std::wstring& CMachOCrashLog::report_id() {
	return m_report_id;
}
const std::wstring& CMachOCrashLog::report_id() const {
	return m_report_id;
}
unsigned long long int CMachOCrashLog::addr() const {
	return m_img_addr;
}
unsigned long long int CMachOCrashLog::end_addr() const {
	return m_img_end_addr;
}
const std::map<std::wstring,std::wstring>& CMachOCrashLog::links() const {
	return m_linkLibWithUUID;
}
CMachOCrashLog::eCrashType CMachOCrashLog::type() const {
	return m_type;
}
bool CMachOCrashLog::isJsonFile() const
{
	return m_isJsonFile;
}
bool& CMachOCrashLog::isJsonFile()
{
	return m_isJsonFile;
}
CMachOCrashLog CMachOCrashLog::ParseJson(const std::wstring& report)
{
	CMachOCrashLog ret;
	rapidjson::MemoryStream ms(reinterpret_cast<const char*>(report.c_str()), report.size()*sizeof(std::wstring::value_type));
	rapidjson::EncodedInputStream<rapidjson::UTF16LE<>, rapidjson::MemoryStream> is(ms);
	
	rapidjson::GenericDocument<rapidjson::UTF16LE<>> document;
	do
	{
		document.ParseStream<rapidjson::kParseStopWhenDoneFlag, rapidjson::UTF16LE<> >(is);
		auto err = document.GetParseError();
		if (err == rapidjson::kParseErrorNone)
		{
			if (!document.HasMember(L"crashReporterKey"))
			{
				continue;
			}
			break;
		}
		else if (err == rapidjson::kParseErrorDocumentRootNotSingular)
		{
			continue;
		}
		else 
		{
			return ret;
		}
	}
	while(true);

	ret.m_isJsonFile = true;

	

	ret.id() = document[L"incident"].GetString();

	ret.id() = ReadJsonString(document, L"incident");
	
	std::wstring procPath = ReadJsonString(document, L"procPath");
	
	/* usedImages example:
	"usedImages" : [
		  {
			"source" : "P",
			"arch" : "arm64",
			"base" : 6859857920,
			"size" : 221184,
			"uuid" : "78e77e28-74d0-371a-a246-6d41374ba19a",
			"path" : "\/usr\/lib\/libobjc.A.dylib",
			"name" : "libobjc.A.dylib"
		  }
	]
	*/	
	auto& images = document[L"usedImages"];
	bool hasMain = false;
	for (auto& image : images.GetArray())
	{
		if (ReadJsonString(image, L"path") == procPath)
		{
			assert(!hasMain);
			hasMain = true;
			ret.m_img_addr = image[L"base"].GetInt64();
			ret.m_img_end_addr = ret.m_img_addr + image[L"size"].GetInt64();
			ret.m_uuid = MakeUppercase(DropDashes(ReadJsonString(image, L"uuid")));
			ret.m_image_name = ReadJsonString(image, L"name");
		}
		else
		{
			std::wstring uuid = MakeUppercase(DropDashes(ReadJsonString(image, L"uuid")));
			std::wstring name = ReadJsonString(image, L"name");
			ret.m_linkLibWithUUID.insert(std::make_pair(name, uuid));
		}
	}
	ret.m_type = eCrashTypeUnknown;

	auto& exceptionInfo = document[L"exception"];
	ret.m_type = DeduceTypeFromExceptionTypeString(MakeUppercase(ReadJsonString(exceptionInfo, L"signal")));
	if (ret.m_type == eCrashTypeUnknown)
	{
		ret.m_type = DeduceTypeFromExceptionTypeString(MakeUppercase(ReadJsonString(exceptionInfo, L"type")));	
	}
	if (ret.m_type == eCrashTypeUnknown)
	{	
		if (document.HasMember(L"termination"))
		{
			auto& terminationInfo = document[L"termination"];
			for (auto& reason : terminationInfo[L"reasons"].GetArray())
			{
				ret.m_type = DeduceTypeFromExceptionCode(reason.GetString());
				if (ret.m_type != eCrashTypeUnknown)
				{
					break;
				}
			}
		}
	}

	// TODO handling for these two cases:
	//Backup deductions
	//if (report.find(L"Last Exception Backtrace:") != std::wstring::npos)
	//{
	//	otype = eCrashTypeUnhandledException;
	//}
	//else if (report.find(L"Crashed:") != std::wstring::npos)
	//{
	//	otype = eCrashTypeUnknownWithCrashedThread;
	//}

	return ret;
}

CMachOCrashLog CMachOCrashLog::Parse(const std::wstring& report)
{
	CMachOCrashLog ret;

	//incident identifier
	{
		std::wstring::size_type id_off = report.find(kTOKEN_INCIDENT_IDENTIFIER);
		if (id_off != std::wstring::npos)
		{
			std::wstring::const_iterator start_it = report.begin() + id_off + lstrlen(kTOKEN_INCIDENT_IDENTIFIER);
			std::wstring::const_iterator first_newline = std::find_if(start_it, report.end(), ::isspace);
			if (first_newline != report.end())
			{
				ret.id() = std::wstring(start_it, first_newline);
			}
		}
	}

	//main image path
	std::wstring path;
	{
		std::wstring::size_type id_path_len = report.find(kTOKEN_Path);
		const std::wstring::size_type path_len = lstrlen(kTOKEN_Path);
		if (id_path_len != std::wstring::npos)
		{
			std::wstring::const_iterator start_it = report.begin() + id_path_len + path_len;
			while (::isspace(*start_it) && start_it != report.end()) start_it++;

			if (start_it != report.end())
			{
				std::wstring::size_type first_newline_off = report.find_first_of(L"\n\r", id_path_len + path_len);
				if (first_newline_off != std::wstring::npos)
				{
					path = std::wstring(start_it, report.begin() + first_newline_off);
				}
			}
		}
	}
	
	//collect infro about the binary images
	std::wstring::size_type it = report.find(kTOKEN_BINARY_IMAGES);
	if(it != std::wstring::npos)
	{
		const int bin_imgs_len = lstrlen(kTOKEN_BINARY_IMAGES);
		//get name of file path
		std::wstring idle;
		std::wstring img_path;
		if(ParseImagesLine(&report[it+bin_imgs_len],ret.m_img_addr, ret.m_img_end_addr,ret.m_image_name,ret.m_uuid, img_path ))
		{
			MakeUppercase(ret.m_uuid);
			do
			{
				it = report.find_first_of(L"\n\r",(it+1));
				if(it != std::wstring::npos && (it+1) < report.size())
				{
					std::wstring ouuid, oname;
					macho::t_uint64 oaddr, oend_addr;
					if(ParseImagesLine(&report[it+1],oaddr, oend_addr,oname,ouuid, img_path ))
					{
						std::wstring upuuid(ouuid.begin(),ouuid.end());
						MakeUppercase(upuuid);
						ret.m_linkLibWithUUID.insert(std::make_pair(oname, upuuid));
						if( !path.empty() && img_path == path )
						{
							ret.m_img_addr = oaddr;
							ret.m_img_end_addr = oend_addr;
							ret.m_image_name = oname;
							ret.m_uuid = upuuid;
						}
					}
				}
			}while(it != std::wstring::npos && (it+1) < report.size());
		}
	}
	ret.m_type = ParseType(report);
	return ret;
}

bool CMachOCrashLog::ParseImagesLine(const wchar_t* report,unsigned long long int& oaddr, unsigned long long int& oend_addr,std::wstring& oapp_name, std::wstring& oapp_uuid, std::wstring& oapp_path)
{
	macho::t_uint64 base_addr,end_addr;
	wchar_t app_name[4096] = {0}, arch_name[32] = {0}, uuid_data[64] = {0}, path_data[4096] = {0};
	int params_filled = swscanf_s(report,L"%llx - %llx %s %s  <%32s> %s",
		&base_addr,
		&end_addr,
		&app_name,_countof(app_name),
		&arch_name,_countof(arch_name),
		&uuid_data,_countof(uuid_data),
		&path_data,_countof(path_data));

	oaddr = base_addr;
	oend_addr = end_addr;
	oapp_name = app_name;
	oapp_uuid = uuid_data;
	oapp_path = path_data;
	return (params_filled == /*num of parameters*/6);
}

CMachOCrashLog::eCrashType CMachOCrashLog::ParseType(const std::wstring& report)
{
	eCrashType otype = eCrashTypeUnknown;
	std::wstring::size_type extype = report.find(kTOKEN_EXCEPTION_TYPE);
	
	if(extype != std::wstring::npos)
	{
		std::wstring::size_type start_of_type = extype + lstrlen(kTOKEN_EXCEPTION_TYPE);
		std::wstring::size_type end_of_type = report.find_first_of(L"\n\r", start_of_type);
		if(start_of_type != std::wstring::npos && end_of_type != std::wstring::npos && start_of_type <= end_of_type)
		{
			std::wstring buffer = report.substr(start_of_type, end_of_type - start_of_type);
			DeduceTypeFromExceptionTypeString(buffer);
			if (otype != eCrashTypeUnknown)
			{
				return otype;
			}
				
			std::wstring::size_type excode = report.find(kTOKEN_EXCEPTION_CODES);
			if (excode != std::wstring::npos)
			{
				std::wstring::size_type start_of_codes = excode + lstrlen(kTOKEN_EXCEPTION_CODES);
				std::wstring::size_type end_of_codes = report.find_first_of(L"\n\r", start_of_type);
				buffer = report.substr(start_of_codes, end_of_codes - start_of_codes);
				DeduceTypeFromExceptionCode(buffer);
			}
			if (otype != eCrashTypeUnknown)
			{
				return otype;
			}

			//Backup deductions
			if (report.find(L"Last Exception Backtrace:") != std::wstring::npos)
			{
				otype = eCrashTypeUnhandledException;
			}
			else if (report.find(L"Crashed:") != std::wstring::npos)
			{
				otype = eCrashTypeUnknownWithCrashedThread;
			}
		}
	}
	return otype;
}

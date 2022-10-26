#include "CMachOCrashLog.h"
#include <macho/CFAT.h>

const wchar_t CMachOCrashLog::kTOKEN_BINARY_IMAGES[] = L"Binary Images:";
const wchar_t CMachOCrashLog::kTOKEN_INCIDENT_IDENTIFIER[] = L"Incident Identifier: ";
const wchar_t CMachOCrashLog::kTOKEN_EXCEPTION_TYPE[] = L"Exception Type:";
const wchar_t CMachOCrashLog::kTOKEN_EXCEPTION_CODES[] = L"Exception Codes:";
const wchar_t CMachOCrashLog::kCRASH_INVALID_ADDRESS[] = L"SIGSEGV";
const wchar_t CMachOCrashLog::kCRASH_ABNORMAL_EXIT[] = L"SIGABRT";
const wchar_t CMachOCrashLog::kCRASH_ADDRESS_NOT_EXIST_OR_NOT_ALIGNED[] = L"SIGBUS";
const wchar_t CMachOCrashLog::kCRASH_INVALID_ARITHMETIC_OPERATION[] = L"SIGFPE";
const wchar_t CMachOCrashLog::kCRASH_BROKEN_PIPE[] = L"SIGPIPE";
const wchar_t CMachOCrashLog::kCRASH_ILLEGAL_INSTRUCTION[] = L"SIGILL";
const wchar_t CMachOCrashLog::kCRASH_BREAK_POINT[] = L"SIGTRAP";

CMachOCrashLog::CMachOCrashLog()
	:m_path(),
	m_name(),
	m_uuid(),
	m_id(),
	m_report_id(),
	m_image_name(),
	m_img_addr(),
	m_img_end_addr(),
	m_type(eCrashTypeUnknown)
{
	return;
}
CMachOCrashLog::CMachOCrashLog(const std::wstring& path, const std::wstring& name, const std::wstring& uuid, const std::wstring& id, const std::wstring& report_id,const unsigned long long int& addr)
	:m_path(path),
	m_name(name),
	m_uuid(uuid),
	m_id(id),
	m_report_id(report_id),
	m_image_name(),
	m_img_addr(addr),
	m_img_end_addr(),
	m_type(eCrashTypeUnknown)
{
	return;
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
CMachOCrashLog CMachOCrashLog::Parse(const std::wstring& report)
{
	CMachOCrashLog ret;
	const int bin_imgs_len = lstrlen(kTOKEN_BINARY_IMAGES);
	std::wstring::size_type id_off = report.find(kTOKEN_INCIDENT_IDENTIFIER);
	std::wstring::size_type it = report.find(kTOKEN_BINARY_IMAGES);
	if(id_off != std::wstring::npos)
	{
		std::wstring::const_iterator start_it = report.begin() + id_off + lstrlen(kTOKEN_INCIDENT_IDENTIFIER);
		std::wstring::const_iterator first_newline = std::find_if(start_it, report.end(), ::isspace);
		if(first_newline != report.end())
		{
			ret.id() = std::wstring(start_it, first_newline);
		}
	}
	if(it != std::wstring::npos)
	{
		//get name of file path
		std::wstring idle;
		if(ParseImagesLine(&report[it+bin_imgs_len],ret.m_img_addr, ret.m_img_end_addr,ret.m_image_name,ret.m_uuid))
		{
			std::transform(ret.m_uuid.begin(),ret.m_uuid.end(), ret.m_uuid.begin(),::toupper);
			do
			{
				it = report.find_first_of(L"\n\r",(it+bin_imgs_len));
				if(it != std::wstring::npos && (it+1) < report.size())
				{
					std::wstring ouuid, oname;
					macho::t_uint64 oaddr, oend_addr;
					if(ParseImagesLine(&report[it+1],oaddr, oend_addr,oname,ouuid))
					{
						std::wstring upuuid(ouuid.begin(),ouuid.end());
						std::transform(upuuid.begin(),upuuid.end(), upuuid.begin(),::toupper);
						ret.m_linkLibWithUUID.insert(std::make_pair(oname, upuuid));
					}
					//
				}
			}while(it != std::wstring::npos && (it+1) < report.size());
		}
	}
	ret.m_type = ParseType(report);
	return ret;
}

bool CMachOCrashLog::ParseImagesLine(const wchar_t* report,unsigned long long int& oaddr, unsigned long long int& oend_addr,std::wstring& oapp_name, std::wstring& oapp_uuid)
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
	return (params_filled == /*num of parameters*/6);
}
CMachOCrashLog::eCrashType CMachOCrashLog::ParseType(const std::wstring& report)
{
	eCrashType otype = eCrashTypeUnknown;
	std::wstring::size_type extype = report.find(kTOKEN_EXCEPTION_TYPE);
	std::wstring::size_type excode = report.find(kTOKEN_EXCEPTION_CODES);
	if(extype != std::wstring::npos)
	{
		std::wstring::size_type start_of_type = extype + lstrlen(kTOKEN_EXCEPTION_TYPE);
		std::wstring::size_type end_of_type = report.find_first_of(L"\n\r", start_of_type);
		if(start_of_type != std::wstring::npos && end_of_type != std::wstring::npos && start_of_type <= end_of_type)
		{
			std::wstring stype = report.substr(start_of_type, end_of_type - start_of_type);
			if(stype.find(kCRASH_INVALID_ADDRESS) != std::wstring::npos)
			{
				otype = eCrashTypeInvalidAddress;
			}
			else if(stype.find(kCRASH_ABNORMAL_EXIT) != std::wstring::npos)
			{
				otype = eCrashTypeAbnormalExit;
			}
			else if(stype.find(kCRASH_ADDRESS_NOT_EXIST_OR_NOT_ALIGNED) != std::wstring::npos)
			{
				otype = eCrashTypeAddressNotExistOrNotAligned;
			}
			else if(stype.find(kCRASH_BROKEN_PIPE) != std::wstring::npos)
			{
				otype = eCrashTypeBrokenPipe;
			}
			else if(stype.find(kCRASH_ILLEGAL_INSTRUCTION) != std::wstring::npos)
			{
				otype = eCrashTypeIllegalInstruction;
			}
			else if(stype.find(kCRASH_BREAK_POINT) != std::wstring::npos)
			{
				otype = eCrashTypeBreakPoint;
			}
			else if(excode != std::wstring::npos)
			{
				std::wstring::size_type start_of_codes = excode + lstrlen(kTOKEN_EXCEPTION_CODES);
				std::wstring::size_type end_of_codes = report.find_first_of(L"\n\r", start_of_type);
				std::wstring scodes = report.substr(start_of_codes, end_of_codes - start_of_codes);
				if(scodes.find(L"8badf00d") != std::wstring::npos)
				{
					otype = eCrashTypeVoIPWatchdogTimeout;
				}
				else if(scodes.find(L"baaaaaad") != std::wstring::npos)
				{
					otype = eCrashTypeExceptionStackshot;
				}
				else if(scodes.find(L"baaaaaad") != std::wstring::npos)
				{
					otype = eCrashTypeExceptionStackshot;
				}
				else if(scodes.find(L"bad22222") != std::wstring::npos)
				{
					otype = eCrashTypeVoIPfrequently;
				}
				else if(scodes.find(L"c00010ff") != std::wstring::npos)
				{
					otype = eCrashTypeThermalEvent;
				}
				else if(scodes.find(L"dead10cc") != std::wstring::npos)
				{
					otype = eCrashTypeSystemResourceAccess;
				}
				else if(scodes.find(L"deadfa11") != std::wstring::npos)
				{
					otype = eCrashTypeUserForceQuit;
				}
				else if(scodes.find(L"deadfa11") != std::wstring::npos)
				{
					otype = eCrashTypeUserForceQuit;
				}
				else if(report.find(L"Last Exception Backtrace:") != std::wstring::npos)
				{
					otype = eCrashTypeUnhandledException;
				}
				else if(report.find(L"Crashed:") != std::wstring::npos)
				{
					otype = eCrashTypeUnknownWithCrashedThread;
				}
			}
		}
	}
	return otype;
}
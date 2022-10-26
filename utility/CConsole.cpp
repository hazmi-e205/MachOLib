#include "CConsole.h"

#ifdef WIN32
#include <windows.h>
#elif OS_IPHONE
#include <Foundation/Foundation.h>
#endif

namespace debug
{
	bool CConsole::s_is_init = false;
	bool CConsole::s_is_allocated_console = false;
	CConsole::subscriber_table CConsole::m_subscribers;

	void CConsole::Init()
	{
		if(!s_is_init)
		{
			s_is_init = true;
#ifdef WIN32
			if(1)
			{
				AllocConsole();
				s_is_allocated_console = true;
			}
			else
			{
				AttachConsole(ATTACH_PARENT_PROCESS);
				s_is_allocated_console = false;
			}
#endif
		}
		return;
	}
	void CConsole::Deinit()
	{
		if(s_is_init && s_is_allocated_console)
		{
#ifdef WIN32
			FreeConsole();
#endif
			s_is_init = false;
			s_is_allocated_console = false;
		}
		return;
	}
	void CConsole::Log(const char *format,...) 
	{
#if ENABLE_CONSOLE
		Init();
#endif
#ifdef  WIN32
		va_list argp;
		char buffer[65535];
		DWORD ret;

		va_start(argp, format);
		vsnprintf(buffer,65535,format, argp);
		for(subscriber_table_const_iterator i = m_subscribers.begin(), e = m_subscribers.end();
			i != e;
			++i)
		{
			(*i)->onMessage(buffer);
		}
		strcat(buffer,"\n");
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE),buffer,strlen(buffer),&ret,NULL);
		va_end(argp);
#elif   OS_IPHONE
        va_list argp;
		char buffer[65535];
        va_start(argp, format);
		vsnprintf(buffer,65535,format, argp);
        NSLog(@"%s",buffer);
        return;
#else
        return;
#endif
	}
	void CConsole::Print(const std::string &text)
	{
		Print(text.c_str());
		return;
	}
	void CConsole::Print(const char *text)
	{
#ifdef WIN32
#	if ENABLE_CONSOLE
		Init();
#	endif
		unsigned int l = strlen(text);
		DWORD ret;
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE),text,l,&ret,NULL);
#endif
		return;
	}
	void CConsole::Print(unsigned int number)
	{
		char buffer[10] = {0};
		sprintf(buffer,"%u",number);
#ifdef WIN32
#	if ENABLE_CONSOLE
		Init();
#	endif
		unsigned int l = strlen(buffer);
		DWORD ret;
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE),buffer,l,&ret,NULL);
#endif
		return;
	}
	void CConsole::AddSubscriber(subscriber* node)
	{
		m_subscribers.push_back(subscriber_proxy());
		m_subscribers.back().mSubscriber = node;
	}
	void CConsole::DeleteAllSubscriber()
	{
		m_subscribers.clear();
	}
	

}//namespace debug
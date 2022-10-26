#ifndef __CCONSOLE_H__
#define __CCONSOLE_H__
#include <string>
#include <vector>

#ifndef NDEBUG
#define ENABLE_CONSOLE 1
#else
#define ENABLE_CONSOLE 0
#endif 

#ifndef NDEBUG
#define TRACE(format,...) do{ debug::CConsole::Log(format,##__VA_ARGS__); } while(0)
#else
#define TRACE(format,...) 
#endif 

namespace debug
{
	class CConsole
	{
	public:
		struct subscriber
		{
			virtual ~subscriber(){}
			virtual void onMessage(const char* msg) = 0;
		};
		struct subscriber_proxy
		{
			subscriber_proxy()
				:mSubscriber(NULL)
			{
				return;
			}
			subscriber_proxy(subscriber* node)
				:mSubscriber(node)
			{
				return;
			}
			~subscriber_proxy()
			{
				delete mSubscriber;
				mSubscriber = NULL;
			}
			subscriber* operator ->() const
			{
				return mSubscriber;
			}
			void onMessage(const char* msg)
			{
				mSubscriber->onMessage(msg);
			}
			subscriber* mSubscriber;
		};
		typedef std::vector<subscriber_proxy>		subscriber_table;
		typedef subscriber_table::iterator			subscriber_table_iterator;
		typedef subscriber_table::const_iterator	subscriber_table_const_iterator;
	public:
		static bool s_is_init;
		static bool s_is_allocated_console;
		static subscriber_table m_subscribers;
	public:
		static void Init();
		static void Deinit();
		static void Log(const char *format,...);
		static void Print(const char *text);
		static void Print(const std::string &text);
		static void	Print(unsigned int number);
		static void AddSubscriber(subscriber* node); 
		static void DeleteAllSubscriber(); 
	};
}//namespace debug

#endif //__CCONSOLE_H__
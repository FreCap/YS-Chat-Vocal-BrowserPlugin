/**********************************************************\

  Auto-generated YouStunAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "YouStun.h"

#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>




#include "TeamTalk4.h"
#include "concurrentQueue.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <tchar.h>
#include <set>
#include <vector>
#include <iostream>
#include <map>


#ifndef H_YouStunAPI
#define H_YouStunAPI



#define COPY_STR(to, from) wcsncpy(to, from, TT_STRLEN)

using namespace std;

enum HotKeyID
{
    HOTKEY_PUSHTOTALK,
    HOTKEY_VOICEACTIVATION,
    HOTKEY_INCVOLUME,
    HOTKEY_DECVOLUME,
    HOTKEY_MUTEALL,
    HOTKEY_MICROPHONEGAIN_INC,
    HOTKEY_MICROPHONEGAIN_DEC,
    HOTKEY_VIDEOTX
};

 
class YouStunAPI : public FB::JSAPIAuto
{
public:
	int id;
    YouStunAPI(const YouStunPtr& plugin, const FB::BrowserHostPtr& host);
    virtual ~YouStunAPI();

    YouStunPtr getPlugin();

    // Read-only property ${PROPERTY.ident}
    std::string get_version();
	

    // Method echo
	FB::variant echo(const FB::variant& msg); 

    void call(const std::string& call_id, const std::string& call_password, const long call_version, const long server, const long port);
	void disconnect();
	std::string isCalling();

    // Event helpers
	FB_JSAPI_EVENT(startedCall, 1, (const std::string&));    
	FB_JSAPI_EVENT(disconnected, 2, (const int, const std::string&));
	FB_JSAPI_EVENT(connectingError, 2, (const int, const std::string&));
	FB_JSAPI_EVENT(connectionStatusChanged, 1, (const int));

	/*
	0 disconnected
	1 connected to the server
	2 logged into the server
	3 joined in the channel
	*/

    //FB_JSAPI_EVENT(fired, 3, (const FB::variant&, bool, int));
    FB_JSAPI_EVENT(echo, 2, (const FB::variant&, const int));
    //FB_JSAPI_EVENT(notify, 0, ());
	

	//TeamTalk

	void TeamTalk_initDevices();
	void TeamTalk_initHotkey();
	void TeamTalk_initConnect(const std::string& server_DNS, const INT32 port, const std::string& nickname, const std::string& serverPassword);

	void TeamTalk_dispatcher(const TTMessage& msg);
	void TeamTalk_channelJoinDefault();                            
	void TeamTalk_channelJoin(const std::string& channel, const std::string& password);
	
	void TeamTalk_worker();
	void TeamTalk_init(const std::string& serverDNS, const INT32 port, const std::string& nickname, const std::string& serverPassword, const std::string& channel, const std::string& channelPassword);

	bool TeamTalk_workerIncrement();
	bool TeamTalk_workerDecrement();
	bool TeamTalk_workerDisconnected();

	bool TeamTalk_workerStatusChanged(int code);

	
	bool TeamTalk_stop(int code);
	bool TeamTalk_stop(int code, const std::string& name);

private:
    YouStunWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    std::string m_testString;
};

#endif // H_YouStunAPI


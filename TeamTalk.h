#pragma once


#include "TeamTalk4.h"
#include "concurrentQueue.h"
#include "YouStunAPI.h"

#include <boost\function.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>

#include <tchar.h>
#include <string>
#include <sstream>

#include <set>
#include <vector>

#include <iostream>
#include <map>

#define COPY_STR(to, from) wcsncpy(to, from, TT_STRLEN)

using namespace std;
/*
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
};*/

class TeamTalk: public YouStunAPI {
public:
	void worker();
	void init(const std::string& serverDNS, const INT32 port, const std::string& nickname, const std::string& serverPassword, const std::string& channel, const std::string& channelPassword);
};
void TeamTalk_initDevices();
void TeamTalk_initHotkey();
void TeamTalk_initConnect(const std::string& server_DNS, const INT32 port, const std::string& nickname, const std::string& serverPassword);

void TeamTalk_dispatcher(const TTMessage& msg);
void TeamTalk_channelJoinDefault();
void TeamTalk_channelJoin(const std::string& channel, const std::string& password);

LPVOID TeamTalk_Instance;
	
boost::mutex TeamTalk_mutexWorker;
int TeamTalk_workingWorker = 0;

boost::mutex TeamTalk_mutexState;
int TeamTalk_state = 0;

bool TeamTalk_workerIncrement();
bool TeamTalk_workerDecrement();

concurrentQueue<boost::function<bool()> > TeamTalk_TTCallbackTasks(1);
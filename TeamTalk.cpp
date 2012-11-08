#include "TeamTalk.h"


bool YouStunAPI::TeamTalk_workerIncrement(){
	boost::mutex::scoped_lock lock(TeamTalk_mutexWorker);
	TeamTalk_workingWorker++;
	return true;
}


bool YouStunAPI::TeamTalk_workerDecrement(){
	boost::mutex::scoped_lock lock(TeamTalk_mutexWorker);
	TeamTalk_workingWorker--;
	if(TeamTalk_state > 0){
		function<bool()> func = boost::bind(&TeamTalk_workerDecrement);
		TeamTalk_TTCallbackTasks.push(func);
	}
	return false;
}

void YouStunAPI::TeamTalk_worker(){

	TeamTalk_workerIncrement();
	boost::function<bool()> func;

    while(TeamTalk_TTCallbackTasks.wait_and_pop(func)){
        if(!func()){
			break;
		}	
    }

}

void YouStunAPI::TeamTalk_stop(){
	boost::mutex::scoped_lock lock(TeamTalk_mutexState);
	if(TeamTalk_state > 0){
		function<bool()> func = boost::bind(&TeamTalk_workerDecrement);
		TeamTalk_TTCallbackTasks.push(func);
		TT_CloseTeamTalk(TeamTalk_Instance);
		TeamTalk_state = 0;
	}
}

void YouStunAPI::TeamTalk_init(const std::string& serverDNS, const INT32 port, const std::string& nickname, const std::string& serverPassword, const std::string& channel, const std::string& channelPassword){

	
	boost::mutex::scoped_lock lock(TeamTalk_mutexState);

	boost::thread t1(boost::bind(&TeamTalk::worker, this));
	boost::thread t2(boost::bind(&TeamTalk::worker, this));
	boost::thread t3(boost::bind(&TeamTalk::worker, this));

	TeamTalk_Instance = TT_InitTeamTalkPoll();

	TeamTalk_state = 1;

	TeamTalk_initDevices();

	//connect
	TeamTalk_initConnect(serverDNS, port, nickname, serverPassword);

	TeamTalk_initHotkey();
	
	//join
	TeamTalk_channelJoin(channel, channelPassword);

	lock.unlock();

	TTMessage msg;
    while(TT_GetMessage(TeamTalk_Instance, &msg, NULL))
        TeamTalk_dispatcher(msg);

}

void YouStunAPI::TeamTalk_initConnect(const std::string& serverDNS, const INT32 port, const std::string& nickname, const std::string& serverPassword){

	std::wstring serverDNSW, nicknameW, serverPasswordW;
	
	serverDNSW.assign(serverDNS.begin(),serverDNS.end());
	nicknameW.assign(nickname.begin(),nickname.end());
	serverPasswordW.assign(serverPassword.begin(),serverPassword.end());

	BOOL b = TT_Connect(TeamTalk_Instance, serverDNSW.c_str(), port, port, 0, 0);
	
	if(!b){
		//TODO de-init all
	}

	TTMessage msg;
	int wait_ms = 10000;
    if(!TT_GetMessage(TeamTalk_Instance, &msg, &wait_ms) || msg.wmMsg == WM_TEAMTALK_CON_FAILED || msg.wmMsg != WM_TEAMTALK_CON_SUCCESS){
		//TODO error
	}
       
    if(!(TT_GetFlags(TeamTalk_Instance) & CLIENT_CONNECTED)){
		//TODO error
	}

   // login
   // int cmd_id = TT_DoLogin(TeamTalk_Instance, _T("nickname"), _T("ServerPassword"), _T("username"), _T("password"));
	int cmd_id = TT_DoLogin(TeamTalk_Instance, nicknameW.c_str(), serverPasswordW.c_str(), _T(""), _T(""));
    if(cmd_id < 0){
		//TODO error
	}

    //wait for server reply
    if(!TT_GetMessage(TeamTalk_Instance, &msg, &wait_ms) ||
        msg.wmMsg != WM_TEAMTALK_CMD_PROCESSING){
		//TODO error
	}
        
    //get response
    if(!TT_GetMessage(TeamTalk_Instance, &msg, &wait_ms) ||
        msg.wmMsg == WM_TEAMTALK_CMD_ERROR){
		//TODO error
	}

    //wait for login command to complete
    //client will now post all the server information
    while(TT_GetMessage(TeamTalk_Instance, &msg, &wait_ms) && msg.wmMsg != WM_TEAMTALK_CMD_PROCESSING)
    {
        TeamTalk_dispatcher(msg);
    }
    if(msg.lParam != TRUE){ //command processing complete
		//TODO error
	}
	
	if(!(TT_GetFlags(TeamTalk_Instance) & CLIENT_AUTHORIZED)){
		//TODO error
	}

}

void YouStunAPI::TeamTalk_initHotkey(){

	// ctrl
	int key = 162;
	TT_HotKey_Register(TeamTalk_Instance, HOTKEY_PUSHTOTALK, &key, 1);

}

void YouStunAPI::TeamTalk_initDevices(){

	// GET & SET DEVICES input e output
	SoundSystem nSoundSys = SOUNDSYSTEM_DSOUND;

	vector<SoundDevice> devs;
	
	map<WCHAR*,int> mapInput;
	map<WCHAR*,int> mapOutut;

	int nCount = 0;
    TT_GetSoundInputDevices(TeamTalk_Instance, NULL, &nCount);
    if(nCount)
    {
        devs.resize(nCount);
        TT_GetSoundInputDevices(TeamTalk_Instance, &devs[0], &nCount);        
    }
	
	for(int i=0;i<nCount;i++)
	{
        if(devs[i].nSoundSystem != nSoundSys)
            continue;
		mapInput.insert( pair<WCHAR*,int>(devs[i].szDeviceName, devs[i].nDeviceID));
	}


	//Get output devices
	TT_GetSoundOutputDevices(TeamTalk_Instance, NULL, &nCount);
    if(nCount)
    {
        devs.resize(nCount);
        TT_GetSoundOutputDevices(TeamTalk_Instance, &devs[0], &nCount);        
    }

	for(int i=0;i<nCount;i++)
	{
        if(devs[i].nSoundSystem != nSoundSys)
            continue;
		mapOutut.insert( pair<WCHAR*,int>(devs[i].szDeviceName, devs[i].nDeviceID));
	}

	BOOL bSuccess = FALSE;
    bSuccess = TT_InitSoundInputDevice(TeamTalk_Instance, (*mapInput.begin()).second);
    bSuccess &= TT_InitSoundOutputDevice(TeamTalk_Instance, (*mapOutut.begin()).second);

	if(!bSuccess)
    {
        TT_CloseSoundInputDevice(TeamTalk_Instance);
        TT_CloseSoundOutputDevice(TeamTalk_Instance);
	}

}

void YouStunAPI::TeamTalk_dispatcher(const TTMessage& msg)
{
    Channel chan = {0};
    User user = {0};
    ServerProperties srvprop = {0};
    FileInfo file = {0};
    TTCHAR buf[TT_STRLEN] = {0};
	int active;
    switch(msg.wmMsg)
    {
	case WM_TEAMTALK_HOTKEY :

		active = msg.lParam;
		switch(msg.wParam){
		case HOTKEY_PUSHTOTALK :
			TT_EnableTransmission(TeamTalk_Instance, TRANSMIT_AUDIO, active);
			break;
		case HOTKEY_VOICEACTIVATION :
			if(active)
				TT_EnableVoiceActivation(TeamTalk_Instance, 
						 !(TT_GetFlags(TeamTalk_Instance) & CLIENT_SNDINPUT_VOICEACTIVATED));
			break;
		case HOTKEY_MUTEALL :
			if(active)
				TT_SetSoundOutputMute(TeamTalk_Instance, 
									  !(TT_GetFlags(TeamTalk_Instance) & CLIENT_SNDOUTPUT_MUTE));
			break;
		/*case HOTKEY_INCVOLUME :
			if(active)
				ui.volumeSlider->setValue(ui.volumeSlider->value()+15);
			break;
		case HOTKEY_DECVOLUME :
			if(active)
				ui.volumeSlider->setValue(ui.volumeSlider->value()-15);
			break;
		case HOTKEY_MICROPHONEGAIN_INC :
			if(active)
				ui.micSlider->setValue(TT_GetSoundInputGainLevel(TeamTalk_Instance)+200);
			break;
		case HOTKEY_MICROPHONEGAIN_DEC :
			if(active)
				ui.micSlider->setValue(TT_GetSoundInputGainLevel(TeamTalk_Instance)-200);
			break;*/
		}
		cout << "hotkey message #" << msg.wmMsg << " wParam " << msg.wParam << " lParam " << msg.lParam << endl;
		break;
    case WM_TEAMTALK_CMD_MYSELF_LOGGEDIN :
        cout << "Logged in successfully..." << endl;
        cout << "Got user ID #" << msg.wParam << endl;
        break;
    case WM_TEAMTALK_CMD_CHANNEL_REMOVE :
        //cannot retrieve channel since it no longer exists.
        cout << "Removed channel #" << msg.wParam << endl;
        break;

    case WM_TEAMTALK_CMD_USER_JOINED :
        if(TT_GetUser(TeamTalk_Instance, msg.wParam, &user) && 
           TT_GetChannelPath(TeamTalk_Instance, msg.lParam, buf))
        {
            cout << "User #" << user.nUserID << " " << user.szNickname << " joined " << buf << endl;
            int cmd_id = TT_DoSubscribe(TeamTalk_Instance, user.nUserID, 
                                        SUBSCRIBE_INTERCEPT_USER_MSG | SUBSCRIBE_INTERCEPT_CHANNEL_MSG |
                                        SUBSCRIBE_INTERCEPT_AUDIO);
            if(cmd_id>0)
                cout << "Subscribing to text and audio events from #" << user.nUserID << endl;
            else
                cout << "Failed to issue subscribe command" << endl;
        }
        break;
    case WM_TEAMTALK_CMD_USER_LEFT : 
        if(TT_GetUser(TeamTalk_Instance, msg.wParam, &user) && 
           TT_GetChannelPath(TeamTalk_Instance, msg.lParam, buf))
            cout << "User #" << user.nUserID << " " << user.szNickname << " left " << buf << endl;
        break;
    default :
        cout << "Unknown message #" << msg.wmMsg << " wParam " << msg.wParam << " lParam " << msg.lParam << endl;
    }
}

void YouStunAPI::TeamTalk_channelJoinDefault(){
	
	int cmdid = TT_DoJoinChannelByID(TeamTalk_Instance, 
                        TT_GetRootChannelID(TeamTalk_Instance), 
                        _T(""));
	cout << "Test#" << cmdid << endl;

}

void YouStunAPI::TeamTalk_channelJoin(const std::string& channelPre, const std::string& password){

	std::wstring channelW, passwordW;
	std::string channel;

	std::string slash = "/";

	channel = slash + channelPre;

	channelW.assign(channel.begin(), channel.end());
	passwordW.assign(password.begin(), password.end());

	int cmdid = TT_DoJoinChannelByID(TeamTalk_Instance, 
		TT_GetChannelIDFromPath(TeamTalk_Instance, channelW.c_str()),
		passwordW.c_str());
	cout << "Test#" << cmdid << endl;

}
/**********************************************************\

  Auto-generated YouStunAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "YouStunAPI.h"


LPVOID TeamTalk_Instance;


boost::mutex mutexIdAutoincrement;
int idAutoincrement = 0;
int idCaller = 0;
string call_idActive;

boost::mutex TeamTalk_mutexWorker;
int TeamTalk_workingWorker = 0;

boost::mutex TeamTalk_mutexState;
int TeamTalk_state = 0;

concurrentQueue<boost::function<bool()> > TeamTalk_TTCallbackTasks(1);



///////////////////////////////////////////////////////////////////////////////
/// @fn YouStunAPI::YouStunAPI(const YouStunPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
YouStunAPI::YouStunAPI(const YouStunPtr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host)
{


    registerMethod("call",      make_method(this, &YouStunAPI::call));
    registerMethod("disconnect",      make_method(this, &YouStunAPI::disconnect));
    registerMethod("isCalling",      make_method(this, &YouStunAPI::isCalling));

	// Read-only property
    registerProperty("version",
                     make_property(this,
                        &YouStunAPI::get_version));
	

	boost::mutex::scoped_lock lock(mutexIdAutoincrement);
	idAutoincrement++;
	this->id = idAutoincrement;
	/*
	registerMethod("testEvent", make_method(this, &YouStunAPI::testEvent));

    registerMethod("echoasd", make_method(this, &YouStunAPI::echoasd));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &YouStunAPI::get_testString,
                        &YouStunAPI::set_testString));

  

	*/

}

FB::variant YouStunAPI::echo(const FB::variant& msg) 
{ 
    static int n(0); 
    fire_echo(msg, n++); 
    return msg; 
} 

///////////////////////////////////////////////////////////////////////////////
/// @fn YouStunAPI::~YouStunAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
YouStunAPI::~YouStunAPI()
{
	FBLOG_INFO("~YouStunAPI()", "~YouStunAPI()");
	/*
	boost::mutex::scoped_lock lock(mutexIdAutoincrement);
	if(idCaller == this->id){
		function<bool()> func = boost::bind(&YouStunAPI::TeamTalk_stop, this, 0);
		//TeamTalk_TTCallbackTasks.push(func);
	}
	lock.unlock();
	*/
}

///////////////////////////////////////////////////////////////////////////////
/// @fn YouStunPtr YouStunAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
YouStunPtr YouStunAPI::getPlugin()
{
    YouStunPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

std::string YouStunAPI::isCalling(){
	return call_idActive;
}

//call_id:"String", call_password:"String", call_version:(int), server:(int), port:(int) 
void YouStunAPI::call(const std::string& call_id, const std::string& call_password, const long call_version, const long server, const long port)
{
	FBLOG_INFO("call()", "");
	boost::mutex::scoped_lock lock(mutexIdAutoincrement);
	idCaller = this->id;
	call_idActive = call_id;
	lock.unlock();
	m_host->htmlLog("call");
	echo("call"); 

	char* serverDNS = "192.168.1.7";// linux virtual machine
	char* nickname = "asd";
	char* serverPassword = "";
	boost::thread t(boost::bind(&YouStunAPI::TeamTalk_init, this, serverDNS, port, nickname, serverPassword, call_id, call_password));
}

void YouStunAPI::disconnect()
{
	this->TeamTalk_stop(10, "Disconnect");
	boost::mutex::scoped_lock lock(mutexIdAutoincrement);
	idCaller = 0;
	call_idActive.clear();
	lock.unlock();
	/*
    function<bool()> func = boost::bind(&YouStunAPI::TeamTalk_stop, this, 0);
	TeamTalk_TTCallbackTasks.push(func);
	*/
}

// Read-only property version
std::string YouStunAPI::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}


/*

TEST

// Method echo
void YouStunAPI::echoasd()
{
     m_host->htmlLog("EchoingASASa: ");
}
 


// Read/Write property testString
std::string YouStunAPI::get_testString()
{
    return m_testString;
}
void YouStunAPI::set_testString(const std::string& val)
{
    m_testString = val;
}



// Method echo
FB::variant YouStunAPI::echo(const FB::variant& msg)
{
    static int n(0);
    fire_echo(msg, n++);
    return msg;
}

void YouStunAPI::testEvent(const FB::variant& var)
{
    fire_fired(var, true, 1);
}

*/









//TeamTalk


void YouStunAPI::TeamTalk_worker(){
	m_host->htmlLog("Worker ");
	TeamTalk_workerIncrement();
	boost::function<bool()> func;

    while(TeamTalk_TTCallbackTasks.wait_and_pop(func)){
        if(!func()){
			break;
		}	
    }
}



bool YouStunAPI::TeamTalk_stop(int code, const std::string& text){
	fire_disconnected(code, text);
	boost::mutex::scoped_lock lock(TeamTalk_mutexState);
	if(TeamTalk_state > 0){
		function<bool()> func = boost::bind(&YouStunAPI::TeamTalk_workerDecrement, this);
		TeamTalk_TTCallbackTasks.push(func);
		TT_CloseTeamTalk(TeamTalk_Instance);
		TeamTalk_state = 0;
	}
	return true;
}

void YouStunAPI::TeamTalk_init(const std::string& serverDNS, const INT32 port, const std::string& nickname, const std::string& serverPassword, const std::string& channel, const std::string& channelPassword){

	m_host->htmlLog("TeamTalk_init");
	
	this->TeamTalk_stop(2,"New Call");

	boost::mutex::scoped_lock lock(TeamTalk_mutexState);
	
	boost::thread t1(boost::bind(&YouStunAPI::TeamTalk_worker, this));
	boost::thread t2(boost::bind(&YouStunAPI::TeamTalk_worker, this));
	boost::thread t3(boost::bind(&YouStunAPI::TeamTalk_worker, this));
	
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

	function<bool()> func;
    switch(msg.wmMsg)
    {
	case WM_TEAMTALK_CON_SUCCESS :
		func = boost::bind(&YouStunAPI::TeamTalk_workerStatusChanged, this, 1);
		TeamTalk_TTCallbackTasks.push(func);
		break;
	case WM_TEAMTALK_CMD_MYSELF_LOGGEDIN :
		func = boost::bind(&YouStunAPI::TeamTalk_workerStatusChanged, this, 2);
		TeamTalk_TTCallbackTasks.push(func);
		break;
	case WM_TEAMTALK_CMD_MYSELF_JOINED :
		func = boost::bind(&YouStunAPI::TeamTalk_workerStatusChanged, this, 3);
		TeamTalk_TTCallbackTasks.push(func);
		break;
	case WM_TEAMTALK_INTERNAL_ERROR :
		func = boost::bind(&YouStunAPI::TeamTalk_stop, this, 11);
		TeamTalk_TTCallbackTasks.push(func);
		break;
	case WM_TEAMTALK_CMD_MYSELF_KICKED :
		func = boost::bind(&YouStunAPI::TeamTalk_stop, this, 12);
		TeamTalk_TTCallbackTasks.push(func);
		break;
	case WM_TEAMTALK_CMD_MYSELF_LEFT :
		func = boost::bind(&YouStunAPI::TeamTalk_stop, this, 13);
		TeamTalk_TTCallbackTasks.push(func);
		break;
	case WM_TEAMTALK_CMD_MYSELF_LOGGEDOUT :
		func = boost::bind(&YouStunAPI::TeamTalk_stop, this, 14);
		TeamTalk_TTCallbackTasks.push(func);
		break;
	case WM_TEAMTALK_CON_LOST :
		func = boost::bind(&YouStunAPI::TeamTalk_stop, this, 15);
		TeamTalk_TTCallbackTasks.push(func);
		break;
	case WM_TEAMTALK_CON_FAILED :
		func = boost::bind(&YouStunAPI::TeamTalk_stop, this, 16);
		TeamTalk_TTCallbackTasks.push(func);
		break;
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
        m_host->htmlLog("disconnected from the call");
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









//Worker Events


bool YouStunAPI::TeamTalk_workerIncrement(){
	
	boost::mutex::scoped_lock lock(TeamTalk_mutexWorker);
	TeamTalk_workingWorker++;
	return true;
}


bool YouStunAPI::TeamTalk_workerDecrement(){
	boost::mutex::scoped_lock lock(TeamTalk_mutexWorker);
	TeamTalk_workingWorker--;
	if(TeamTalk_state > 0){
		function<bool()> func = boost::bind(&YouStunAPI::TeamTalk_workerDecrement, this);
		TeamTalk_TTCallbackTasks.push(func);
	}
	return false;
}

bool YouStunAPI::TeamTalk_stop(int code){
	
	switch(code)
    {
	case 11:
		this->TeamTalk_stop(11, "Internal Error");
		break;
	case 12 :
		this->TeamTalk_stop(12, "Kicked");
		break;
	case 13 :
		this->TeamTalk_stop(13, "My Left");
		break;
	case 14 :
		this->TeamTalk_stop(14, "Logged out");
		break;
	case 15 :
		this->TeamTalk_stop(15, "Connection Lost");
		break;
	case 16 :
		this->TeamTalk_stop(16, "Connection Failed");
		break;
	default:
		this->TeamTalk_stop(0, "undefined");
	}
	return true;
}

bool YouStunAPI::TeamTalk_workerDisconnected(){
	this->TeamTalk_stop(1, "disconnected");
	return false;
}


bool YouStunAPI::TeamTalk_workerStatusChanged(int code){
	switch(code)
    {
	case 1 ://connected success
		m_host->htmlLog("connected success");
		fire_connectionStatusChanged(1);
		break;
	case 2 ://loggein
		//cout << "Logged in successfully..." << endl;
       // cout << "Got user ID #" << msg.wParam << endl;
		m_host->htmlLog("logged myself");
        
		fire_connectionStatusChanged(2);
		break;
	case 3 ://joined channel
		m_host->htmlLog("joined channel");
		fire_connectionStatusChanged(3);
		break;
	}
	return true;
}

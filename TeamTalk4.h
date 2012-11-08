#if !defined(TEAMTALKDLL_H)
#define TEAMTALKDLL_H

#include <Windows.h>



/*
 * BearWare.dk TeamTalk 4 SDK.
 *
 * Copyright 2011, BearWare.dk.
 *
 * Read the License.txt file included with the TeamTalk 4 SDK for
 * terms of use.
 */

/**
 * @brief Ensure the header and DLL are exactly the same version. To
 * get the version of the loaded DLL call TT_GetVersion(). A remote
 * client's version can be seen in the @a szVersion member of the
 * #User-struct.
 */

#define TEAMTALK4_VERSION "4.2.0.1550"

#if defined(WIN32)

#ifdef TEAMTALKDLL_EXPORTS
#define TEAMTALKDLL_API __declspec(dllexport)
#else
#define TEAMTALKDLL_API __declspec(dllimport)
#endif

#else 

#define TEAMTALKDLL_API

#endif

/**
 * @defgroup initclient Client Initialization
 *
 * @brief The first thing a user application must to is call
 * #TT_InitTeamTalk which will return a #TTInstance-pointer which
 * is a new client instance.
 *
 * #TT_InitTeamTalk takes as parameter a HWND which will have messages
 * posted whenever an event in the client instance takes place. These
 * are the events defined in #ClientEvent. If the end-user application
 * is console-based the function #TT_InitTeamTalkPoll can be used
 * where events are processed using #TT_GetMessage.
 *
 * When a new client instance is created a call to #TT_GetFlags will
 * show that the client initially is in state #CLIENT_CLOSED. This
 * means that no operation has been performed on the client.
 *
 * Querying #TT_GetFlags call can be used by a user application as a
 * state-machine to show which actions are currently possible for the
 * user application. The #ClientFlag enumeration shows the information
 * which can be retrived though #TT_GetFlags.
 *
 *
 * @defgroup sounddevices Sound Capture and Playback
 * @brief This section explains how to record and playback audio
 * in the client instance.
 *
 * Before being able to initialize the sound devices to use for
 * recording and playback the computer's available sound devices must
 * first be queried. This is done using the functions
 * #TT_GetSoundInputDevices and #TT_GetSoundOutputDevices. These two
 * functions return arrays of #SoundDevice-structs which contains a
 * description of the sound device. In the #SoundDevice-struct there's
 * a member variable called @a nDeviceID. This ID should be passed to
 * the client instance's two sound initialization functions
 * #TT_InitSoundInputDevice and #TT_InitSoundOutputDevice. Once this
 * has been done the #TT_GetFlags call will return a value containing
 * the mask bits #CLIENT_SNDINPUT_READY and #CLIENT_SNDOUTPUT_READY.
 *
 * A computer's default sound devices can be queried using
 * #TT_GetDefaultSoundDevices. A loop-back test of the selected sound
 * devices can be performed using #TT_StartSoundLoopbackTest.
 * 
 * Be aware that the sound devices might fail if e.g. a USB sound
 * device is unplugged while the client is talking in a channel. In
 * this case ensure the application is processing the errors
 * #INTERR_SNDINPUT_FAILURE and #INTERR_SNDOUTPUT_FAILURE in the
 * #WM_TEAMTALK_INTERNAL_ERROR event.
 *
 * Read section @ref transmission to see how to transmit recorded
 * audio to other users.
 *
 *
 * @defgroup videocapture Video Capture and Image Display
 *
 * @brief This section explains how to initialize a video device
 * and display captured images on the user's display.
 *
 * The client is able to capture video and present them to the user
 * application in RGB32-format and transmit the image in encoded
 * format to other users.
 *
 * @section vidcapinit Initialize Video Capture Device
 *
 * To capture video the user application must first query the
 * available capture devices by calling #TT_GetVideoCaptureDevices. A
 * #VideoCaptureDevice supports a certain number of capture formats
 * each described in the @a captureFormats member of #CaptureFormat.
 *
 * Once a device has been chosen the #TT_InitVideoCaptureDevice must
 * be called for the client instance to start capturing video
 * frames. Use the @a szDevice member of #VideoCaptureDevice as the
 * device identifier for the video capture device and pass a
 * #CaptureFormat from the @a captureFormats array of
 * #VideoCaptureDevice. The @a lpVideoCodec parameter of
 * #TT_InitVideoCaptureDevice can be passed as NULL if the captured
 * video frames do not need to be transmitted. Check out section @ref
 * codecs on how to configure the video codec.
 *
 * @section vidcapdisplay Display Captured Video
 *
 * When a video frame becomes available the event
 * #WM_TEAMTALK_USER_VIDEOFRAME is posted to the application and
 * #TT_GetUserVideoFrame can be used to extract the RGB32 image. On
 * Windows it's also possible to call #TT_PaintVideoFrame to make the
 * client instance paint on a HWND by getting its HDC.
 *
 *
 * @defgroup codecs Audio and Video Codecs
 *
 * @brief The client is able to encode audio in Speex and CELT
 * format whereas video can be encoded in <a
 * href="http://www.theora.org">Theora</a> format. Speex is
 * recommended for voice and CELT for music.
 *
 * Choosing the right codec settings in an end-user application is
 * very important and proper settings depend entirely on the user
 * scenario. Always ensure that the codec settings do not require too
 * much bandwidth and thereby resulting in packetloss causing
 * inaudible conversations and poor video quality. Detecting
 * packetloss can be done using #UserStatistics and #ClientStatistics.
 *
 * Note that bandwidth usage will be much higher when running in peer
 * to peer mode since each client must then broadcast data to all
 * users instead of having the server forward the audio and video
 * packets. Checkout the section @ref connectivity on the different
 * types of connections.
 * 
 * Every channel must configure which audio codec to use in order for
 * users to be able to talk to each other. The @a codec member of
 * #Channel specifies which audio codec (#AudioCodec) should be
 * used. A channel does not restrict the video codec (#VideoCodec)
 * users are using.
 *
 *
 * @defgroup events Client Event Handling
 *
 * @brief When events occur in the client instance, like e.g. if a new
 * user joins a channel, the client instance queues a message which
 * the user application must retrieve.
 *
 * If #TT_InitTeamTalk is used with a HWND then the events are sent to
 * the user application with WinAPI's PostMessage(...)  function and
 * is retrieved through GetMessage(...).
 * 
 * If a HWND is not used then events can instead be retrieved through
 * #TT_GetMessage.
 *
 * Note that when an event occurs the TeamTalk client instance doesn't
 * wait for the user application to process the event. So if e.g. a
 * user sends a text-message and immediately after disconnects from
 * the server, then the text-message cannot be retrieved since the
 * user is no longer available when the user application starts
 * processing the new text-message event. This is, of course, annoying
 * when designing the user application, but the reason for this design
 * choice it that the client instance is a realtime component which
 * cannot wait for the UI to process data, since audio playback and
 * recording would then be halted.
 *
 * The section @ref stepbystep gives a good idea of how events are
 * processed in a user application.
 *
 *
 * @defgroup errorhandling Client Error Handling
 *
 * @brief There are two types errors which can occur in the client,
 * either server command error or internal errors. Section @ref
 * commands describes all the commands a client can issue to a
 * server. If a server commands fails the client instance notifies the
 * user application through the event #WM_TEAMTALK_CMD_ERROR. An
 * example of a server command error could be to issue the #TT_DoLogin
 * command with an incorrect server password. The server will in this
 * case respond with the error #CMDERR_INCORRECT_SERVER_PASSWORD. The
 * user application must be designed to process these errors so
 * application users can be notified of errors.
 *
 * Internal errors are errors due to failing devices. Currently only
 * two such errors exist #INTERR_SNDINPUT_FAILURE and
 * #INTERR_SNDOUTPUT_FAILURE.
 *
 *
 * @defgroup connectivity Client/Server Connectivity
 *
 * @brief To communicate with a server the TeamTalk client creates
 * both a TCP and UDP connection to the server. Commands, i.e. the
 * TT_Do*-functions, are sent on TCP whereas audio and video are sent
 * on UDP.
 *
 * To connect to a server the user application must call
 * #TT_Connect. Once connected the event #WM_TEAMTALK_CON_SUCCESS is
 * posted to the user application and the #TT_DoLogin command can be
 * issued. Always ensure to call #TT_Disconnect before attempting to
 * create a new connection with #TT_Connect.
 *
 * When the client instance has joined a channel and wants to transmit
 * audio or video data to other users this can be done in two ways
 * depending on how the user application configures the client. One
 * way is forward through server and the other is peer to peer mode
 * which are explained in the following two sections.
 *
 * @section txforward Forward Through Server Transmission Mode
 *
 * By default the client instance is sending its audio and video
 * packets to the server and the server will then broadcast the
 * packets to the other users on behalf of the client. In other words
 * the client puts the bandwidth load onto the server. This approach
 * has its advantages and disadvantages. Since most internet
 * connections nowadays have limited upstream they cannot broadcast
 * audio and video packets to numerous users at the same time, so
 * therefore the default behaviour of the TeamTalk client is to have
 * the server do the broadcasting. This means that the server must
 * have sufficient bandwidth available to handle data transmission
 * from and to all the connected users. One disadvantage by having the
 * server forward the audio and video packets is that it doubles
 * latency, since the client doesn't send directly to other clients.
 *
 * If the server should not allow clients to forward audio and video
 * packets the @a uUserRights member of #ServerProperties must disable
 * #USERRIGHT_FORWARD_AUDIO and #USERRIGHT_FORWARD_VIDEO. Doing so means
 * that clients connecting must use peer to peer connections in order
 * to communicate. Note that the client doesn't automatically switch
 * to peer to peer mode, but relies on the user application to call
 * #TT_EnablePeerToPeer.
 *
 * @section txp2p Peer to Peer Transmission Mode
 *
 * By calling #TT_EnablePeerToPeer the client instance will attempt to
 * create peer to peer connections to all users it's communicating
 * with. This reduces latency since the client will then broadcast the
 * audio and video packets itself, so they do not have to be forwarded
 * through the server.
 *
 * The event #WM_TEAMTALK_CON_P2P is posted to the user application if
 * a peer to peer connection either fails or succeeds. If a peer to
 * peer connection fails the client will send through the server to
 * that user given that the server allows it, i.e. if
 * #USERRIGHT_FORWARD_AUDIO or #USERRIGHT_FORWARD_VIDEO is enabled.
 *
 * If the server does not allow users to forward audio and video
 * packets and the peer to peer connection to a user fails, then that
 * user will be unavailable for audio and video data. So beware of
 * this when configuring the server and client.
 *
 *
 * @defgroup commands Client/Server Commands
 *
 * @brief This section contains the list of commands which can be
 * issued by the client instance to the server.
 *
 * @section cmdprocessing Client/Server Command Processing
 *
 * Functions with the prefix TT_Do* are commands which the client can
 * issue to the server. Every TT_Do* function returns a command
 * identifier which can user application can use to check when the
 * server has finished processing the issued command. Once the client
 * receives a response to a command the client instance posts the
 * event #WM_TEAMTALK_CMD_PROCESSING to the user application
 * containing the command identifier and whether the command is being
 * processed or it has completed.
 *
 * As an example, say the user application wants to issue the
 * #TT_DoLogin command. When the application calls #TT_DoLogin the
 * returned command ID is stored in a variable. The application then
 * waits for the #WM_TEAMTALK_CMD_PROCESSING event to be posted with
 * the stored command ID. The first time #WM_TEAMTALK_CMD_PROCESSING
 * is posted to the user application it is to say that processing has
 * begun. The second time #WM_TEAMTALK_CMD_PROCESSING is called it is
 * to say that the command has completed. In between the command
 * starting and completing several other events may take place. If
 * e.g. the #TT_DoLogin fails the user application would receive the
 * event #WM_TEAMTALK_CMD_ERROR. If on the other hand the command was
 * successful all the channels and user would be posted as events to
 * the application before the login-command completed processing.
 *
 *
 * @defgroup transmission Audio and Video Transmission
 *
 * @brief Once the client instance has joined a channel it can
 * transmit audio and video to other users in the channel. When
 * transmitting audio and video it is important to be aware of how the
 * client is configured to do this. Sections @ref txforward and @ref
 * txp2p explains the two supported data transmission modes.
 *
 * To transmit audio the client must have the flag
 * #CLIENT_SNDINPUT_READY enabled which is done in the function
 * #TT_InitSoundInputDevice. To transmit video requires the flag
 * #CLIENT_VIDEO_READY which is enabled by the function
 * #TT_InitVideoCaptureDevice. To hear what others users are saying a
 * sound output device must have been configured using
 * #TT_InitSoundOutputDevice and thereby have enabled the flag
 * #CLIENT_SNDOUTPUT_READY.
 *
 * Calling #TT_EnableTransmission will make the client instance start
 * transmitting either audio or video data. Note that audio
 * transmission can also be activated automatically using voice
 * activation. This is done by called #TT_EnableVoiceActivation and
 * setting a voice activation level using #TT_SetVoiceActivationLevel.
 *
 *
 * @defgroup server Server Properties
 *
 * @brief The server keeps track of which users are in which channels
 * and ensures that users in the same channel can communicate with
 * each other. It is also the job of the server to provide user
 * authentication so only users with the proper credentials are
 * allowed to do certain operations.
 *
 * It is a good idea to check out section @ref serversetup to learn
 * how to configure the TeamTalk server.
 *
 * The server can be configured in a number of ways using the
 * #ServerProperties-struct. Only users who are logged on to the
 * server as administrators can change a server's properties while
 * it's running. This is done using the command #TT_DoUpdateServer.
 *
 * The @a uUserRights bitmask in #ServerProperties specifies what
 * users are allowed to do. The server can e.g. be configured to only
 * allowed users who have an account to log on by disabling the flag
 * #USERRIGHT_GUEST_LOGIN. If default users (without administrator
 * rights) shouldn't be allowed to create channels this can be
 * disabled by disabling the flag #USERRIGHT_CHANNEL_CREATION.
 *
 * @section useradmin User Administration
 *
 * Two types of users exists on a server, default users and
 * administrator users. The #UserType-enum can be used to see who is
 * what. To be administrator the user must have a user account on the
 * server with the #USERTYPE_ADMIN flag set.
 *
 * As administrator it is possible to list all users who have an
 * account on the server using #TT_DoListUserAccounts. To create a new
 * user account call the command #TT_DoNewUserAccount and to delete an
 * account call #TT_DoDeleteUserAccount.
 *
 * @section userban Kicking and Banning Users
 *
 * Sometimes it may be necessary to kick and ban users from a
 * server. As administrator it is possible to use the command
 * #TT_DoKickUser to kick a user off the server. A channel operator
 * can also kick a user from a channel (but not off a server).
 *
 * As administrator it is also possible to ban users from the server,
 * so they can no longer log in. This can be done using
 * #TT_DoBanUser. To list who are currently banned call #TT_DoListBans
 * and to remove a ban call #TT_DoUnBanUser.
 *
 *
 * @defgroup channels Channel Properties
 *
 * @brief Users are arranged in a tree structure consisting of
 * channels where each channel can hold a number of users. While
 * in a channel users can transmit audio and video to each other
 * as well as sending channel messages. On a server there will
 * always be a root channel which cannot be deleted.
 *
 * In other conferencing tools channels are also refered to as
 * "rooms".
 *
 * @section chanadmin Channel Administration
 *
 * To create a new channel on a server an administrator can call
 * #TT_DoMakeChannel whereas a default user (without administrator
 * rights) has to use #TT_DoJoinChannel to create a new channel. Using
 * #TT_DoJoinChannel creates a dynamic channel which disappears again
 * from the server when the last user leaves the channel. The first
 * user to join a dynamic channel will become operator of the channel,
 * meaning that he can kick user and make other operators as well.
 *
 * Updating a channel's properties can only be done by administrators
 * and operators of the channel and this is done by using
 * #TT_DoUpdateChannel.
 *
 * To remove a channel a user must be administrator and can do so by
 * calling #TT_DoRemoveChannel.
 * 
 * @section filesharing File Sharing
 *
 * While in a channel users can upload and download files. To upload a
 * file to a channel the channel needs to have a disk quota. The disk
 * quota is specified by @a nDiskQuota in the #Channel-struct. The
 * file being uploaded must have a file size which is less than the
 * disk quota and the sum of sizes of existing files. Once a file is
 * uploaded only administrators, channel operators and the file's
 * owner can delete a file. Note that the file's owner must have an
 * account on the server to delete the file.
 *
 * Call #TT_DoSendFile to upload a file and #TT_DoRecvFile to download
 * a file. Only users who have a #UserAccount on the server are
 * allowed to upload files. There is no limit on the maximum number of
 * file transfers but it is advised to queue file transfers so the
 * file transfers do no affect server performance.
 *
 * @section voicelog Storing Conversations to Audio Files
 *
 * In some applications it may be required to be able to save all
 * audio data received by the client instance to disk. This can be
 * archived by calling #TT_SetUserAudioFolder which will then save
 * received audio data in the following format: "YYYYMMDD-HHMMSS
 * \#USERID USERNAME.wav". USERNAME is the @a szUsername from #User.
 *
 * To store audio data from outside the local client instance's
 * channel, please read section @ref spying.
 *
 *
 * @defgroup users User Properties
 *
 * @brief Users can be seen on the server after a successful call
 * to #TT_DoLogin. Once logged in a user can send user to user
 * text-messages using #TT_DoTextMessage as well as receive
 * broadcast messages. A user cannot send audio and video data to
 * other users until they have joined the same channel.
 *
 * @section usertypes User Types
 *
 * A user can either be a default user #USERTYPE_DEFAULT or an
 * administrator #USERTYPE_ADMIN. A default user has limited rights on the
 * server whereas an administrator can change server properties,
 * create, delete and remove channels as well as move, kick and ban
 * users.
 *
 * @section userinteract User Interaction
 *
 * Once a user has joined a channel it is possible to transmit audio
 * and video data to other users. If a user starts talking in the
 * channel the event #WM_TEAMTALK_USER_TALKING is posted to the user
 * application and if a video frame is received the event
 * #WM_TEAMTALK_USER_VIDEOFRAME is sent to the user application.
 *
 * @section uservolume User Audio Settings
 *
 * While in the same channel the user application can change different
 * settings on a user. If e.g. a user's volume is too low the user
 * application can call #TT_SetUserVolume to increase the volume. If
 * it's still not loud enough #TT_SetUserGainLevel can be called to
 * use software to amplify the volume level.
 *
 * If on the other hand the user application wants to mute a user
 * #TT_SetUserMute can be used for this. Note that muting a user
 * doesn't mean that the client instance will stop receiving audio
 * from that user, it simply means it will not be played. To stop
 * receiving audio from a user the local client instance must ask the
 * server to unsubscribe audio data from the user. This is explained
 * in the next section.
 *
 * @section subscriptions User Subscriptions
 * 
 * When logging on to a server the local client instance will by
 * default subscribe to user messages, channel messages, broadcast
 * messages, audio data and video data from all users. If, however, a
 * client wants to stop receiving e.g. audio from a user, he can call
 * #TT_DoUnsubscribe along with the user ID and the
 * #SUBSCRIBE_AUDIO-flag to tell the server that he no longer wants to
 * receive audio from that user. The server will then respond with the
 * event #WM_TEAMTALK_CMD_USER_UPDATE and the @a uLocalSubscriptions
 * member of #User will have the #SUBSCRIBE_AUDIO-flag removed. At the
 * remote user the \a uPeerSubscriptions member will be
 * changed. Subscribe/unsubscribe can also be done for user, channel
 * and broadcast messages and video data. The function #TT_DoSubscribe
 * can be used to revert unsubscriptions.
 *
 * @subsection spying Spying on Users
 *
 * Previously it has been said that users can only receive audio and
 * video from users when they are in the same channel, but actually an
 * administrator user can call #TT_DoSubscribe with the flags prefixed
 * SUBSCRIBE_INTERCEPT_* to spy on users outside his own channel. In
 * other words it's possible hear and see video data outside ones
 * channel. Also all user and channel messages sent by a user can also
 * be intercepted in this way.
 *
 * Having the ability to intercept all data sent from users in any
 * channel means that it's possible to log everything that is
 * happening on the server. Both audio and video transfers as well as
 * text messaging. Checkout #TT_SetUserAudioFolder on how to store
 * voice data to audio files.
 *
 *
 * @defgroup hotkey Windows Hotkeys
 *
 * @brief Hotkeys can be used to e.g. enable push-to-talk.
 *
 * Windows supports intercepting key strokes globally, i.e. without
 * having the user application's window focused. To investigate which
 * keys are currently being pressed the function
 * #TT_HotKey_InstallTestHook can be used. Once the desired
 * key-combination has been found the function #TT_HotKey_Register can
 * be used to register the combination as a hotkey and have the
 * #WM_TEAMTALK_HOTKEY event posted whenever the key combination
 * becomes active.
 *
 * Note that it's not advised to have a hotkey installed while
 * debugging an application in Visual Studio. It slows down the
 * debugger dramatically.
 * 
 *
 * @defgroup mixer Windows Mixer
 *
 * @brief The Windows mixer can be manipulated so e.g. Line In can be
 * chosen instead of Microphone for recording.
 *
 * To find the mixer which is associated with the current sound input
 * or output device the @a nWaveDeviceID member of #SoundDevice must
 * be used when querying the mixer.
 *
 * @defgroup firewall Windows Firewall
 *
 * @brief The Windows Firewall can be modified so applications can be
 * added to the firewall's exception list.
 *
 * The Windows Firewall was introduced in Windows XP SP2. Modifying
 * the Windows Firewall requires administrator rights. On Windows XP
 * the user running the application, which calls the DLL, is assumed
 * to have administrator rights. On Windows Vista/7 the DLL will
 * automatically call User Account Control (UAC) to obtain
 * administrator rights.
 *
 * Check out TT_Firewall_AddAppException() on how to add application
 * executables to the Windows Firewall exception list.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

    /* OS specific types. */
#if defined(WIN32)
    /** @brief TeamTalk uses Unicode on Windows (which means that it's
     * not able to run on Windows 9x/ME). @see TT_STRLEN */

    /* If you get a compiler error here you probably forgot to include
     * <windows.h> before this file.  */
    typedef WCHAR TTCHAR; 
#else
    typedef char TTCHAR;
    typedef int BOOL;
    typedef int INT32;
    typedef long long INT64;
    typedef unsigned int UINT32;
    typedef void VOID;

/** Windows has macros which tell whether a parameter is used as input
 * or output so these are just defined as nothing. */

#define IN
#define OUT

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/** @brief Linux doesn't have WM_USER defined so set it to 0. It is
 * used as an initial value for #ClientEvent. */
#define WM_USER 0

#endif
    /** @ingroup initclient
     *
     * @brief Pointer to a TeamTalk client instance created by
     * #TT_InitTeamTalk. @see TT_CloseTeamTalk */
    typedef VOID TTInstance;

    /** @addtogroup events
     * @{ */

    /** 
     * @brief TeamTalk client event messages. On Windows these
     * messages are posted to the HWND which was provided to
     * #TT_InitTeamTalk
     */
    typedef enum ClientEvent
    {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
        /** @brief Ignore this message. It can be used in applications
         * to filter messages related to TeamTalk and internal
         * messages */
        WM_TEAMTALK_DUMMY_FIRST = WM_USER + 449, 
#endif
        /**
         * @brief Connected successfully to the server.
         *
         * This event is posted if #TT_Connect was successful.
         *
         * #TT_DoLogin can now be called in order to logon to the
         * server.
         *
         * @param wParam Ignored
         * @param lParam Ignored
         * @see TT_Connect
         * @see TT_DoLogin */
        WM_TEAMTALK_CON_SUCCESS, /* WM_USER + 450 */

        /** 
         * @brief Failed to connect to server.
         *
         * This event is posted if #TT_Connect fails. Ensure to call
         * #TT_Disconnect before calling #TT_Connect again.
         *
         * @param wParam Ignored
         * @param lParam Ignored
         * @see TT_Connect */
        WM_TEAMTALK_CON_FAILED,

        /** 
         * @brief Connection to server has been lost.
         *
         * The server is not responding to requests from the local
         * client instance and the connection has been dropped. To
         * change when the client instance should regard the server as
         * unavailable call #TT_SetServerTimeout. 
         *
         * #TT_GetStatistics can be used to check when data was
         * last received from the server.
         *
         * Ensure to call #TT_Disconnect before calling #TT_Connect
         * again.
         *
         * @param wParam Ignored
         * @param lParam Ignored
         * @see TT_Connect */
        WM_TEAMTALK_CON_LOST,

        /** 
         * @brief Peer to peer (p2p) status changed.
         *
         * Audio and video packets can be sent to the user without
         * forwarding through the server. This is done by enabling P2P
         * network using #TT_EnablePeerToPeer. Once the client instance
         * joins a new channel it will try and create a peer to peer
         * connection to each user in the channel. The client instance
         * will try to create a peer to peer connection for 5 seconds.
         *
         * Read section @ref txforward and section @ref txp2p on the
         * different ways of transmitting data.
         *
         * @param wParam User ID
         * @param lParam TRUE if P2P connection was successful, FALSE if P2P
         * connection failed.
         * @see USERRIGHT_FORWARD_AUDIO
         * @see USERRIGHT_FORWARD_VIDEO */
        WM_TEAMTALK_CON_P2P,

        /** 
         * @brief A command issued by @c TT_Do* methods is being
         * processed.
         *
         * Read section @ref cmdprocessing on how to use command
         * processing in the user application.
         *
         * @param wParam Command ID being processed (returned by TT_Do* 
         * commands)
         * @param lParam Is 0 if command ID started processing and 1
         * if the command has finished processing. */
        WM_TEAMTALK_CMD_PROCESSING,

        /** 
         * @brief The client instance successfully logged on to
         * server.
         *
         * The call to #TT_DoLogin was successful and all channels on
         * the server will be posted in the event
         * #WM_TEAMTALK_CMD_CHANNEL_NEW immediately following this
         * event. If #USERRIGHT_VIEW_ALL_USERS is enabled the client
         * instance will also receive the events
         * #WM_TEAMTALK_CMD_USER_LOGGEDIN and
         * #WM_TEAMTALK_CMD_USER_JOINED for every user on the server.
         *
         * @param wParam The client instance's user ID, i.e. what can now 
         * be retrieved through #TT_GetMyUserID.
         * @param lParam Ignored
         * @see TT_DoLogin */
        WM_TEAMTALK_CMD_MYSELF_LOGGEDIN,

        /** 
         * @brief The client instance logged out of a server.  
         *
         * A response to #TT_DoLogout.
         *
         * @param wParam Ignored
         * @param lParam Ignored
         * @see TT_DoLogout */
        WM_TEAMTALK_CMD_MYSELF_LOGGEDOUT,

        /** 
         * @brief The client instance has joined a new channel. 
         *
         * Result of command #TT_DoJoinChannel or
         * #TT_DoJoinChannelByID. Can also be a result of an
         * administrator calling #TT_DoMoveUser.
         *
         * If #USERRIGHT_VIEW_ALL_USERS is disabled the client instance
         * will afterwards receive the #WM_TEAMTALK_CMD_USER_JOINED
         * event for each of the users in the channel.
         *
         * @param wParam Channel's ID
         * @param lParam Ignored
         * @see WM_TEAMTALK_CMD_MYSELF_LEFT */
        WM_TEAMTALK_CMD_MYSELF_JOINED,

        /** 
         * @brief The client instance left a channel. 
         *
         * The WPARAM contains the channel ID. 
         *
         * @param wParam Channel's ID
         * @param lParam Ignored
         * @see TT_DoLeaveChannel
         * @see WM_TEAMTALK_CMD_MYSELF_JOINED */
        WM_TEAMTALK_CMD_MYSELF_LEFT,

        /** 
         * @brief The client instance was kicked from a channel.
         *
         * @param wParam User ID of the kicker.
         * @param lParam Ignored */
        WM_TEAMTALK_CMD_MYSELF_KICKED,

        /**
         * @brief A new user logged on to the server.
         *
         * Use #TT_GetUser to get the properties of the user.
         *
         * @param wParam The user's ID.
         * @param lParam Unused
         * @see TT_DoLogin
         * @see TT_GetUser To retrieve user.
         * @see WM_TEAMTALK_CMD_USER_LOGGEDOUT */
        WM_TEAMTALK_CMD_USER_LOGGEDIN,

        /**
         * @brief A client logged out of the server. 
         *
         * This event is called when a user logs out with
         * #TT_DoLogout or disconnects with #TT_Disconnect.
         *
         * @param wParam The user's ID.
         * @param lParam Unused
         * @see TT_DoLogout
         * @see TT_Disconnect
         * @see WM_TEAMTALK_CMD_USER_LOGGEDIN */
        WM_TEAMTALK_CMD_USER_LOGGEDOUT,

        /**
         * @brief User changed properties.
         *
         * Use #TT_GetUser to see the new properties.
         *
         * @param wParam User's ID
         * @param lParam Channel ID. 0 for no channel.
         * @see TT_GetUser To retrieve user. */
        WM_TEAMTALK_CMD_USER_UPDATE,

        /** 
         * @brief A user has joined a channel.
         *
         * @param wParam User's ID.
         * @param lParam Channel ID.
         * @see TT_GetUser To retrieve user. */
        WM_TEAMTALK_CMD_USER_JOINED,

        /** 
         * @brief User has left a channel.
         *
         * @param wParam User's ID.
         * @param lParam Channel ID. */
        WM_TEAMTALK_CMD_USER_LEFT,

        /** 
         * @brief A user has sent a text-message.
         *
         * @param wParam The user's user ID.
         * @param lParam The message's ID. 
         * @see TT_GetTextMessage To retrieve text message.
         * @see TT_GetUser To retrieve user.
         * @see TT_DoTextMessage() To send text message. */
        WM_TEAMTALK_CMD_USER_TEXTMSG,

        /** 
         * @brief A new channel has been created.
         *
         * @param wParam Channel's ID
         * @param lParam Ignored
         * @see TT_GetChannel To retrieve channel. */
        WM_TEAMTALK_CMD_CHANNEL_NEW,

        /** 
         * @brief A channel's properties has been updated.
         *
         * wParam Channel's ID
         * lParam Ignored
         * @see TT_GetChannel To retrieve channel. */
        WM_TEAMTALK_CMD_CHANNEL_UPDATE,

        /** 
         * @brief A channel has been removed.
         *
         * Note that calling the #TT_GetChannel with the channel ID
         * will fail because the channel is no longer there.
         *
         * @param wParam Channel's ID
         * @param lParam Ignored */
        WM_TEAMTALK_CMD_CHANNEL_REMOVE,

        /** 
         * @brief Server has updated its settings (server name, MOTD,
         * etc.)
         * 
         * Use #TT_GetServerProperties to get the new server
         * properties.
         *
         * @param wParam Ignored
         * @param lParam Ignored */
        WM_TEAMTALK_CMD_SERVER_UPDATE,

        /** 
         * @brief A new file is added to a channel. 
         *
         * Use #TT_GetChannelFileInfo to get information about the
         * file.
         *
         * @param wParam File ID.
         * @param lParam Channel ID.
         * @see TT_GetChannelFileInfo To retrieve file. */
        WM_TEAMTALK_CMD_FILE_NEW,

        /** 
         * @brief A file has been removed from a channel.
         *
         * @param wParam File ID. 
         * @param lParam Channel ID. */
        WM_TEAMTALK_CMD_FILE_REMOVE,

        /** 
         * @brief The server rejected a command issued by the local
         * client instance.
         *
         * To figure out which command failed use the command ID
         * returned by the TT_Do* command. Section @ref cmdprocessing
         * explains how to use command ID.
         *
         * @param wParam Error number
         * @param lParam The command ID returned from the TT_Do* commands.
         * @see TT_GetErrorMessage */
        WM_TEAMTALK_CMD_ERROR,

        /**
         * @brief The server successfully processed  a command issued by the local
         * client instance.
         *
         * To figure out which command succeeded use the command ID
         * returned by the TT_Do* command. Section @ref cmdprocessing
         * explains how to use command ID.
         *
         * @param wParam The command ID returned from the TT_Do* commands.
         * @param lParam 0. */
        WM_TEAMTALK_CMD_SUCCESS,

        /**
         * @brief A user is talking.
         *
         * Playback using the sound output device has started for a
         * user.
         *
         * @param wParam User's ID.
         * @param lParam TRUE if talking otherwise FALSE.
         * @see TT_GetUser To retrieve user.
         * @see TT_IsTransmitting To see if 'myself' is transmitting.
         * @see TT_SetUserStoppedTalkingDelay */
        WM_TEAMTALK_USER_TALKING,

        /** 
         * @brief A new video frame was received from a user.
         *
         * Use #TT_GetUserVideoFrame to display the image.
         *
         * @param wParam User's ID.
         * @param lParam Number of video frames currently in queue for
         * display. The client uses a cyclic buffer for video frame in
         * order to prevent resources from being drained. Therefore
         * the #WM_TEAMTALK_USER_VIDEOFRAME event might be posted
         * more times than there actually are frames available. So use
         * the frame count to ensure the lastest frame is always
         * displayed. */
        WM_TEAMTALK_USER_VIDEOFRAME,

        /** 
         * @brief An audio file recording has changed status.
         *
         * #TT_SetUserAudioFolder makes the client instance store all
         * audio from a user to a specified folder. Every time an
         * audio file is being processed this event is posted.
         *
         * @param wParam The user's ID.
         * @param lParam The status of the audio file. Based on
         * #AudioFileStatus. */
        WM_TEAMTALK_USER_AUDIOFILE,

        /** 
         * @brief A sound device failed to initialize. 
         *
         * This can e.g. happen if a new user joins a channel and
         * there is no sound output channels
         * available. nMaxOutputChannels of #SoundDevice struct tells
         * how many streams can be active simultaneously.
         *
         * @param wParam An error number based on #ClientError. The value
         * will be of the type @c INTERR_*.
         * @see WM_TEAMTALK_CMD_MYSELF_JOINED If sound input device fails it
         * will be when joining a new channel.
         * @see TT_GetSoundOutputDevices
         * @see SoundDevice */
        WM_TEAMTALK_INTERNAL_ERROR,

        /** 
         * @brief Voice activation has triggered transmission.
         *
         * @param wParam TRUE if enabled, FALSE if disabled.
         * @param lParam Unused
         * @see TT_SetVoiceActivationLevel
         * @see CLIENT_SNDINPUT_VOICEACTIVATION
         * @see TT_EnableTransmission */
        WM_TEAMTALK_VOICE_ACTIVATION,

        /** 
         * @brief An audio file being streamed to a user is
         * processing.
         *
         * This event is called as a result of
         * #TT_StartStreamingAudioFileToUser.
         *
         * @param wParam User's ID of where the audio file is streamed to.
         * @param lParam The status of the audio file. Based on #AudioFileStatus
         * @see TT_StartStreamingAudioFileToUser */
        WM_TEAMTALK_STREAM_AUDIOFILE_USER,

        /** 
         * @brief Audio file being stream to a channel is processing.
         *
         * This event is called as a result of
         * #TT_StartStreamingAudioFileToChannel.
         *
         * @param wParam Channel's ID of where the audio file is being 
         * streamed to.
         * @param lParam The status of the audio file. Based on
         * #AudioFileStatus.
         * @see TT_StartStreamingAudioFileToChannel */
        WM_TEAMTALK_STREAM_AUDIOFILE_CHANNEL,

        /** 
         * @brief A hotkey has been acticated or deactivated.
         *
         * @param wParam The hotkey ID passed to #TT_HotKey_Register
         * @param lParam is TRUE when hotkey is active and FALSE when 
         * it becomes inactive.
         * @see TT_HotKey_Register
         * @see TT_HotKey_Unregister */
        WM_TEAMTALK_HOTKEY,

        /**
         * @brief A button was pressed or released on the user's
         * keyboard or mouse.
         * 
         * When #TT_HotKey_InstallTestHook is called a hook is
         * installed in Windows which intercepts all keyboard and
         * mouse presses. Every time a key or mouse is pressed or
         * released this event is posted.
         *
         * Use #TT_HotKey_GetKeyString to get a key description of the 
         * pressed key.
         *
         * @param wParam The virtual key code. Look here for a list of virtual
         * key codes: http://msdn.microsoft.com/en-us/library/ms645540(VS.85).aspx
         * @param lParam TRUE when key is down and FALSE when released. 
         * @see TT_HotKey_InstallTestHook */
        WM_TEAMTALK_HOTKEY_TEST,

        /**
         * @brief A file transfer is processing. 
         *
         * Use #TT_GetFileTransferInfo to get information about the
         * file transfer. Ensure to check if the file transfer is
         * completed, because the file transfer instance will be
         * removed from the client instance when the user application
         * reads the #FileTransfer object and it has completed the
         * transfer.
         *
         * @param wParam Transfer ID
         * @param lParam The #FileTransfer's status described by 
         * #FileTransferStatus.
         * @see TT_GetFileTransferInfo To retrieve #FileTransfer.*/
        WM_TEAMTALK_FILETRANSFER,

#ifndef DOXYGEN_SHOULD_SKIP_THIS
        /** @brief Ignore this event. It can be used in applications
         * to filter messages related to TeamTalk and internal
         * messages */
        WM_TEAMTALK_DUMMY_LAST
#endif
    } ClientEvent;

    /**
     * @brief A struct containing the properties of an event.
     *
     * The event can be retrieved by called #TT_GetMessage. This
     * struct is only required on non-Windows systems.
     *
     * Section @ref events explains event handling in the local client
     * instance.
     *
     * @see TT_GetMessage */
    typedef struct TTMessage
    {
        /** @brief The event's message number @see ClientEvent */
        ClientEvent wmMsg;
        /** @brief The first message parameter */
        UINT32 wParam;
        /** @brief The second message parameter */
        UINT32 lParam;
    } TTMessage;

    /** @}*/

    /** @def TT_STRLEN
     * @brief The length of a string (characters, not bytes) which is used to
     * extract information from this DLL. 
     *
     * If a string is passed to the client instance is longer than
     * TT_STRLEN it will be truncated.
     *
     * On Windows the client instance converts unicode characters to
     * UTF-8 before transmission, so be aware of non-ASCII characters
     * if communicating with the TeamTalk server from another
     * applications than the TeamTalk client. */
#define TT_STRLEN 512

    /** @ingroup videocapture
     * @def TT_CAPTUREFORMATS_MAX
     * The maximum number of video formats which will be queried for a 
     * #VideoCaptureDevice. */
#define TT_CAPTUREFORMATS_MAX 128

    /** @ingroup channels
     * @def TT_VOICEUSERS_MAX
     * The maximum number of users in queue when a #Channel is configured
     * with #CHANNEL_CLASSROOM. */
#define TT_VOICEUSERS_MAX 16

    /** @ingroup channels
     * @def TT_VIDEOUSERS_MAX
     * The maximum number of users in queue when a #Channel is configured
     * with #CHANNEL_CLASSROOM. */
#define TT_VIDEOUSERS_MAX 16

    /** @addtogroup sounddevices
     * @{ */

    /**
     * @brief The supported sound systems.
     *
     * DirectSound is highly recommended on Windows 2K/XP/Vista.
     *
     * @see SoundDevice
     * @see TT_InitSoundInputDevice
     * @see TT_InitSoundOutputDevice */
    typedef enum SoundSystem
    {
        /** @brief Should be used on Windows CE. */
        SOUNDSYSTEM_WINMM = 1,
        /** @brief Should be used on Windows 2K/XP/Vista. */
        SOUNDSYSTEM_DSOUND = 2,
        /** @brief Should be used on Linux. */
        SOUNDSYSTEM_ALSA = 3,
        /** @brief Should be used on MacOS. */
        SOUNDSYSTEM_COREAUDIO = 4
    } SoundSystem;

    /** 
     * @brief A struct containing the properties of a sound device
     * for either playback or recording.
     *
     * Use @a nDeviceID to pass to #TT_InitSoundInputDevice or
     * #TT_InitSoundOutputDevice.
     *
     * Note that the @a nDeviceID may change if the user application
     * is restarted and a new sound device is added or removed from
     * the computer.
     * 
     * @see TT_GetSoundInputDevices
     * @see TT_GetSoundOutputDevices */
    typedef struct SoundDevice
    {
        /** @brief The ID of the sound device. Used for passing to
         * #TT_InitSoundInputDevice and
         * #TT_InitSoundOutputDevice. Note that @a nDeviceID might change
         * if USB sound devices are plugged in or unplugged, therefore
         * use @a szDeviceID to ensure proper device is used.  */
        INT32 nDeviceID;
        /** @brief The sound system used by the sound device */
        SoundSystem nSoundSystem;
        /** @brief The name of the sound device */
        TTCHAR szDeviceName[TT_STRLEN];
        /** @brief An identifier uniquely identifying the sound device
         * even when new sound devices are being added and removed.  In
         * DirectSound and WinMM it would be the GUID of the sound
         * device. Note that it may not always be available. */
        TTCHAR szDeviceID[TT_STRLEN];
#ifdef WIN32
        /** 
         * @brief The ID of the device used in Win32's
         * waveInGetDevCaps and waveOutGetDevCaps.
         *
         * Value will be -1 if no ID could be found This ID can also
         * be used to find the corresponding mixer on Windows passing
         * it as @a nWaveDeviceID.  Note that this ID applies both to
         * DirectSound and WinMM.
         *
         * @see TT_Mixer_GetWaveInName
         * @see TT_Mixer_GetWaveOutName
         * @see TT_Mixer_GetMixerCount */
        INT32 nWaveDeviceID;
#endif
        /** @brief Whether the sound device supports 3D-sound
         * effects. */
        BOOL bSupports3D;
        /** @brief The maximum number of input channels. */
        INT32 nMaxInputChannels;
        /** @brief The maximum number of output channels. */
        INT32 nMaxOutputChannels;
        /** @brief Supported sample rates by device. A zero value terminates
         * the list of supported sample rates or its maximum size of 16. 
         * Investigating the support sample rates is usually only required
         * on Linux since sound devices often don't numerous sample rates. */
        INT32 supportedSampleRates[16];
    } SoundDevice;
    
    /**
     * @brief An enum encapsulation the minimum, maximum and default sound
     * levels for input and output sound devices. */
    typedef enum SoundLevel
    {
        /**
         * @brief The maximum value of recorded audio.
         * @see TT_GetSoundInputLevel
         * @see TT_SetVoiceActivationLevel
         * @see TT_GetVoiceActivationLevel */
        SOUND_VU_MAX = 20,
        /**
         * @brief The minimum value of recorded audio.
         * @see TT_GetSoundInputLevel
         * @see TT_SetVoiceActivationLevel
         * @see TT_GetVoiceActivationLevel */
        SOUND_VU_MIN = 0,
        /**
         * @brief The maximum volume for master volume. To gain the
         * volume level using software call #TT_SetUserGainLevel.
         *
         * @see TT_SetSoundOutputVolume
         * @see TT_GetSoundOutputVolume
         * @see TT_SetUserVolume
         * @see TT_GetUserVolume */
        SOUND_VOLUME_MAX = 255,
        /**
         * @brief The minimum volume for master volume.
         * @see TT_SetSoundOutputVolume
         * @see TT_GetSoundOutputVolume
         * @see TT_SetUserVolume
         * @see TT_GetUserVolume */
        SOUND_VOLUME_MIN = 0,
        /**
         * @brief The maximum gain level. 
         *
         * A gain level of 32000 gains the volume by a factor 32.  A gain
         * level of #SOUND_GAIN_DEFAULT means no gain.
         *
         * @see TT_SetSoundInputGainLevel
         * @see TT_GetSoundInputGainLevel
         * @see TT_SetUserGainLevel
         * @see TT_GetUserGainLevel */
        SOUND_GAIN_MAX = 32000,
        /**
         * @brief The default gain level.
         *
         * A gain level of 1000 means no gain. Check #SOUND_GAIN_MAX
         * and #SOUND_GAIN_MIN to see how to increase and lower gain
         * level.
         *
         * @see TT_SetSoundInputGainLevel
         * @see TT_GetSoundInputGainLevel
         * @see TT_SetUserGainLevel
         * @see TT_GetUserGainLevel */
        SOUND_GAIN_DEFAULT = 1000,
        /**
         * @brief The minimum gain level (since it's zero it means
         * silence).
         *
         * A gain level of 100 is 1/10 of the default volume.
         *
         * @see TT_SetSoundInputGainLevel
         * @see TT_GetSoundInputGainLevel
         * @see TT_SetUserGainLevel
         * @see TT_GetUserGainLevel */
        SOUND_GAIN_MIN = 0
    } SoundLevel;
    /** @} */

    /** @addtogroup videocapture
     * @{ */

    /** 
     * @brief The picture format used by a capture device. 
     *
     * @see CaptureFormat
     * @see VideoCaptureDevice */
    typedef enum FourCC
    {
        /** @brief Internal use to denote no supported formats. */
        FOURCC_NONE   =   0,
        /** @brief Prefered image format with the lowest bandwidth
         * usage. A 640x480 pixel image takes up 460.800 bytes. */
        FOURCC_I420   = 100,
        /** @brief Image format where a 640x480 pixel images takes up
         * 614.400 bytes. */
        FOURCC_YUY2   = 101,
        /** @brief The image format with the highest bandwidth
         * usage. A 640x480 pixel images takes up 1.228.880 bytes. */
        FOURCC_RGB32  = 102
    } FourCC;

    /** 
     * @brief A struct containing the properties of a video capture
     * format.
     *
     * A struct for holding a supported video capture format by a 
     * #VideoCaptureDevice. */
    typedef struct CaptureFormat
    {
        /** @brief The width in pixels of the video device supported
         * video format. */
        INT32 nWidth;
        /** @brief The height in pixels of the video device supported
         * video format. */
        INT32 nHeight;
        /** @brief The numerator of the video capture device's video
         * format. Divinding @a nFPS_Numerator with @a
         * nFPS_Denominator gives the frame-rate. */
        INT32 nFPS_Numerator; 
        /** @brief The denominator of the video capture device's video
         * format. Divinding @a nFPS_Numerator with @a
         * nFPS_Denominator gives the frame-rate.*/
        INT32 nFPS_Denominator; 
        /** @brief Picture format for capturing. */
        FourCC picFourCC; 
    } CaptureFormat;

    /**
     * @brief A RGB32 image where the pixels can be accessed directly
     * in an allocated @a frameBuffer.
     *
     * Use TT_AcquireUserVideoFrame() to acquire a user's image and
     * remember to call TT_ReleaseUserVideoFrame() when the image has
     * been processed so TeamTalk can release its resources. */
    typedef struct VideoFrame
    {
        /** @brief The width in pixels of the image contained in @a
         * frameBuffer. */
        INT32 nWidth;
        /** @brief The height in pixels of the image contained in @a
         * imageBuffer. */
        INT32 nHeight;
        /** @brief A unique identifier for the frames which are part of the
         * same video sequence. If the stream ID changes it means the
         * frames which are being received are part of a new video sequence
         * and @a nWidth and @a nHeight may have changed. The @a nStreamID
         * will always be a positive integer value.*/
        INT32 nStreamID;
        /** @brief Whether the image acquired is a key-frame. If it is
         * not a key-frame and there has been packet loss or a
         * key-frame has not been acquired prior then the image may
         * look blurred. */
        BOOL bKeyFrame;
        /** @brief A buffer allocated internally by TeamTalk. */
        VOID* frameBuffer;
        /** @brief The size in bytes of the buffer allocate in @a
         * frameBuffer. */
        INT32 nFrameBufferSize;
    } VideoFrame;

    /** 
     * @brief A struct containing the properties of a video capture
     * device.
     *
     * The information retrieved from the video capture device is used
     * to initialize the video capture device using the
     * #TT_InitVideoCaptureDevice function.
     * 
     * @see TT_GetVideoCaptureDevices */
    typedef struct VideoCaptureDevice
    {
        /** @brief A string identifying the device. */
        TTCHAR szDeviceID[TT_STRLEN];
        /** @brief The name of the capture device. */
        TTCHAR szDeviceName[TT_STRLEN];
        /** @brief The name of the API used to capture video. */ 
        TTCHAR szCaptureAPI[TT_STRLEN];
        /** @brief The supported capture formats. */
        CaptureFormat captureFormats[TT_CAPTUREFORMATS_MAX];
        /** @brief The number of capture formats available in @a
         * captureFormats array. */
        INT32 nCaptureFormatsCount; 
    } VideoCaptureDevice;
    /** @} */

    /** @addtogroup codecs
     * @{ */

    /** @brief Speex audio codec settings for Constant Bitrate mode
     * (CBR). The Speex codec is recommended for voice and uses less
     * bandwidth than #CELTCodec. @see SpeexVBRCodec */
    typedef struct SpeexCodec
    {
        /** @brief Set to 0 for 8 KHz (narrow band), set to 1 for 16 KHz 
         * (wide band), set to 2 for 32 KHz (ultra-wide band). */
        INT32 nBandmode;
        /** @brief A value from 1-10. As of DLL version 4.2 also 0 is
         * supported.*/
        INT32 nQuality;
        /** @brief Milliseconds of audio data in each packet. Speex
         * uses 20 msec frame sizes. Recommended is 40 ms. Min is 20,
         * max is 1000. */
        INT32 nMSecPerPacket;
        /** @brief Use Speex' jitter buffer for playback. Recommended
         * is FALSE. */
        BOOL bUseJitterBuffer;
        /** @brief Playback should be done in stereo. Doing so will
         * disable 3d-positioning.
         *
         * @see TT_SetUserPosition
         * @see TT_SetUserStereo */
        BOOL bStereoPlayback;
    } SpeexCodec;

    /** @brief Speex audio codec settings for Variable Bitrate mode
     * (VBR). The Speex codec is recommended for voice and uses less
     * bandwidth than #CELTCodec. The Speex VBR codec was introduced
     * in version 4.2. */
    typedef struct SpeexVBRCodec
    {
        /** @brief Set to 0 for 8 KHz (narrow band), set to 1 for 16 KHz 
         * (wide band), set to 2 for 32 KHz (ultra-wide band). */
        INT32 nBandmode;
        /** @brief A value from 0-10. If @c nBitRate is non-zero it
         * will override this value. */
        INT32 nQualityVBR;
        /** @brief The bitrate at which the audio codec should output
         * encoded audio data. Dividing it by 8 gives roughly the
         * number of bytes per second used for transmitting the
         * encoded data. For limits check out #SPEEX_NB_MIN_BITRATE,
         * #SPEEX_NB_MAX_BITRATE, #SPEEX_WB_MIN_BITRATE,
         * #SPEEX_WB_MAX_BITRATE, #SPEEX_UWB_MIN_BITRATE and
         * #SPEEX_UWB_MAX_BITRATE. Note that specifying @c nBitRate
         * will override nQualityVBR. */
        INT32 nBitRate;
        /** @brief The maximum bitrate at which the audio codec is
         * allowed to output audio. Set to zero if it should be
         * ignored. */
        INT32 nMaxBitRate;
        /** @brief Enable/disable discontinuous transmission. When
         * enabled Speex will ignore silence, so the bitrate will
         * become very low. */
        BOOL bDTX;
        /** @brief Milliseconds of audio data in each packet. Speex
         * uses 20 msec frame sizes. Recommended is 40 ms. Min is 20,
         * max is 1000. */
        INT32 nMSecPerPacket;
        /** @brief Use Speex' jitter buffer for playback. Recommended
         * is FALSE. */
        BOOL bUseJitterBuffer;
        /** @brief Playback should be done in stereo. Doing so will
         * disable 3d-positioning.
         *
         * @see TT_SetUserPosition
         * @see TT_SetUserStereo */
        BOOL bStereoPlayback; 
    } SpeexVBRCodec;

/** @brief The minimum bitrate for Speex codec in 8 KHz mode. Bandmode
 * = 0. */
#define SPEEX_NB_MIN_BITRATE 2150
/** @brief The maximum bitrate for Speex codec in 8 KHz mode. Bandmode
 * = 0. */
#define SPEEX_NB_MAX_BITRATE 24600
/** @brief The minimum bitrate for Speex codec in 16 KHz
 * mode. Bandmode = 1. */
#define SPEEX_WB_MIN_BITRATE 3950
/** @brief The maximum bitrate for Speex codec in 16 KHz
 * mode. Bandmode = 1. */
#define SPEEX_WB_MAX_BITRATE 42200
/** @brief The minimum bitrate for Speex codec in 32 KHz
 * mode. Bandmode = 2. */
#define SPEEX_UWB_MIN_BITRATE 4150
/** @brief The maximum bitrate for Speex codec in 32 KHz
 * mode. Bandmode = 2. */
#define SPEEX_UWB_MAX_BITRATE 44000

    /** @brief CELT audio codec settings. The CELT codec is
     * recommended for music and speech. It has a higher bandwidth
     * usage than #SpeexCodec. */
    typedef struct CELTCodec
    {
        /** @brief The sample rate to use. Sample rate must be in the
         * range 32000 - 96000 Hz. */
        INT32 nSampleRate;
        /** @brief Mono = 1 or stereo = 2. Note that echo
         * cancellation, denoising and AGC is not support when using
         * stereo.
         *
         * @see TT_EnableEchoCancellation()
         * @see TT_EnableDenoising()
         * @see TT_EnableAGC() */
        INT32 nChannels;
        /** @brief The bitrate at which the audio codec should output
         * encoded audio data. Dividing it by 8 gives roughly the
         * number of bytes per second used for transmitting the
         * encoded data. #CELT_MIN_BITRATE and #CELT_MAX_BITRATE
         * specifies bitrate limited. */
        INT32 nBitRate;
        /** @brief Milliseconds of audio data in each
         * packet. Recommended is 40 ms. Min is 20, max is 1000. */
        INT32 nMSecPerPacket;
    } CELTCodec;

    /** @brief CELT audio codec settings. The CELT codec is
     * recommended for music and speech. It has a higher bandwidth
     * usage than #SpeexCodec. */
    typedef struct CELTVBRCodec
    {
        /** @brief The sample rate to use. Sample rate must be in the
         * range 32000 - 96000 Hz. */
        INT32 nSampleRate;
        /** @brief Mono = 1 or stereo = 2. Note that echo
         * cancellation, denoising and AGC is not support when using
         * stereo.
         *
         * @see TT_EnableEchoCancellation()
         * @see TT_EnableDenoising()
         * @see TT_EnableAGC() */
        INT32 nChannels;
        /** @brief The bitrate at which the audio codec can output
         * encoded audio data. Dividing it by 8 gives roughly the
         * number of bytes per second used for transmitting the
         * encoded data. @see CELT_MIN_BITRATE @see
         * CELT_MAX_BITRATE */
        INT32 nBitRate;
        /** @brief Milliseconds of audio data in each
         * packet. Recommended is 40 ms. Min is 20, max is 1000. */
        INT32 nMSecPerPacket;
    } CELTVBRCodec;

/** @brief The minimum bitrate supported for CELT. */
#define CELT_MIN_BITRATE 35000
/** @brief The maximum bitrate supported for CELT. */
#define CELT_MAX_BITRATE 3000000

    /** 
     * @brief Theora video codec settings.
     *
     * Configuring the Theora codec so it fits an end-user's domain
     * can be trickly. A high resolution in #CaptureFormat will result
     * in much high bandwidth usage. Also using a high frame-rate will
     * cause much higher bandwidth usage.
     *
     * Note that width and height for a video frame encoded by Theora
     * must be a multiple of 16. If it is not then part of the picture
     * will be "cut" out. */
    typedef struct TheoraCodec
    {
        /** @brief The target bitrate in bits per second. If set to
         * zero the Theora codec will use variable bitrate (VBR). If
         * non-zero it will use constant bitrate (CBR). */
        INT32 nTargetBitRate; 
        /** @brief A value from 0 - 63. Higher value gives higher
         * quality. */
        INT32 nQuality;
    } TheoraCodec;

    /** @brief The codecs supported.
     * @see AudioCodec
     * @see VideoCodec */
    typedef enum Codec
    {
        /** @brief No codec specified. */
        NO_CODEC                    = 0,
        /** @brief Speex audio codec, http://www.speex.org @see
         * SpeexCodec */
        SPEEX_CODEC                 = 1,
        /** @brief CELT audio codec version 0.5.2 used in version 4.1 and 
         * prior, http://www.celt-codec.org.
         *
         * This codec is not supported in version 4.2 and later. */
        CELT_0_5_2_OBSOLETE_CODEC   = 2,
        /** @brief Theora video codec, http://www.theora.org @see
         * TheoraCodec */
        THEORA_CODEC                = 3,
        /** @brief Speex audio codec in VBR mode, http://www.speex.org
         * @see SpeexVBRCodec */
        SPEEX_VBR_CODEC             = 4,
        /** @brief CELT audio codec version 0.11.1, 
         * http://www.celt-codec.org @see CELTCodec */
        CELT_CODEC                  = 5,
        /** @brief CELT audio codec version 0.11.1 in VBR mode, 
         * http://www.celt-codec.org @see CELTVBRCodec */
        CELT_VBR_CODEC              = 6
    } Codec;

    /** @brief Struct used for specifying which audio codec a channel
     * uses. */
    typedef struct AudioCodec
    {
        /** @brief Specifies whether the member @a speex or @a celt holds
         * the codec settings. */
        Codec nCodec;  
        union
        {
            /** @brief Speex codec settings if @a nCodec is
             * #SPEEX_CODEC */
            SpeexCodec speex;
            /** @brief CELT codec settings if @a nCodec is
             * #CELT_CODEC */
            CELTCodec celt;
            /** @brief Speex codec settings if @a nCodec is
             * #SPEEX_VBR_CODEC */
            SpeexVBRCodec speex_vbr;
            /** @brief CELT codec settings if @a nCodec is
             * #CELT_VBR_CODEC */
            CELTVBRCodec celt_vbr;
        };
    } AudioCodec;

    /** @brief Common audio configuration which should be used by users
     * in the same #Channel.
     *
     * Users' audio levels may be diffent due to how their microphone
     * is configured in their OS. Automatic Gain Control (AGC) can be used
     * to ensure all users in the same channel have the same audio level.
     *
     * @see TT_DoMakeChannel()
     * @see TT_DoUpdateChannel()
     * @see TT_EnableAGC() */
    typedef struct AudioConfig
    {
        /** @brief Whether clients who join a #Channel should
         * automatically enable AGC with the settings specified @a
         * bGainLevel, @a @a nMaxIncDBSec, @a nMaxDecDBSec and @a
         * nMaxGainDB. If the local client instance has already
         * enabled the flag #CLIENT_SNDINPUT_AGC it will not enable
         * AGC automatically when joining the channel. */
        BOOL bEnableAGC;
        /** @brief A value from 0 to 32768. Default is 8000. */
        INT32 nGainLevel;
        /** @brief Used so volume should not be amplified too quickly 
         * (maximal gain increase in dB/second). Default is 12. */
        INT32 nMaxIncDBSec;
        /** @brief Negative value! Used so volume should not be attenuated
         * too quickly (maximal gain decrease in dB/second). Default is 
         * -40. */
        INT32 nMaxDecDBSec;
        /** @brief Ensure volume doesn't become too loud (maximal gain
         * in dB). Default is 30. */
        INT32 nMaxGainDB;
        /** @brief Whether clients who join the channel should automatically
         * enable denoising. If the local client instance has already
         * enabled the flag #CLIENT_SNDINPUT_DENOISING it will not
         * enable denoising automatically when joining a channel. */
        BOOL bEnableDenoise;
        /** @brief Negative value! Maximum attenuation of the noise in dB.
         * Default value is -30. */
        INT32 nMaxNoiseSuppressDB;
    } AudioConfig;

    /** @brief Struct used for specifying the video codec to use. */
    typedef struct VideoCodec
    {
        /** @brief Specifies member holds the codec settings. So far
         * there is only one video codec to choose from, namely @a
         * theora. */
        Codec nCodec;  
        union
        {
            /** @brief Theora codec settings if @a nCodec is
             * #THEORA_CODEC. */
            TheoraCodec theora;
        };
    } VideoCodec;
    /** @} */

    /** @addtogroup transmission
     * @{ */

    /**
     * @brief Enum specifying data transmission types. @see
     * TT_EnableTransmission @see TT_IsTransmitting */
    typedef enum TransmitType
    {
        /** @brief Transmitting nothing. */
        TRANSMIT_NONE  = 0x0,
         /** @brief Transmit audio. */
        TRANSMIT_AUDIO = 0x1,
        /** @brief Transmit video. */
        TRANSMIT_VIDEO = 0x2 
    } TransmitType;

    /** @brief A mask of data transmissions based on
     * #TransmitType. @see TT_EnableTransmission */
    typedef UINT32 TransmitTypes;    
    /** @} */

    /** @addtogroup server
     * @{ */

    /** 
     * @brief The rights users have once they have logged on to the
     * server.
     *
     * #ServerProperties holds the user rights in its \a uUserRights
     * member variable and is retrieved by calling
     * #TT_GetServerProperties once connected to the server.
     *
     * @see ServerProperties
     * @see TT_GetServerProperties */
    typedef enum UserRight
    {
        /** @brief Users who log onto the server has none of the
          * rights below. */
        USERRIGHT_NONE               = 0x0000, 
        /** @brief Users can log on without an account and by only
         * specifying the server password. */
        USERRIGHT_GUEST_LOGIN        = 0x0001, 
        /** @brief Users can see users in all other channels. This
         * option cannot be changed in a call to #TT_DoUpdateServer. */
        USERRIGHT_VIEW_ALL_USERS     = 0x0002,
        /** @brief Users are allowed to create channels. */ 
        USERRIGHT_CHANNEL_CREATION   = 0x0004,
        /** @brief Users can become operators of channels. */
        USERRIGHT_CHANNEL_OPERATORS  = 0x0008,
        /** @brief Users can use channel commands (text messages
         * prefixed with '/'. */
        USERRIGHT_CHANNEL_COMMANDS   = 0x0010, 
        /** @brief None-admins are allowed to broadcast messages to
         * all users. */
        USERRIGHT_CLIENT_BROADCAST   = 0x0020, 
         /** @brief Users are allowed to change subscriptions to other
          * users. */
        USERRIGHT_SUBSCRIPTIONS      = 0x0040, 
        /** @brief Users are allowed to forward audio packets through
         * server. */
        USERRIGHT_FORWARD_AUDIO      = 0x0080, 
        /** @brief Users are allowed to forward video packets through
         * server. */
        USERRIGHT_FORWARD_VIDEO      = 0x0100,
        /** @brief Allow multiple users to log on to the server with
         * the same #UserAccount. */
        USERRIGHT_DOUBLE_LOGIN       = 0x0200
    } UserRight;

    /** 
     * @brief A bitmask based on #UserRight for holding the rights users 
     * have who log on the server.
     * @see ServerProperties */
    typedef UINT32 UserRights;

    /** 
     * @brief A struct containing the properties of the server's
     * settings.
     *
     * The server properties is available after a successful call to
     * #TT_DoLogin
     *
     * @see TT_DoUpdateServer
     * @see TT_GetServerProperties 
     * @see TT_Login
     * @see UserRight */
    typedef struct ServerProperties
    {
        /** @brief The server's name. */
        TTCHAR szServerName[TT_STRLEN];
        /** @brief The server's password to login. Users must provide
         * this in the #TT_DoLogin command. When extracted through
         * #TT_GetServerProperties the password will only be set for users of 
         * user-type #USERTYPE_ADMIN. */
        TTCHAR szServerPasswd[TT_STRLEN];
        /** @brief The message of the day. When updating the MOTD an admin
         * can use the variables %users% (number of users), %admins% (number
         * of admins), %uptime% (hours, minutes and seconds the server has
         * been online), %voicetx% (KBytes transmitted), %voicerx% (KBytes
         * received) and %lastuser% (nickname of last user to log on to the
         * server) as part of the MOTD. */
        TTCHAR szMOTD[TT_STRLEN];
        /** @brief A bitmask based on #UserRight which specifies the rights 
         * a user have who logs onto the server. */
        UserRights uUserRights;
        /** @brief The maximum number of users allowed on the server.  A user
         * with admin account can ignore this */
        INT32 nMaxUsers;
        /** @brief Bandwidth restriction for audio codecs created by 
         * non-administrators. This value will hold the highest bitrate which 
         * is allowed for audio codecs. 0 = no limit. @see AudioCodec */
        INT32 nAudioCodecBpsLimit;
        /** @brief The maximum number of bytes per second which the server 
         * will allow for audio packets. If this value is exceeded the server
         * will start dropping audio packets. 0 = disabled. */
        INT32 nMaxAudioTxPerSecond;
        /** @brief The maximum number of bytes per second which the server 
         * will allow for audio packets. If this value is exceeded the server
         * will start dropping video packets. 0 = disabled. */
        INT32 nMaxVideoTxPerSecond;
        /** @brief The amount of bytes per second which the server 
         * will allow for packet forwarding.  If this value is exceeded the server
         * will start dropping packets. 0 = disabled. */
        INT32 nMaxTotalTxPerSecond;
        /** @brief Whether the server automatically saves changes */
        BOOL bAutoSave;
        /** @brief The server's TCP port. */
        INT32 nTcpPort;
        /** @brief The server's UDP port. */
        INT32 nUdpPort;
        /** @brief The number of seconds before a user who hasn't
         * responded to keepalives will be kicked off the server. @see
         * TT_SetKeepAliveInterval. */
        INT32 nUserTimeout;
        /** @brief The server version. */
        TTCHAR szServerVersion[TT_STRLEN];
        /** @brief The version of the server's protocol. */
        TTCHAR szServerProtocolVersion[TT_STRLEN];
    } ServerProperties;

    /**
     * @brief A struct containing the server's statistics,
     * i.e. bandwidth usage and user activity.
     *
     * Use TT_DoQueryServerStats() to query the server's statistics
     * and when the command completes use TT_GetServerStatistics() to
     * extract the statistics. */
    typedef struct ServerStatistics
    {
        /** @brief The number of bytes sent from the server to
         * clients. */
        INT64 nTotalBytesTX;
        /** @brief The number of bytes received by the server from
         * clients. */
        INT64 nTotalBytesRX;
        /** @brief The number of bytes in audio packets sent from the
         *  server to clients. */
        INT64 nVoiceBytesTX;
        /** @brief The number of bytes in audio packets received by
         *  the server from clients. */
        INT64 nVoiceBytesRX;
        /** @brief The number of bytes in video packets sent from the
         *  server to clients. */
        INT64 nVideoBytesTX;
        /** @brief The number of bytes in video packets received by
         *  the server from clients. */
        INT64 nVideoBytesRX;
        /** @brief The server's uptime in msec. */
        INT64 nUptimeMSec;
    } ServerStatistics;

    /**
     * @brief A struct containing the properties of a banned user.
     * This struct is used by #TT_GetBannedUsers.
     * @see TT_GetBannedUsers */
    typedef struct BannedUser
    {
        /** @brief IP-address of banned user. */
        TTCHAR szIpAddress[TT_STRLEN]; 
        /** @brief Channel where user was located when banned. */
        TTCHAR szChannelPath[TT_STRLEN]; 
        /** @brief Date and time when user was banned. */
        TTCHAR szBanTime[TT_STRLEN];
        /** @brief Nickname of banned user. */
        TTCHAR szNickname[TT_STRLEN];
        /** @brief Username of banned user. */
        TTCHAR szUsername[TT_STRLEN];
    } BannedUser;

    /** @ingroup users
     * @brief The types of users supported. 
     * @see User @see UserAccount */
    typedef enum UserType
    {
        /** @brief Used internally to denote an unauthenticated
         * user. */
        USERTYPE_NONE    = 0x0, 
        /** @brief A default user who can join channels. */
        USERTYPE_DEFAULT = 0x01, 
        /** @brief A user with administrator privileges. */
        USERTYPE_ADMIN   = 0x02 
    } UserType;

    /** @ingroup users
     * @brief A bitmask based on #UserType describing the user type.
     * @see UserType */
    typedef UINT32 UserTypes;

    /** 
     * @brief A struct containing the properties of a user account.
     *
     * A registered user is one that has a user account on the server.
     *
     * @see TT_DoListUserAccounts
     * @see TT_DoNewUserAccount
     * @see TT_DoDeleteUserAccount */
    typedef struct UserAccount
    {
        /** @brief The account's username. */
        TTCHAR szUsername[TT_STRLEN];
        /** @brief The account's password. */
        TTCHAR szPassword[TT_STRLEN];
        /** @brief A bitmask of the type of user based on #UserType. */
        UserTypes uUserType;
        /** @brief A user data field which can be used for additional
         * information. The @a nUserData field of the #User struct will
         * contain this value when a user who logs in with this account. */
        INT32 nUserData;
        /** @brief Additional notes about this user. */
        TTCHAR szNote[TT_STRLEN];
    } UserAccount;
    /** @} */

    /** @addtogroup users
     * @{ */

    /** 
     * @brief A user by default accepts audio, video and text messages
     * from all users. Using subscribtions can, however, change what
     * the local client instance is willing to accept from other
     * users.
     *
     * By calling #TT_DoSubscribe and #TT_DoUnsubscribe the local
     * client instance can tell the server (and thereby remote users)
     * what he is willing to accept from other users.
     *
     * To check what a user subscribes to check out the #User struct's
     * @a uLocalSubscriptions. The subscriptions with the prefix
     * @c SUBSCRIBE_INTERCEPT_* options can be used to spy on users and
     * receive data from them even if one is not participating in the
     * same channel as they are.
     *
     * @see TT_DoSubscribe
     * @see TT_DoUnsubscribe */
    typedef enum Subscription
    {
        /** @brief No subscriptions. */
        SUBSCRIBE_NONE                    = 0x0000,
        /** @brief Subscribing to user messages. */
        SUBSCRIBE_USER_MSG                = 0x0001,
        /** @brief Subscribing to channel messages. */
        SUBSCRIBE_CHANNEL_MSG             = 0x0002,
        /** @brief Subscribing to broadcast messsages. */
        SUBSCRIBE_BROADCAST_MSG           = 0x0004,
        /** @brief Subscribing to audio. */
        SUBSCRIBE_AUDIO                   = 0x0008,
        /** @brief Subscribing to video. */
        SUBSCRIBE_VIDEO                   = 0x0010,
        /** @brief Intercept all user text messages sent by a
        * user. Only user-type #USERTYPE_ADMIN can do this. */
        SUBSCRIBE_INTERCEPT_USER_MSG      = 0x0100,
        /** @brief Intercept all channel messages sent by a user. Only
        * user-type #USERTYPE_ADMIN can do this. */
        SUBSCRIBE_INTERCEPT_CHANNEL_MSG   = 0x0200,
        /* SUBSCRIBE_INTERCEPT_BROADCAST_MSG = 0x0400, */
        /** @brief Intercept all audio sent by a user. Only user-type
         * #USERTYPE_ADMIN can do this. By enabling this subscription an
         * administrator can listen to audio sent by users outside his
         * own channel. */
        SUBSCRIBE_INTERCEPT_AUDIO         = 0x0800,
        /** @brief Intercept all video sent by a user. Only user-type
         * #USERTYPE_ADMIN can do this. By enabling this subscription an
         * administrator can receive video frames sent by users
         * outside his own channel. */
        SUBSCRIBE_INTERCEPT_VIDEO         = 0x1000,
    } Subscription;

    /** 
     * @brief A bitmask based on #Subscription describing which 
     * subscriptions are enabled.
     * @see Subscription */
    typedef UINT32 Subscriptions;

    /** @brief The possible states for a user. Used for #User's @a
     * uUserState variable. */
    typedef enum UserState
    {
        /** @brief The user is in initial state. */
        USERSTATE_NONE          = 0x00,
        /** @brief If set the user is currently talking. Basically
         * means the event #WM_TEAMTALK_USER_TALKING has been posted for
         * this user. This flag doesn't apply to "myself". For
         * "myself" use #TT_IsTransmitting */
        USERSTATE_TALKING       = 0x01,
        /** @brief If set the user is muted. @see TT_SetUserMute */
        USERSTATE_MUTE          = 0x02,
        /** @brief Whether a peer to peer connection has been
         * established to this user.  @see WM_TEAMTALK_CON_P2P */
        USERSTATE_P2P_CONNECTED = 0x04
    } UserState;

    /** @brief A bitmask based on #UserState indicating a #User's current
     * state. */
    typedef UINT32 UserStates;

    /** 
     * @brief A struct containing the properties of a user.
     * @see UserType
     * @see TT_GetUser */
    typedef struct User
    {
        /** @brief The user's ID. A value from 1 - 65535. */
        INT32 nUserID;
         /** @brief The user's nickname. @see TT_DoChangeNickname */
        TTCHAR szNickname[TT_STRLEN];
        /** @brief Username of user's account. */
        TTCHAR szUsername[TT_STRLEN];
        /** @brief The user's currently status mode. @see
         * TT_DoChangeStatusMode */
        INT32 nStatusMode;
        /** @brief The user's current status message. @see
         * TT_DoChangeStatusMode */
        TTCHAR szStatusMsg[TT_STRLEN];
        /** @brief The channel the user is currently participating
         * in. 0 if none. */
        INT32 nChannelID; 
        /** @brief The user's IP-address. */ 
        TTCHAR szIPAddress[TT_STRLEN];
        /** @brief The user's client version. */ 
        TTCHAR szVersion[TT_STRLEN];
        /** @brief A bitmask of the type of user based on #UserType. */
        UserTypes uUserType;
        /** @brief A bitmask of the user's current state, e.g. talking, muted,
         * etc. */
        UserStates uUserState;
        /** @brief A bitmask of what the local user subscribes to from
         * this user.  @see TT_DoSubscribe */
        Subscriptions uLocalSubscriptions;
        /** @brief A bitmask of what this user subscribes to from local
         * user.  @see TT_DoSubscribe */
        Subscriptions uPeerSubscriptions;
        /** @brief The @a nUserData of the #UserAccount used by the user to
         * log in. This field can be use to denote e.g. a database ID. */
        INT32 nUserData;
        /** @brief Store audio received from this user to this
         * folder. @see TT_SetUserAudioFolder */
        TTCHAR szAudioFolder[TT_STRLEN];
    } User;

    /**
     * @brief Packet reception and data statistics for a user.
     * 
     * @see TT_GetUserStatistics */
    typedef struct UserStatistics
    { 
        /** @brief Number of audio packets received from user. */
        INT64 nAudioPacketsRecv;
        /** @brief Number of audio packets lost from user. */
        INT64 nAudioPacketsLost;
        /** @brief Number of video packets received from user. A video 
         * frame can consist of several video packets. */
        INT64 nVideoPacketsRecv;
        /** @brief Number of video frames received from user. */
        INT64 nVideoFramesRecv;
        /** @brief Video frames which couldn't be shown because packets were
         * lost. */
        INT64 nVideoFramesLost;
        /** @brief Number of video frames dropped because user application  
         * didn't retrieve video frames in time. */
        INT64 nVideoFramesDropped;
    } UserStatistics;

    /** 
     * @brief Text message types.
     * 
     * The types of messages which can be passed to #TT_DoTextMessage().
     *
     * @see TextMessage
     * @see TT_DoTextMessage
     * @see TT_GetTextMessage */ 
    typedef enum TextMsgType
    {
         /** @brief A User to user text message. */
        MSGTYPE_USER      = 1,
        /** @brief A User to channel text message. */
        MSGTYPE_CHANNEL   = 2,
         /** @brief A broadcast message. Only admins can send
          * broadcast messages unless #USERRIGHT_CLIENT_BROADCAST is
          * enabled. */
        MSGTYPE_BROADCAST = 3
    } TextMsgType;

    /** 
     * @brief A struct containing the properties of a text message
     * sent by a user.
     *
     * @see WM_TEAMTALK_CMD_USER_TEXTMSG
     * @see TT_DoTextMessage
     * @see TT_GetTextMessage */
    typedef struct TextMessage
    {
        /** @brief The type of text message. */
        TextMsgType nMsgType;
        /** @brief Will be set automatically on outgoing message. */
        INT32 nFromUserID;
        /** @brief The originators username. */
        TTCHAR szFromUsername[TT_STRLEN];
        /** @brief Set to zero if channel message. */
        INT32 nToUserID;
        /** @brief Set to zero if @a nMsgType is #MSGTYPE_USER or
         * #MSGTYPE_BROADCAST. */
        INT32 nChannelID;
        /** @brief The actual text message. The message can be
         * multi-line (include EOL).  */
        TTCHAR szMessage[TT_STRLEN];
    } TextMessage;
    /** @} */

    /** @addtogroup channels
     * @{ */

    /**
     * @brief The types of channels supported. @see Channel */
    typedef enum ChannelType
    {
        /** @brief A default channel is a channel which disappears
         * after the last user leaves the channel. */
        CHANNEL_DEFAULT             = 0x0000,
        /** @brief A channel which persists even when the last user
         * leaves the channel. */
        CHANNEL_STATIC              = 0x0001,
        /** @brief Only one user can transmit at a time. Note that
         * this option doesn't apply if clients are running in peer to
         * peer mode. @see #TT_EnablePeerToPeer */
        CHANNEL_SOLO_TRANSMIT       = 0x0002,
         /** @brief Audio transmitted to the channel by the client
          * instance is also sent back and played by the client
          * instance. */
        CHANNEL_ECHO                = 0x0004,
        /** @brief Voice and video transmission in the channel is controlled 
         * by a channel operator (or an administrator). For a user to
         * transmit audio or video to this type of channel the channel operator must
         * add the user's ID to either @a voiceUsers or @a videoUsers in the
         * #Channel struct and call #TT_DoUpdateChannel.
         *
         * @note
         * Requires server version 4.1.0.994 or later.
         *
         * @see TT_IsChannelOperator
         * @see #USERTYPE_ADMIN */
        CHANNEL_CLASSROOM           = 0x0008
    } ChannelType;

    /** 
     * @brief Bitmask of #ChannelType. */
    typedef UINT32 ChannelTypes;

    /** 
     * @brief A struct containing the properties of a channel.
     * @see TT_GetChannel
     * @see ChannelType
     * @see AudioCodec */
    typedef struct Channel
    {
        /** @brief Parent channel ID. 0 means no parent channel,
         * i.e. it's the root channel. */
        INT32 nParentID;
        /** @brief The channel's ID. A value from 1 - 65535. */
        INT32 nChannelID;
        /** @brief Name of the channel. */
        TTCHAR szName[TT_STRLEN];
        /** @brief Topic of the channel. */
        TTCHAR szTopic[TT_STRLEN];
        /** @brief Password to join the channel. When extracted through
         * #TT_GetChannel the password will only be set for users of 
         * user-type #USERTYPE_ADMIN. */
        TTCHAR szPassword[TT_STRLEN];
        /** @brief Whether password is required to join channel */
        BOOL bPassword;
        /** @brief A bitmask of the type of channel based on #ChannelType. */
        ChannelTypes uChannelType;
        /** @brief Number of bytes available for file storage. */
        INT64 nDiskQuota;
        /** @brief Operator password, i.e. for use with '/opme'
         * command. @see USERRIGHT_CHANNEL_COMMANDS */
        TTCHAR szOpPassword[TT_STRLEN];
        /** @brief Max number of users in channel. */
        INT32 nMaxUsers;
        /** @brief The audio codec used by users in the channel. */
        AudioCodec codec;
        /** @brief The audio configuration which users who join the channel
         * should use. If the audio configuration forces some options, e.g. 
         * AGC and denoising then these options will automatically be enabled
         * and override what is currently set by TT_EnableDenoising() and 
         * TT_EnableAGC().
         *
         * @note
         * Requires server and client version 4.1.0.1127 or later. */
        AudioConfig audiocfg;
        /** @brief The IDs of users who are allowed to transmit voice 
         * data to the channel. This setting only applies to channels of type 
         * #CHANNEL_CLASSROOM. Only channel operators and administrators are
         * allowed to change the users who are allowed to transmit data to a 
         * channel. Call #TT_DoUpdateChannel to update the list of users who
         * are allowed to transmit data to the channel.
         *
         * @note
         * Requires server version 4.1.0.994 or later.
         *
         * @see TT_IsChannelOperator
         * @see TT_DoChannelOp
         * @see TRANSMIT_AUDIO */
        INT32 voiceUsers[TT_VOICEUSERS_MAX];
        /** @brief The IDs of users who are allowed to transmit video 
         * data to the channel. This setting only applies to channels of type 
         * #CHANNEL_CLASSROOM. Only channel operators and administrators are
         * allowed to change the users who are allowed to transmit data to a 
         * channel. Call #TT_DoUpdateChannel to update the list of users who
         * are allowed to transmit data to the channel.
         *
         * @note
         * Requires server version 4.1.0.994 or later.
         *
         * @see TT_IsChannelOperator
         * @see TT_DoChannelOp
         * @see TRANSMIT_VIDEO */
        INT32 videoUsers[TT_VIDEOUSERS_MAX];
    } Channel;

    /** 
     * @brief A struct containing the properties of a file transfer.
     * @see TT_GetFileTransferInfo */
    typedef struct FileTransfer
    {
        /** @brief The ID identifying the file transfer. */
        INT32 nTransferID;
        /** @brief The channel where the file is/will be located. */
        INT32 nChannelID;
        /** @brief The file path on local disk. */
        TTCHAR szLocalFilePath[TT_STRLEN];
        /** @brief The filename in the channel. */
        TTCHAR szRemoteFileName[TT_STRLEN];
        /** @brief The size of the file being transferred. */
        INT64 nFileSize;
        /** @brief The number of bytes transferred so far. */
        INT64 nTransferred;
        /** @brief TRUE if download and FALSE if upload. */
        BOOL bInbound;
    } FileTransfer;

    /** @brief Status of a file transfer.
     * @see WM_TEAMTALK_FILETRANSFER */
    typedef enum FileTransferStatus
    {
        /** @brief Error during file transfer. */
        FILETRANSFER_ERROR      = 0,
        /** @brief File transfer started. */
        FILETRANSFER_STARTED    = 1,
        /** @brief File transfer finished. */
        FILETRANSFER_FINISHED   = 2
    } FileTransferStatus;

    /**
     * @brief A struct containing the properties of a file in a #Channel.
     * @see TT_GetChannelFileInfo */
    typedef struct FileInfo
    {
        /** @brief The ID identifying the file. */
        INT32 nFileID;
        /** @brief The name of the file. */
        TTCHAR szFileName[TT_STRLEN];
        /** @brief The size of the file. */
        INT64 nFileSize;
        /** @brief Username of the person who uploaded the files. */
        TTCHAR szUsername[TT_STRLEN];
    } FileInfo;
    /** @} */

    /** @ingroup sounddevices
     * @brief Status of audio files being written to disk.
     * @see WM_TEAMTALK_USER_AUDIOFILE */
    typedef enum AudioFileStatus
    {
        /** @brief Error while processing audio file. */
        AFS_ERROR           = 0,
        /** @brief Started processing audio file. */
        AFS_STARTED         = 1,
        /** @brief Finished processing audio file. */
        AFS_FINISHED        = 2,
        /** @brief Aborted processing of audio file. */
        AFS_ABORTED         = 3
    } AudioFileStatus;

    /**
     * @brief Audio file formats supported for muxed audio recordings.
     * @see TT_StartRecordingMuxedAudioFile() */
    typedef enum AudioFileFormat
    {
        /** @brief Used to denote nothing selected. */
        AFF_NONE                 = 0,
        /** @brief Store in 16-bit wave format. */
        AFF_WAVE_FORMAT          = 2,
        /** @brief Store in MP3-format. 
         * This requires lame_enc.dll to
         * be in the same directory as the application's
         * execuable. The LAME DLLs can be obtained from
         * http://lame.sourceforge.net. Note that the MP3-format is
         * subject to licensing by Fraunhofer and Thomson
         * Multimedia. */
        AFF_MP3_64KBIT_FORMAT    = 3,
        /** @see #AFF_MP3_64KBIT_FORMAT */
        AFF_MP3_128KBIT_FORMAT   = 4,
        /** @see #AFF_MP3_64KBIT_FORMAT */
        AFF_MP3_256KBIT_FORMAT   = 5
    } AudioFileFormat;

    /** @ingroup connectivity
     * @brief Statistics of bandwidth usage and ping times in the local 
     * client instance.
     * @see TT_GetStatistics */
    typedef struct ClientStatistics
    {
        /** @brief Bytes sent on UDP. */
        INT64 nUdpBytesSent;
        /** @brief Bytes received on UDP. */
        INT64 nUdpBytesRecv;
        /** @brief Voice data sent (on UDP). */
        INT64 nVoiceBytesSent;
        /** @brief Voice data received (on UDP). */
        INT64 nVoiceBytesRecv;
        /** @brief Video data sent (on UDP). */
        INT64 nVideoBytesSent;
        /** @brief Video data received (on UDP). */
        INT64 nVideoBytesRecv;
        /** @brief Response time to server on UDP (based on ping/pong
         * sent at a specified interval. Set to -1 if not currently
         * available. @see TT_SetKeepAliveInterval */
        INT32 nUdpPingTimeMs;
        /** @brief Response time to server on TCP (based on ping/pong
         * sent at a specified interval. Set to -1 if not currently
         * available.  @see TT_SetKeepAliveInterval */
        INT32 nTcpPingTimeMs;
        /** @brief The number of seconds nothing has been received by
         * the client on TCP. */
        INT32 nTcpServerSilenceSec;
        /** @brief The number of seconds nothing has been received by
         * the client on UDP. */
        INT32 nUdpServerSilenceSec;
    } ClientStatistics;

    /** @addtogroup errorhandling
     * @{ */

    /** 
     * @brief Errors which can occur either as a result of client
     * commands or as a result of internal errors.
     *
     * Use #TT_GetErrorMessage to get a text-description of the
     * error. */
    typedef enum ClientError
    {
        /** @brief Command indicating success. Only used internally. */
        CMDERR_SUCCESS = 0,

        /* COMMAND ERRORS 1000-1999 ARE DUE TO INVALID OR UNSUPPORTED
         * COMMANDS */

        /** @brief Command has syntax error. Only used internally. */
        CMDERR_SYNTAX_ERROR = 1000,
        /** @brief The server doesn't support the issued command.
         *
         * This error may occur if the server is an older version than
         * the client instance. */
        CMDERR_UNKNOWN_COMMAND = 1001,
        /** @brief Command cannot be performed due to missing
         * parameter. Only used internally. */
        CMDERR_MISSING_PARAMETER = 1002,
        /** @brief The server uses a protocol which is incompatible
         * with the client instance. */
        CMDERR_INCOMPATIBLE_PROTOCOLS = 1003,
        /** @brief The server does not support the audio codec specified
         * by the client. Introduced in version 4.1.0.1264. 
         * @see TT_DoMakeChannel
         * @see TT_DoJoinChannel */
        CMDERR_UNKNOWN_AUDIOCODEC = 1004,

        /* COMMAND ERRORS 2000-2999 ARE DUE TO INSUFFICIENT RIGHTS */

        /** @brief Invalid server password. 
         *
         * The #TT_DoLogin command passed a server password which was
         * invalid.  @see TT_DoLogin */
        CMDERR_INCORRECT_SERVER_PASSWORD = 2000,
        /** @brief Invalid channel password. 
         *
         * The #TT_DoJoinChannel or #TT_DoJoinChannelByID passed an
         * invalid channel password. #TT_DoMakeChannel can also cause
         * a this error if the password is longer than #TT_STRLEN. */
        CMDERR_INCORRECT_CHANNEL_PASSWORD = 2001,
        /** @brief Invalid username or password for account.
         *
         * The #TT_DoLogin command was issued with invalid account
         * properties. This error can also occur by
         * #TT_DoNewUserAccount if username is empty. */
        CMDERR_INVALID_ACCOUNT = 2002,
        /** @brief Login failed due to maximum number of users on
         * server.
         *
         * #TT_DoLogin failed because the server does not allow any
         * more users. */
        CMDERR_MAX_SERVER_USERS_EXCEEDED = 2003,
        /** @brief Cannot join channel because it has maximum number
         * of users.
         *
         * #TT_DoJoinChannel or #TT_DoJoinChannelByID failed because
         * no more users are allowed in the channel. */
        CMDERR_MAX_CHANNEL_USERS_EXCEEDED = 2004,
        /** @brief IP-address has been banned from server.
         *
         * #TT_DoLogin failed because the local client's IP-address
         * has been banned on the server. */
        CMDERR_SERVER_BANNED = 2005,
        /** @brief Command not authorized.
         *
         * The command cannot be performed because the client instance
         * has insufficient rights.
         *
         * @see TT_DoDeleteFile
         * @see TT_DoJoinChannel
         * @see TT_DoJoinChannelByID
         * @see TT_DoLeaveChannel
         * @see TT_DoChannelOp
         * @see TT_DoChannelOpEx
         * @see TT_DoKickUser
         * @see TT_DoUpdateChannel
         * @see TT_DoChangeNickname
         * @see TT_DoChangeStatus
         * @see TT_DoTextMessage
         * @see TT_DoSubscribe
         * @see TT_DoUnsubscribe
         * @see TT_DoMakeChannel
         * @see TT_DoRemoveChannel
         * @see TT_DoMoveUser
         * @see TT_DoUpdateServer
         * @see TT_DoSaveConfig
         * @see TT_DoSendFile 
         * @see TT_DoRecvFile 
         * @see TT_DoBanUser
         * @see TT_DoUnBanUser
         * @see TT_DoListBans
         * @see TT_DoListUserAccounts
         * @see TT_DoNewUserAccount
         * @see TT_DoDeleteUserAccount */
        CMDERR_NOT_AUTHORIZED = 2006,
         /** @brief Server doesn't allow users to create channels.
          *
          * #TT_DoJoinChannel or #TT_DoJoinChannelByID failed because
          * #USERRIGHT_CHANNEL_CREATION is not enabled.  
          * @see ServerProperties */
        CMDERR_CANNOT_CREATE_CHANNELS = 2007,
        /** @brief Cannot upload file because disk quota will be exceeded.
         *
         * #TT_DoSendFile was not allowed because there's not enough
         * disk space available for upload.
         *
         * @see Channel */
        CMDERR_MAX_DISKUSAGE_EXCEEDED = 2008,
        /** @brief Modifying subscriptions not enabled.
         *
         * #TT_DoSubscribe or #TT_DoUnsubscribe failed because the
         * server does not allow users to change subscriptions. The
         * USERRIGHT_SUBSCRIPTIONS is not enabled in the server's
         * #ServerProperties.
         *
         * @see ServerProperties */
        CMDERR_SUBSCRIPTIONS_DISABLED = 2009,
        /** @brief Invalid password for becoming channel operator.
         * 
         * The password specified in #TT_DoChannelOpEx is not correct.
         * The operator password is the @a szOpPassword of the 
         * #Channel-struct. */
        CMDERR_INCORRECT_OP_PASSWORD = 2010,

        /** @brief The selected #AudioCodec exceeds what the server allows.
         *
         * A server can limit the vitrate of audio codecs if @c 
         * nAudioCodecBpsLimit of #ServerProperties is specified. */
        CMDERR_AUDIOCODEC_BITRATE_LIMIT_EXCEEDED = 2011,

        /* COMMAND ERRORS 3000-3999 ARE DUE TO INVALID STATE OF CLIENT INSTANCE */

        /** @brief Client instance has not been authenticated.
         * 
         * #TT_DoLogin has not been issued successfully or
         * #TT_DoLogout could not be performed because client
         * instance is already logged in.*/
        CMDERR_NOT_LOGGEDIN = 3000,

        /** @brief Already logged in.
         *
         * #TT_DoLogin cannot be performed twice. */
        CMDERR_ALREADY_LOGGEDIN = 3001,
        /** @brief Cannot leave channel because not in channel.
         *
         * #TT_DoLeaveChannel failed because user is not in a channel. */
        CMDERR_NOT_IN_CHANNEL = 3002,
        /** @brief Cannot join same channel twice.
         * 
         * #TT_DoJoinChannel or #TT_DoJoinChannelByID failed because
         * client instance is already in the specified channel. */
        CMDERR_ALREADY_IN_CHANNEL = 3003,
        /** @brief Channel already exists.
         *
         * #TT_DoMakeChannel failed because channel already exists. */
        CMDERR_CHANNEL_ALREADY_EXISTS = 3004,
        /** @brief Channel does not exist.
         *
         * Command failed because channel does not exists.
         * @see TT_DoRemoveChannel
         * @see TT_DoUpdateChannel
         * @see TT_DoMakeChannel Due to invalid channel name
         * @see TT_DoSendFile
         * @see TT_DoRecvFile
         * @see TT_DoDeleteFile
         * @see TT_DoJoinChannel
         * @see TT_DoJoinChannelByID
         * @see TT_DoLeaveChannel
         * @see TT_DoChannelOp
         * @see TT_DoKickUser
         * @see TT_DoBanUser
         * @see TT_DoMoveUser
         * @see TT_DoTextMessage */
        CMDERR_CHANNEL_NOT_FOUND = 3005,
        /** @brief User not found.
         * 
         * Command failed because user does not exists.
         * @see TT_DoChannelOp
         * @see TT_DoKickUser
         * @see TT_DoBanUser
         * @see TT_DoMoveUser
         * @see TT_DoTextMessage
         * @see TT_DoSubscribe
         * @see TT_DoUnsubscribe */
        CMDERR_USER_NOT_FOUND = 3006,
        /** @brief Banned IP-address does not exist.
         * 
         * #TT_DoUnBanUser failed because there is no banned
         * IP-address which matches what was specified. */
        CMDERR_BAN_NOT_FOUND = 3007,
        /** @brief File transfer doesn't exists.
         *
         * #TT_DoSendFile or #TT_DoRecvFile failed because the server
         * cannot process the file transfer. */
        CMDERR_FILETRANSFER_NOT_FOUND = 3008,
        /** @brief Server failed to open file.
         *
         * #TT_DoSendFile or #TT_DoRecvFile failed because the server
         * cannot open the specified file (possible file lock). */
        CMDERR_OPENFILE_FAILED = 3009,
        /** @brief Cannot find user account.
         * 
         * #TT_DoDeleteUserAccount failed because the specified user
         * account does not exists. */
        CMDERR_ACCOUNT_NOT_FOUND = 3010,
        /** @brief File does not exist.
         *
         * #TT_DoSendFile, #TT_DoRecvFile or #TT_DoDeleteFile failed
         * because the server cannot find the specified file. */
        CMDERR_FILE_NOT_FOUND = 3011,
        /** @brief File already exist.
         *
         * #TT_DoSendFile failed because the file already exists in
         * the channel. */
        CMDERR_FILE_ALREADY_EXISTS = 3012,
        /** @brief Server does not allow file transfers.
         *
         * #TT_DoSendFile or #TT_DoRecvFile failed because the server
         * does not allow file transfers. */
        CMDERR_FILESHARING_DISABLED = 3013,
        /** @brief Cannot process command since server is not empty.
         * @see TT_DoUpdateServer Ensure not to update 
         * #USERRIGHT_VIEW_ALL_USERS. */
        CMDERR_SERVER_HAS_USERS = 3014,
        /** @brief Cannot process command since channel is not empty.
         * @see TT_DoUpdateChannel #AudioCodec cannot be changed while
         * there are users in a channel. */
        CMDERR_CHANNEL_HAS_USERS = 3015,

        /* ERRORS 10000-10999 ARE NOT COMMAND ERRORS BUT INSTEAD
         * ERRORS IN THE CLIENT INSTANCE. */

        /** @brief A sound input device failed. 
         *
         * This can e.g. happen when joining a channel and the sound
         * input device has been unplugged. 
         * 
         * Call #TT_CloseSoundInputDevice and TT_InitSoundInputDevice
         * with a valid #SoundDevice to releave the problem. */
        INTERR_SNDINPUT_FAILURE = 10000,
        /** @brief A sound output device failed.
         *
         * This can e.g. happen when joining a channel and the sound
         * output device has been unplugged. Note that it can be posted
         * multiple times if there's several users in the channel.
         *
         * Call #TT_CloseSoundOutputDevice and TT_InitSoundOutputDevice
         * with a valid #SoundDevice to releave the problem. */
        INTERR_SNDOUTPUT_FAILURE = 10001,
    } ClientError;
    /** @} */

    /** @addtogroup initclient
     * @{ */

    /**
     * @brief Flags used to describe the the client instance current
     * state.
     *
     * The client's state is a bitmask of the flags in #ClientFlag.
     *
     * The state of the client instance can be retrieved by calling
     * #TT_GetFlags. This enables the user application to display the
     * possible options to the end user. If e.g. the flag
     * #CLIENT_AUTHORIZED is not set it will not be possible to
     * perform any other commands except #TT_DoLogin. Doing so will
     * make the server return an error message to the client. */
    typedef enum ClientFlag
    {
        /** @brief The client instance (#TTInstance) is in closed
         * state, i.e. #TT_InitTeamTalk has return a valid instance
         * ready for use but no operations has been performed on
         * it. */
        CLIENT_CLOSED                    = 0x00000000,
        /** @brief If set the client instance's sound input device has
         * been initialized, i.e. #TT_InitSoundInputDevice has been
         * called successfully. */
        CLIENT_SNDINPUT_READY            = 0x00000001,
        /** @brief If set the client instance's sound output device
         * has been initialized, i.e. #TT_InitSoundOutputDevice has
         * been called successfully. */
        CLIENT_SNDOUTPUT_READY           = 0x00000002,
        /** @brief If set the client instance's video device has been
         * initialized, i.e. #TT_InitVideoCaptureDevice has been
         * called successfuly. */
        CLIENT_VIDEO_READY               = 0x00000004,
        /** @brief If set the client instance will start transmitting
         * audio if the sound level is above the voice activation
         * level. The event #WM_TEAMTALK_VOICE_ACTIVATION is posted
         * when voice activation initiates transmission.
         * @see TT_SetVoiceActivationLevel
         * @see TT_EnableVoiceActivation */
        CLIENT_SNDINPUT_VOICEACTIVATED   = 0x00000010,
        /** @brief If set the client instance will try to remove noise
         * from recorded audio. @see TT_EnableDenoising */
        CLIENT_SNDINPUT_DENOISING        = 0x00000020,
        /** @brief If set the client instance is using automatic gain 
         * control. @see TT_EnableAGC */
        CLIENT_SNDINPUT_AGC              = 0x00000040,
        /** @brief If set the client instance has muted all users.
        * @see TT_SetSoundOutputMute */
        CLIENT_SNDOUTPUT_MUTE            = 0x00000080,
        /** @brief If set the client instance will auto position users
        * in a 180 degree circle using 3D-sound. This option is only
        * available with #SOUNDSYSTEM_DSOUND.
        * @see TT_SetUserPosition 
        * @see TT_Enable3DSoundPositioning */
        CLIENT_SNDOUTPUT_AUTO3DPOSITION  = 0x00000100,
        /** @brief If set the client instance will try to eliminate
         * echo from speakers. To enable echo cancellation first make
         * the client run on sound duplex mode by calling
         * TT_InitSoundDuplexDevices() and afterwards call
         * TT_EnableEchoCancellation(). */
        CLIENT_SNDINPUT_AEC              = 0x00000200,
        /** @brief If set the client instance is running in sound
         * duplex mode where multiple audio output streams are mixed
         * into a single stream. This option must be enabled to
         * support echo cancellation (#CLIENT_SNDINPUT_AEC). Call
         * TT_InitSoundDuplexDevices() to enable duplex mode.*/
        CLIENT_SNDINOUTPUT_DUPLEX        = 0x00000400,
        /** @brief If set the client instance is currently transmitting
         * audio.  @see TT_EnableTransmission */
        CLIENT_TX_AUDIO                  = 0x00001000,
        /** @brief If set the client instance is currently transmitting video.
         * @see TT_EnableTransmission */
        CLIENT_TX_VIDEO                  = 0x00002000,
        /** @brief If set the client instance is currently muxing
         * audio streams into a single file. This is enabled by calling
         * TT_StartRecordingMuxedAudioFile(). */
        CLIENT_MUX_AUDIOFILE             = 0x00004000,
        /** @brief If set the client instance is currently try to
         * connect to a server, i.e. #TT_Connect has been called. */
        CLIENT_CONNECTING                = 0x00010000,
        /** @brief If set the client instance is connected to a server,
         * i.e. #WM_TEAMTALK_CON_SUCCESS event has been issued after
         * doing a #TT_Connect. Valid commands in this state:
         * #TT_DoLogin */
        CLIENT_CONNECTED                 = 0x00020000,
        /** @brief Helper for #CLIENT_CONNECTING and #CLIENT_CONNECTED
         * to see if #TT_Disconnect should be called. */
        CLIENT_CONNECTION                = CLIENT_CONNECTING | CLIENT_CONNECTED,
        /** @brief If set the client instance is logged on to a
         * server, i.e. got #WM_TEAMTALK_CMD_MYSELF_LOGGEDIN event
         * after issueing #TT_DoLogin. */
        CLIENT_AUTHORIZED                = 0x00040000,
        /** @brief If set the client instance will try and connect to
         * other users using peer to peer connections. Audio will be
         * broadcast to users instead of forwarded through server
         * (thereby increasing the bandwith usage).  Note that if
         * #USERRIGHT_FORWARD_AUDIO is disabled and no peer to peer
         * connection could be established, i.e. event
         * #WM_TEAMTALK_CON_P2P was posted with failure, then data
         * cannot be transferred to a user. */
        CLIENT_P2P_AUDIO                 = 0x00100000,
        /** @brief If set the client instance will try and connect to
         * other users using peer to peer connections. Video will be
         * broadcast to users instead of forwarded through server
         * (thereby increasing the bandwith usage).  Note that if
         * #USERRIGHT_FORWARD_VIDEO is disabled and no peer to peer
         * connection could be established, i.e. event
         * #WM_TEAMTALK_CON_P2P was posted with failure, then data
         * cannot be transferred to a user. */
        CLIENT_P2P_VIDEO                 = 0x00200000,
        /** @brief Helper for #CLIENT_P2P_AUDIO and #CLIENT_P2P_VIDEO to see
         * if the client instance is currently attempting P2P connections. */
        CLIENT_P2P                       = CLIENT_P2P_AUDIO | CLIENT_P2P_VIDEO
    } ClientFlag;

    /** @brief A bitmask based on #ClientFlag describing the local client 
     * instance's current state.  */
    typedef UINT32 ClientFlags;
    
    /** @brief Get the DLL's version number. */
    TEAMTALKDLL_API const TTCHAR* TT_GetVersion();

#if defined(WIN32)
    /** 
     * @brief Create a new TeamTalk client instance where events are
     * posted to a HWND.
     *
     * This function must be invoked before any other of the TT_*
     * functions can be called. Call #TT_CloseTeamTalk to shutdown the
     * TeamTalk client and release its resources.
     *
     * @param hWnd The window handle which will receive the events defined
     * in #ClientEvent.
     * @return A pointer to a new client instance. NULL if a failure occured.
     * @see TT_CloseTeamTalk */
    TEAMTALKDLL_API TTInstance* TT_InitTeamTalk(IN HWND hWnd);

    /**
     * @brief Replace the HWND passed as parameter to #TT_InitTeamTalk
     * with this HWND.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param hWnd The new HWND which should receive event messages. */
    TEAMTALKDLL_API BOOL TT_SwapTeamTalkHWND(IN TTInstance* lpTTInstance,
                                             IN HWND hWnd);
#endif

    /** 
     * @brief Create a new TeamTalk client instance where events are 
     * 'polled' using #TT_GetMessage.
     *
     * This 'polled' method can be used by application which doesn't
     * have a HWND, e.g. console applications.
     *
     * This function must be invoked before any other of the TT_*
     * functions can be called. Call #TT_CloseTeamTalk to shutdown the
     * TeamTalk client and release its resources.
     *
     * @return A pointer to a new client instance. NULL if a failure occured.
     * @see TT_CloseTeamTalk */
    TEAMTALKDLL_API TTInstance* TT_InitTeamTalkPoll();

    /** 
     * @brief Close the TeamTalk client instance and release its
     * resources.
     *
     * It is adviced to call this before closing the main application
     * to ensure a proper shutdown.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @see TT_InitTeamTalk */
    TEAMTALKDLL_API BOOL TT_CloseTeamTalk(IN TTInstance* lpTTInstance);

    /**
     * @brief Poll for events in the client instance.
     * 
     * On Windows a each client instance can send its events to a 
     * HWND but events can also be polled using a timer using 
     * @a TT_GetMessage.
     *
     * @param lpTTInstance Pointer to client instance created by
     * @param pMsg Pointer to a TTMessage instance which will hold the 
     * events that has occured.
     * @param pnWaitMs The amount of time to wait for the event. If NULL or -1
     * the function will block forever or until the next event occurs.
     * @return Returns TRUE if an event has occured otherwise FALSE.
     * @see TT_InitTeamTalkPolled
     * @see ClientEvent */
    TEAMTALKDLL_API BOOL TT_GetMessage(IN TTInstance* lpTTInstance, 
                                       OUT TTMessage* pMsg,
                                       IN const INT32* pnWaitMs);

    /**
     * @brief Get a bitmask describing the client's current state.
     *
     * Checks whether the client is connecting, connected, authorized,
     * etc. The current state can be checked by and'ing the returned
     * bitmask which is based on #ClientFlag.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @return A bitmask describing the current state (based on 
     * #ClientFlag).
     * @see ClientFlag */
     TEAMTALKDLL_API ClientFlags TT_GetFlags(IN TTInstance* lpTTInstance);

     /**
      * @brief Set license information to disable trial mode.
      *
      * This function must be called before #TT_InitTeamTalk.
      *
      * @param szRegName The registration name provided by BearWare.dk.
      * @param nRegKey The registration key provided by BearWare.dk.
      * @return True if the provided registration is acceptable. */
     TEAMTALKDLL_API BOOL TT_SetLicenseInformation(IN const TTCHAR szRegName[TT_STRLEN],
                                                   IN INT64 nRegKey);
    /** @} */

    /** @addtogroup sounddevices
     * @{ */

    /**
     * @brief Get the default sound devices. 
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param lpnInputDeviceID The ID of the default input device.
     * @param lpnOutputDeviceID The ID of the default output device.
     * @see TT_InitSoundInputDevice
     * @see TT_InitSoundOutputDevice */
    TEAMTALKDLL_API BOOL TT_GetDefaultSoundDevices(IN TTInstance* lpTTInstance, 
                                                   OUT INT32* lpnInputDeviceID, 
                                                   OUT INT32* lpnOutputDeviceID);

    /**
     * @brief Get information about input devices for audio recording. 
     *
     * @param lpTTInstance Pointer to client instance created by
     * TT_InitTeamTalk.
     * @param lpSoundDevices Array of SoundDevice-structs where lpnHowMany holds
     * the size of the array. Pass NULL as @a lpSoundDevices to query the 
     * number of devices.
     * @param lpnHowMany This is both an input and an output parameter. If 
     * @a lpSoundDevices is NULL lpnHowMany will after the call hold the
     * number of devices, otherwise it should hold the size of the
     * @a lpSoundDevices array.
     * @see TT_GetDefaultSoundDevices
     * @see TT_InitSoundInputDevice
     * @see TT_InitSoundOutputDevice */
    TEAMTALKDLL_API BOOL TT_GetSoundInputDevices(IN TTInstance* lpTTInstance, 
                                                 IN OUT SoundDevice* lpSoundDevices,
                                                 IN OUT INT32* lpnHowMany);

    /** 
     * @brief Get the list of sound output devices for playback. 
     *
     * The nDeviceID of the #SoundDevice struct should be passed to
     * #TT_InitSoundOutputDevice.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpSoundDevices Array of SoundDevice-structs where @a lpnHowMany
     * holds the size of the array. Pass NULL as @a lpSoundDevices to query the 
     * number of devices.
     * @param lpnHowMany This is both an input and an output parameter. If 
     * @a lpSoundDevices is NULL @a lpnHowMany will after the call hold the
     * number of devices, otherwise it should hold the size of the
     * @a lpSoundDevices array.
     * @see TT_GetDefaultSoundDevices 
     * @see TT_InitSoundOutputDevice */
    TEAMTALKDLL_API BOOL TT_GetSoundOutputDevices(IN TTInstance* lpTTInstance, 
                                                  IN OUT SoundDevice* lpSoundDevices,
                                                  IN OUT INT32* lpnHowMany);

    /**
     * @brief Initialize the sound input devices (for recording audio).
     *
     * The @a nDeviceID of the #SoundDevice should be used as @a 
     * nInputDeviceID.
     *
     * Callling this function will set the flag #CLIENT_SNDINPUT_READY.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nInputDeviceID The @a nDeviceID of #SoundDevice extracted 
     * through #TT_GetSoundInputDevices.
     * @see SoundDevice
     * @see TT_GetDefaultSoundDevices
     * @see TT_GetSoundInputDevices
     * @see TT_CloseSoundInputDevice
     * @see TT_GetSoundInputLevel */
    TEAMTALKDLL_API BOOL TT_InitSoundInputDevice(IN TTInstance* lpTTInstance, 
                                                 IN INT32 nInputDeviceID);

    /** 
     * @brief Initialize the sound output devices (for sound playback).
     *
     * The @a nDeviceID of the #SoundDevice should be used as @a 
     * nOutputDeviceID.
     *
     * Callling this function will set the flag
     * #CLIENT_SNDOUTPUT_READY.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nOutputDeviceID The @a nDeviceID of #SoundDevice extracted
     * through #TT_GetSoundOutputDevices.
     * @see SoundDevice
     * @see TT_GetDefaultSoundDevices
     * @see TT_GetSoundOutputDevices
     * @see TT_CloseSoundOutputDevice */
    TEAMTALKDLL_API BOOL TT_InitSoundOutputDevice(IN TTInstance* lpTTInstance, 
                                                  IN INT32 nOutputDeviceID);

    /**
     * @brief Enable duplex mode where multiple audio streams are
     * mixed into a single stream using software.
     *
     * If TT_InitSoundDuplexDevices() is successful the following
     * flags will be set:
     *
     * - #CLIENT_SNDINOUTPUT_DUPLEX
     * - #CLIENT_SNDOUTPUT_READY
     * - #CLIENT_SNDINPUT_READY
     *
     * Sound duplex mode is required for echo cancellation since sound
     * input and output device must be synchronized. Also sound cards
     * which does not support multiple output stream should use
     * duplex mode.
     *
     * Call TT_CloseSoundDuplexDevices() to shut down duplex mode.
     *
     * Note that it is only the audio streams from users in the local
     * client instance's current channel which will be mixed. If the
     * local client instance calls TT_DoSubscribe() with
     * #SUBSCRIBE_INTERCEPT_AUDIO on a user in another channel then
     * the audio from this user will be started in a separate
     * stream. The reason for this is that the other user may use a
     * different audio codec.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nInputDeviceID The @a nDeviceID of #SoundDevice extracted 
     * through #TT_GetSoundInputDevices.
     * @param nOutputDeviceID The @a nDeviceID of #SoundDevice extracted
     * through #TT_GetSoundOutputDevices.
     * @see TT_InitSoundInputDevice()
     * @see TT_InitSoundOutputDevice()
     * @see TT_EnableEchoCancellation */
    TEAMTALKDLL_API BOOL TT_InitSoundDuplexDevices(IN TTInstance* lpTTInstance, 
                                                   IN INT32 nInputDeviceID,
                                                   IN INT32 nOutputDeviceID);

    /**
     * @brief Shutdown the input sound device.
     *
     * Callling this function will clear the flag
     * #CLIENT_SNDINPUT_READY.
     *
     * If the local client instance is running in duplex mode (flag
     * #CLIENT_SNDINOUTPUT_DUPLEX is set) then trying to close the
     * sound device will fail since duplex mode require that both
     * input and output sound devices are active at the same
     * time. Therefore in order to close sound devices running in
     * duplex mode call TT_CloseSoundDuplexDevices().
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @return If running in sound duplex mode (#CLIENT_SNDINOUTPUT_DUPLEX)
     * then ensure to disable duplex mode prior to closing the sound device.
     * @see TT_InitSoundInputDevice */
    TEAMTALKDLL_API BOOL TT_CloseSoundInputDevice(IN TTInstance* lpTTInstance);

    /**
     * @brief Shutdown the output sound device.
     *
     * Callling this function will clear set the flag
     * #CLIENT_SNDOUTPUT_READY.
     *
     * If the local client instance is running in duplex mode (flag
     * #CLIENT_SNDINOUTPUT_DUPLEX is set) then trying to close the
     * sound device will fail since duplex mode require that both
     * input and output sound devices are active at the same
     * time. Therefore in order to close sound devices running in
     * duplex mode call TT_CloseSoundDuplexDevices().
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @return If running in sound duplex mode (#CLIENT_SNDINOUTPUT_DUPLEX)
     * then ensure to disable duplex mode prior to closing the sound device.
     * @see TT_InitSoundOutputDevice */
    TEAMTALKDLL_API BOOL TT_CloseSoundOutputDevice(IN TTInstance* lpTTInstance);

    /**
     * @brief Shut down sound devices running in duplex mode.
     *
     * Calling this function only applies if sound devices has been
     * initialized with TT_InitSoundDuplexDevices().
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk. */
    TEAMTALKDLL_API BOOL TT_CloseSoundDuplexDevices(IN TTInstance* lpTTInstance);

    /**
     * @brief Perform a record and playback test of specified sound
     * devices.
     *
     * Call TT_StopSoundLoopbackTest() to stop the loopback test.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nInputDeviceID Should be the @a nDeviceID extracted through 
     * #TT_GetSoundInputDevices.
     * @param nOutputDeviceID Should be the @a nDeviceID extracted through 
     * #TT_GetSoundOutputDevices.
     * @param nSampleRate The sample rate the client's recorder should 
     * use.
     * @param nChannels Number of channels to use, i.e. 1 = mono, 2 = stereo
     * Note that echo cancellation, denoising and AGC is not supported in
     * stereo.
     * @see TT_InitSoundInputDevice()
     * @see TT_InitSoundOutputDevice()
     * @see TT_InitSoundDuplexDevices()
     * @see TT_StopSoundLoopbackTest */
    TEAMTALKDLL_API BOOL TT_StartSoundLoopbackTest(IN TTInstance* lpTTInstance, 
                                                   IN INT32 nInputDeviceID, 
                                                   IN INT32 nOutputDeviceID,
                                                   IN INT32 nSampleRate,
                                                   IN INT32 nChannels);

    /**
     * @brief Perform a record and playback test of specified sound
     * devices along with an audio configuration and ability to try
     * echo cancellation.
     *
     * This function is almost like TT_StartSoundLoopbackTest() except
     * that it allows the use of #AudioConfig to enable AGC, denoising
     * and AGC.
     *
     * Call TT_StopSoundLoopbackTest() to stop the loopback
     * test.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nInputDeviceID Should be the @a nDeviceID extracted through 
     * #TT_GetSoundInputDevices.
     * @param nOutputDeviceID Should be the @a nDeviceID extracted through 
     * #TT_GetSoundOutputDevices.
     * @param nSampleRate The sample rate the client's recorder should 
     * use.
     * @param nChannels Number of channels to use, i.e. 1 = mono, 2 = stereo.
     * Note that echo cancellation, denoising and AGC is not supported in
     * stereo.
     * @param lpAudioConfig The audio configuration to use, i.e. AGC 
     * and denoising properties.
     * @param bEchoCancel Whether to enable echo cancellation.
     * @see TT_InitSoundInputDevice()
     * @see TT_InitSoundOutputDevice()
     * @see TT_InitSoundDuplexDevices()
     * @see TT_StopSoundLoopbackTest() */
    TEAMTALKDLL_API BOOL TT_StartSoundLoopbackTestEx(IN TTInstance* lpTTInstance, 
                                                     IN INT32 nInputDeviceID, 
                                                     IN INT32 nOutputDeviceID,
                                                     IN INT32 nSampleRate,
                                                     IN INT32 nChannels,
                                                     IN const AudioConfig* lpAudioConfig,
                                                     IN BOOL bEchoCancel);
    /**
     * @brief Stop recorder and playback test.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @see TT_InitSoundInputDevice
     * @see TT_InitSoundOutputDevice
     * @see TT_StartSoundLoopbackTest */
    TEAMTALKDLL_API BOOL TT_StopSoundLoopbackTest(IN TTInstance* lpTTInstance);

    /**
     * @brief Get the volume level of the current recorded audio.
     *
     * The current level is updated at an interval specified in a channel's
     * #AudioCodec.
     *
     * Note that the volume level will not be available until the
     * client instance joins a channel, i.e. it knows what sample rate
     * to use.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @return Returns a value between #SOUND_VU_MIN and #SOUND_VU_MAX. */
    TEAMTALKDLL_API INT32 TT_GetSoundInputLevel(IN TTInstance* lpTTInstance);

    /** 
     * @brief Set voice gaining of recorded audio. 
     *
     * The gain level ranges from #SOUND_GAIN_MIN to #SOUND_GAIN_MAX
     * where #SOUND_GAIN_DEFAULT is no gain. So 100 is 1/10 of the
     * original volume and 8000 is 8 times the original volume.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nLevel A value from #SOUND_GAIN_MIN to #SOUND_GAIN_MAX.
     * @see TT_GetSoundInputGainLevel */
    TEAMTALKDLL_API BOOL TT_SetSoundInputGainLevel(IN TTInstance* lpTTInstance, 
                                                   IN INT32 nLevel);

    /**
     * @brief Get voice gain level of outgoing audio
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @return A value from #SOUND_GAIN_MIN to #SOUND_GAIN_MAX.
     * @see TT_SetSoundInputGainLevel */
    TEAMTALKDLL_API INT32 TT_GetSoundInputGainLevel(IN TTInstance* lpTTInstance);

    /**
     * @brief Set master volume. 
     *
     * If still not loud enough use #TT_SetUserGainLevel.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nVolume A value from #SOUND_VOLUME_MIN to  #SOUND_VOLUME_MAX.
     * @see TT_SetUserVolume */
    TEAMTALKDLL_API BOOL TT_SetSoundOutputVolume(IN TTInstance* lpTTInstance, 
                                                 IN INT32 nVolume);

    /**
     * @brief Get master volume.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @return Returns the master volume.
     * @see SOUND_VOLUME_MAX
     * @see SOUND_VOLUME_MIN
     * @see TT_GetUserVolume */
    TEAMTALKDLL_API INT32 TT_GetSoundOutputVolume(IN TTInstance* lpTTInstance);

    /**
     * @brief Set all users mute.
     *
     * To stop receiving audio from a user call #TT_DoUnsubscribe.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param bMuteAll Whether to mute or unmute all users.
     * @see CLIENT_SNDOUTPUT_MUTE */
    TEAMTALKDLL_API BOOL TT_SetSoundOutputMute(IN TTInstance* lpTTInstance, 
                                               IN BOOL bMuteAll);

    /**
     * @brief Enable denoising of recorded audio.
     *
     * This call will enable/disable the #CLIENT_SNDINPUT_DENOISING
     * flag. Denoising will not be active if the local client instance
     * is participating in a channel which uses a stereo #AudioCodec.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param bEnable TRUE to enable, otherwise FALSE.
     * @see TT_SetDenoiseLevel() */
    TEAMTALKDLL_API BOOL TT_EnableDenoising(IN TTInstance* lpTTInstance, 
                                            IN BOOL bEnable);

    /**
     * @brief Set the denoise level of recorded audio.
     *
     * Setting denoise level is only valid if TT_GetFlags() contains
     * #CLIENT_SNDINPUT_DENOISING.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nLevel Maximum attenuation of the noise in dB. Negative value!
     * Default value is -15.
     * @see TT_EnableDenoising() */
    TEAMTALKDLL_API BOOL TT_SetDenoiseLevel(IN TTInstance* lpTTInstance, 
                                            IN INT32 nLevel);

    /**
     * @brief Set the denoise level of recorded audio.
     *
     * Getting denoise level is only valid if TT_GetFlags() contains
     * #CLIENT_SNDINPUT_DENOISING.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Maximum attenuation of the noise in dB. Negative value!
     * @see TT_SetDenoiseLevel() */
    TEAMTALKDLL_API INT32 TT_GetDenoiseLevel(IN TTInstance* lpTTInstance);

    /**
     * @brief Enable/disable acoustic echo cancellation (AEC).
     *
     * In order to enable echo cancellation mode the local client
     * instance must first be set in sound duplex mode by calling
     * TT_InitSoundDuplexDevices(). This is because the echo canceller
     * must first mixed all audio streams into a single stream and
     * have then run in synch with the input stream. After calling
     * TT_InitSoundDuplexDevices() the flag #CLIENT_SNDINOUTPUT_DUPLEX
     * will be set.
     *
     * For echo cancellation to work the sound input and output device
     * must be the same sound card since the input and output stream
     * must be completely synchronized. Also it is recommended to also
     * enable denoising and AGC for better echo cancellation by calling
     * TT_EnableDenoising() and TT_EnableAGC() respectively. Using an 
     * #AudioConfig on a #Channel can also be use to automatically
     * enable denoising and AGC.
     *
     * Echo cancellation will not be active if the local client
     * instance is participating in a channel which uses a stereo
     * #AudioCodec.
     *
     * After calling TT_EnableEchoCancellation() the flag
     * #CLIENT_SNDINPUT_AEC will be set.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param bEnable Whether to enable or disable echo callation.
     * @see TT_EnableDenoising()
     * @see TT_EnableAGC()
     * @see TT_InitSoundInputDevice()
     * @see TT_InitSoundOutputDevice() */
    TEAMTALKDLL_API BOOL TT_EnableEchoCancellation(IN TTInstance* lpTTInstance,
                                                   IN BOOL bEnable);

    /** 
     * @brief Enable Automatic Gain Control
     *
     * Each #Channel can also be configured to automatically enable
     * the local client instance to enable AGC by specifying the @a
     * bEnableAGC member of #AudioConfig.
     *
     * AGC will not be active if the local client instance is
     * participating in a channel which uses a stereo #AudioCodec.
     *
     * Note that Windows Mobile/CE doesn't support AGC.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param bEnable Whether to enable AGC.
     * @see CLIENT_SNDINPUT_AGC
     * @see TT_SetAGCSettings */
    TEAMTALKDLL_API BOOL TT_EnableAGC(IN TTInstance* lpTTInstance, 
                                      IN BOOL bEnable);

    /** 
     * @brief Set Automatic Gain Control (AGC) settings.
     *
     * Since microphone volumes may vary AGC can be used to adjust a signal
     * to a reference volume. That way users will speak at the same volume
     * level.
     *
     * Calling TT_SetAGCSettings() is only valid if TT_GetFlags() contains
     * #CLIENT_SNDINPUT_AGC.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nGainLevel A value from 0 to 32768. Default is 8000.
     * @param nMaxIncrement Used so volume
     * should not be amplified too quickly (maximal gain increase in dB/second).
     * Default is 12.
     * @param nMaxDecrement Negative value! Used so volume
     * should not be attenuated too quickly (maximal gain decrease in
     * dB/second). Default is -40.
     * @param nMaxGain Ensure volume doesn't become too loud (maximal gain
     * in dB). Default is 30.
     * @see TT_EnableAGC */
    TEAMTALKDLL_API BOOL TT_SetAGCSettings(IN TTInstance* lpTTInstance, 
                                           IN INT32 nGainLevel, 
                                           IN INT32 nMaxIncrement,
                                           IN INT32 nMaxDecrement,
                                           IN INT32 nMaxGain);

    /** 
     * @brief Get Automatic Gain Control settings.
     *
     * Calling TT_GetAGCSettings() is only valid if TT_GetFlags() contains
     * #CLIENT_SNDINPUT_AGC.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpnGainLevel A value from 0 to 32768.
     * @param lpnMaxIncrement Used so volume
     * should not be amplified too quickly (maximal gain increase in dB/second).
     * @param lpnMaxDecrement Negative value! Used so volume
     * should not be attenuated too quickly (maximal gain decrease in 
     * dB/second).
     * @param lpnMaxGain Ensure volume doesn't become too loud (maximal gain
     * in dB).
     * @see TT_EnableAGC */
    TEAMTALKDLL_API BOOL TT_GetAGCSettings(IN TTInstance* lpTTInstance, 
                                           OUT INT32* lpnGainLevel, 
                                           OUT INT32* lpnMaxIncrement,
                                           OUT INT32* lpnMaxDecrement,
                                           OUT INT32* lpnMaxGain);

    /** 
     * @brief Enable automatically position users using 3D-sound.
     *
     * Note that 3d-sound does not work if sound is running in duplex
     * mode (#CLIENT_SNDINOUTPUT_DUPLEX).
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param bEnable TRUE to enable, otherwise FALSE.
     * @see TT_SetUserPosition */
    TEAMTALKDLL_API BOOL TT_Enable3DSoundPositioning(IN TTInstance* lpTTInstance, 
                                                     IN BOOL bEnable);

    /** 
     * @brief Automatically position users using 3D-sound.
     *
     * Note that 3d-sound does not work if sound is running in duplex
     * mode (#CLIENT_SNDINOUTPUT_DUPLEX).
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @see TT_SetUserPosition */
    TEAMTALKDLL_API BOOL TT_AutoPositionUsers(IN TTInstance* lpTTInstance);
    /** @} */

    /** @addtogroup transmission
     * @{ */

    /**
     * @brief Enable voice activation.
     *
     * The client instance will start transmitting audio if the recorded audio
     * level is above or equal to the voice activation level set by
     * #TT_SetVoiceActivationLevel. Once the voice activation level is reached
     * the event #WM_TEAMTALK_VOICE_ACTIVATION is posted.
     *
     * The current volume level can be queried calling
     * #TT_GetSoundInputLevel.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param bEnable TRUE to enable, otherwise FALSE.
     * @see CLIENT_SNDINPUT_VOICEACTIVATION
     * @see TT_SetVoiceActivationStopDelay */
    TEAMTALKDLL_API BOOL TT_EnableVoiceActivation(IN TTInstance* lpTTInstance, 
                                                  IN BOOL bEnable);

    /** 
     * @brief Set voice activation level.
     *
     * The current volume level can be queried calling
     * #TT_GetSoundInputLevel.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nLevel Must be between #SOUND_VU_MIN and #SOUND_VU_MAX
     * @see TT_EnableVoiceActivation
     * @see TT_GetVoiceActivationLevel
     * @see TT_SetVoiceActivationStopDelay */
    TEAMTALKDLL_API BOOL TT_SetVoiceActivationLevel(IN TTInstance* lpTTInstance, 
                                                    IN INT32 nLevel);

    /** 
     * @brief Get voice activation level.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Returns A value between #SOUND_VU_MIN and #SOUND_VU_MAX
     * @see TT_EnableVoiceActivation
     * @see TT_SetVoiceActivationLevel */
    TEAMTALKDLL_API INT32 TT_GetVoiceActivationLevel(IN TTInstance* lpTTInstance);

    /**
     * @brief Set the delay of when voice activation should be stopped.
     *
     * When TT_GetSoundInputLevel() becomes higher than the specified
     * voice activation level the client instance will start
     * transmitting until TT_GetSoundInputLevel() becomes lower than
     * the voice activation level, plus a delay. This delay is by
     * default set to 1500 msec but this value can be changed by
     * calling TT_SetVoiceActivationStopDelay().
     *
     * @see TT_EnableVoiceActivation
     * @see TT_SetVoiceActivationLevel */
    TEAMTALKDLL_API BOOL TT_SetVoiceActivationStopDelay(IN TTInstance* lpTTInstance,
                                                        IN INT32 nDelayMSec);

    /**
     * @brief Get the delay of when voice active state should be disabled.
     *
     * @return The number of miliseconds before voice activated state
     * should be turned back to inactive.
     *
     * @see TT_SetVoiceActivationStopDelay
     * @see TT_EnableVoiceActivation
     * @see TT_SetVoiceActivationLevel */
    TEAMTALKDLL_API INT32 TT_GetVoiceActivationStopDelay(IN TTInstance* lpTTInstance);

    /**
     * @brief Store audio conversations to a single file.
     *
     * Unlike TT_SetUserAudioFolder(), which stores users' audio
     * streams in separate files, TT_StartRecordingMuxedAudioFile()
     * muxes the audio streams into a single file.
     *
     * The audio streams, which should be muxed together, are
     * required to use the same audio codec. In most cases this is
     * the audio codec of the channel where the user is currently
     * participating (i.e. @c codec member of #Channel).
     *
     * If the user changes to a channel which uses a different audio
     * codec then the recording will continue but simply be silent
     * until the user again joins a channel with the same audio codec
     * as was used for initializing muxed audio recording.
     *
     * Calling TT_StartRecordingMuxedAudioFile() will enable the
     * #CLIENT_MUX_AUDIOFILE flag from TT_GetFlags().
     *
     * Call TT_StopRecordingMuxedAudioFile() to stop recording. Note
     * that only one muxed audio recording can be active at the same
     * time.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpAudioCodec The audio codec which should be used as
     * reference for muxing users' audio streams. In most situations
     * this is the #AudioCodec of the current channel, i.e.
     * TT_GetMyChannelID().
     * @param szAudioFileName The file to store audio to, e.g. 
     * C:\\MyFiles\\Conf.mp3.
     * @param aff The audio format which should be used in the recorded
     * file. The muxer will convert to this format.
     *
     * @see TT_SetUserAudioFolder()
     * @see TT_StopRecordingMuxedAudioFile() */
    TEAMTALKDLL_API BOOL TT_StartRecordingMuxedAudioFile(IN TTInstance* lpTTInstance,
                                                         IN const AudioCodec* lpAudioCodec,
                                                         IN const TTCHAR* szAudioFileName,
                                                         IN AudioFileFormat aff);

    /**
     * @brief Stop an active muxed audio recording.
     *
     * A muxed audio recording started with
     * TT_StartRecordingMuxedAudioFile() can be stopped using this
     * function.
     *
     * Calling TT_StopRecordingMuxedAudioFile() will clear the
     * #CLIENT_MUX_AUDIOFILE flag from TT_GetFlags().
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     *
     * @see TT_StartRecordingMuxedAudioFile() */
    TEAMTALKDLL_API BOOL TT_StopRecordingMuxedAudioFile(IN TTInstance* lpTTInstance);
    /** @} */

    /** @addtogroup videocapture
     * @{ */

    /**
     * @brief Get the list of devices available for video capture.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param lpVideoDevices Array of VideoCaptureDevice-stucts where
     * @a lpnHowMany hold the size of the array. Pass NULL to query
     * the number of devices.
     * @param lpnHowMany This is both an input and output
     * parameter. If @a lpVideoDevices is NULL @a lpnHowMany will after
     * the call hold the number of devices, otherwise it should hold
     * the size of the @a lpVideoDevices array.
     * @see TT_InitVideoCaptureDevice */
    TEAMTALKDLL_API BOOL TT_GetVideoCaptureDevices(IN TTInstance* lpTTInstance,
                                                   IN OUT VideoCaptureDevice* lpVideoDevices,
                                                   IN OUT INT32* lpnHowMany);

    /**
     * @brief Initialize a video capture device.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param szDeviceID The device idenfier @a szDeviceID of #VideoCaptureDevice.
     * @param lpCaptureFormat The capture format to use,
     * i.e. frame-rate, resolution and picture format.
     * @param lpVideoCodec Video codec to use for transmission. Use NULL or set 
     * @a nCodec of #VideoCodec to #NO_CODEC if capture device should only be 
     * used for testing locally. 
     * @see TT_GetVideoCaptureDevices
     * @see TT_CloseVideoCaptureDevice
     * @see TT_EnableTransmission */
    TEAMTALKDLL_API BOOL TT_InitVideoCaptureDevice(IN TTInstance* lpTTInstance,
                                                   IN const TTCHAR* szDeviceID,
                                                   IN const CaptureFormat* lpCaptureFormat,
                                                   IN const VideoCodec* lpVideoCodec);
    /**
     * @brief Close a video capture device.
     *
     * @see TT_InitVideoCaptureDevice */
    TEAMTALKDLL_API BOOL TT_CloseVideoCaptureDevice(IN TTInstance* lpTTInstance);

#ifdef WIN32
    /** 
     * @brief Paint user's video frame using a Windows' DC (device
     * context).
     *
     * An application can either paint using #TT_GetUserVideoFrame
     * which provides a raw RGB32 array of the image or the
     * application can ask the client instance to paint the image
     * using this function.
     *
     * Note that the image will be stretched if it's not the same size
     * as what the user is using as resolution.
     *
     * Typically this paint operation will be called in the WM_PAINT
     * message. Here is how the client instance paints internally:
     *
       @verbatim
       StretchDIBits(hDC, nPosX, nPosY, nWidth, nHeight, 0, 0, 
                     bmh.biWidth, bmh.biHeight*-1, frame_buf, &bmi,
                     DIB_RGB_COLORS, SRCCOPY);
       @endverbatim 
     * 
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param nUserID The user's ID. 0 for local user.
     * @param hDC The handle to the Windows device context.
     * @param nPosX Coordinate of top left corner.
     * @param nPosY Coordinate or top right corner.
     * @param nWidth The width of the image.
     * @param nHeight The height of the image.
     * @see TT_GetUserVideoFrame */
    TEAMTALKDLL_API BOOL TT_PaintVideoFrame(IN TTInstance* lpTTInstance,
                                            IN INT32 nUserID,
                                            IN HDC hDC,
                                            IN INT32 nPosX,
                                            IN INT32 nPosY,
                                            IN INT32 nWidth,
                                            IN INT32 nHeight);
#endif
    
    /**
     * @brief Get or query the raw RGB32 bitmap data of a user's video frame.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param nUserID The user's ID. 0 for local user.
     * @param lpPicBuffer A pointer to a preallocated buffer of the size 
     * specified by @a nPicBufSize. Pass NULL to query the user's video format
     * which will put in @a lpCaptureFormat.
     * @param nPicBufSize The size in bytes of @a lpPicBuffer. The size must be
     * width * height * 4. Use the @a lpCaptureFormat parameter to get the width
     * and height. Ignored if @a lpPicBuffer is NULL.
     * @param lpCaptureFormat If not NULL the user's video frame format will
     * be filled in this parameter. Use this information to calculate the size
     * required of the @a lpPicBuffer parameter. */
    TEAMTALKDLL_API BOOL TT_GetUserVideoFrame(IN TTInstance* lpTTInstance,
                                              IN INT32 nUserID,
                                              IN OUT VOID* lpPicBuffer,
                                              IN INT32 nPicBufSize,
                                              OUT CaptureFormat* lpCaptureFormat);

    /** @brief Extract a user's video frame by making TeamTalk
     * allocate the image buffer.
     *
     * Unlike TT_GetUserVideoFrame() this function does not require
     * that the user preallocates a buffer which will contain the
     * image data. Instead the image buffer is allocated in
     * internally. REMEMBER, however, to call
     * TT_ReleaseUserVideoFrame() when the image has been processed so
     * the resources allocated by TeamTalk can be released.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param nUserID The user's ID. 0 for local user.
     * @param lpVideoFrame The struct which will contain the image data. Note 
     * that it's the @a frameBuffer member of #VideoFrame which will contain 
     * the image data allocated internally by TeamTalk.
     * @return Returns TRUE if a video frame was successfully put in the 
     * @a lpVideoFrame parameter.
     * @see TT_GetUserVideoFrame
     * @see TT_ReleaseUserVideoFrame */
    TEAMTALKDLL_API BOOL TT_AcquireUserVideoFrame(IN TTInstance* lpTTInstance,
                                                  IN INT32 nUserID,
                                                  OUT VideoFrame* lpVideoFrame);

    /** @brief Delete a user's video frame, acquired through
     * TT_AcquireUserVideoFrame(), so its allocated resources can be
     * released.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param nUserID The user's ID. 0 for local user.
     * @return Returns TRUE if a video frame was successfully put in the 
     * @a lpVideoFrame parameter.
     * @see TT_AcquireUserVideoFrame
     * @see TT_GetUserVideoFrame */
    TEAMTALKDLL_API BOOL TT_ReleaseUserVideoFrame(IN TTInstance* lpTTInstance,
                                                  IN INT32 nUserID);
    /** @} */

    /** @addtogroup transmission
     * @{ */

    /**
     * @brief Start/stop transmitting audio or video data.
     *
     * To check if transmission of either audio or video is enabled
     * call #TT_GetFlags and check bits #CLIENT_TX_AUDIO and
     * #CLIENT_TX_VIDEO.
     * 
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param uTxType A bitmask of the data types to enable/disable
     * transmission of.
     * @param bEnable Enable/disable transmission of bits in mask. */
    TEAMTALKDLL_API BOOL TT_EnableTransmission(IN TTInstance* lpTTInstance,
                                               IN TransmitTypes uTxType,
                                               IN BOOL bEnable);

    /** 
     * @brief Check if the client instance is currently transmitting.
     *
     * This call also checks if transmission is ongoing due to voice
     * activation.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param uTxType A mask specifying whether the client instance is 
     * currently transmitting the given #TransmitType.
     * @see TT_EnableTransmission
     * @see TT_EnableVoiceActivation */
    TEAMTALKDLL_API BOOL TT_IsTransmitting(IN TTInstance* lpTTInstance,
                                           IN TransmitTypes uTxType);

    /** 
     * @brief Stream a wave-file to a user in another channel. Only an
     * administrators can use this function.
     *
     * The event #WM_TEAMTALK_STREAM_AUDIOFILE_USER is called when
     * audio file is started and stopped.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user to transmit to. The user cannot be
     * in the same channel as the local client instance.
     * @param szAudioFilePath Path to .wav file. The format of the .wav file
     * must be 16-bit PCM uncompressed. Use <a href="http://audacity.sourceforge.net">
     * Audacity</a> to convert to proper file format.
     * @see TT_StopStreamingAudioFileToUser */
    TEAMTALKDLL_API BOOL TT_StartStreamingAudioFileToUser(IN TTInstance* lpTTInstance,
                                                          IN INT32 nUserID,
                                                          IN const TTCHAR* szAudioFilePath);

    /**
     * @brief Stop transmitting audio file.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param nUserID The ID of the user being transmitted to.
     * @see TT_StartStreamingAudioFileToUser */
    TEAMTALKDLL_API BOOL TT_StopStreamingAudioFileToUser(IN TTInstance* lpTTInstance,
                                                         IN INT32 nUserID);

    /** 
     * @brief Stream audio file to current channel.
     * 
     * Currently it is only possible to stream to the channel which the 
     * local client instance is participating in.
     *
     * When streaming to the current channel it basically replaces the
     * microphone input with a .wav file. Note that it is not possible
     * to stream a .wav file to a single user in the current channel.
     *
     * The event #WM_TEAMTALK_STREAM_AUDIOFILE_CHANNEL is posted when audio
     * file is being processed.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param nChannelID Pass #TT_GetMyChannelID. Transmitting to other 
     * channels is currently not supported. Instead use 
     * @param szAudioFilePath Path to .wav file. The format of the .wav file
     * must be 16-bit PCM uncompressed. Use <a href="http://audacity.sourceforge.net">
     * Audacity</a> to convert to proper file format.
     * @see TT_StartStreamingAudioFileToUser
     * @see TT_StopStreamingAudioFileToChannel */
    TEAMTALKDLL_API BOOL TT_StartStreamingAudioFileToChannel(IN TTInstance* lpTTInstance,
                                                             IN INT32 nChannelID,
                                                             IN const TTCHAR* szAudioFilePath);

    /**
     * @brief Stop streaming audio file to current channel.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. 
     * @param nChannelID Pass #TT_GetMyChannelID. */
    TEAMTALKDLL_API BOOL TT_StopStreamingAudioFileToChannel(IN TTInstance* lpTTInstance,
                                                            IN INT32 nChannelID);
    /** @} */

    /** @addtogroup connectivity
     * @{ */

    /**
     * @brief Connect to a server. 
     * 
     * This is a non-blocking call (but may block due to DNS lookup)
     * so the user application must wait for the event
     * #WM_TEAMTALK_CON_SUCCESS to be posted once the connection has
     * been established or #WM_TEAMTALK_CON_FAILED if connection could
     * not be established. If the connection could not be establish
     * ensure to call #TT_Disconnect to close open connections in the
     * client instance before trying again.
     *
     * Once connected call #TT_DoLogin to log on to the server.
     * 
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param szHostAddress The IP-address or hostname of the server.
     * @param nTcpPort The host port of the server (TCP).
     * @param nUdpPort The audio/video port of the server (UDP).
     * @param nLocalTcpPort The local TCP port which should be used. 
     * Setting it to 0 makes OS select a port number (recommended).
     * @param nLocalUdpPort The local UDP port which should be used. 
     * Setting it to 0 makes OS select a port number (recommended).
     * @see WM_TEAMTALK_CON_SUCCESS
     * @see WM_TEAMTALK_CON_FAILED
     * @see TT_DoLogin */
    TEAMTALKDLL_API BOOL TT_Connect(IN TTInstance* lpTTInstance,
                                    IN const TTCHAR* szHostAddress, 
                                    IN INT32 nTcpPort, 
                                    IN INT32 nUdpPort, 
                                    IN INT32 nLocalTcpPort, 
                                    IN INT32 nLocalUdpPort);

    /**
     * @brief Bind to specific IP-address priot to connecting to server.
     *
     * Same as TT_Connect() except that this also allows which IP-address
     * to bind to on the local interface.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param szHostAddress The IP-address or hostname of the server.
     * @param nTcpPort The host port of the server (TCP).
     * @param nUdpPort The audio/video port of the server (UDP).
     * @param szBindIPAddr The IP-address to bind to on the local interface
     * in dotted decimal format, e.g. 192.168.1.10.
     * @param nLocalTcpPort The local TCP port which should be used. 
     * Setting it to 0 makes OS select a port number (recommended).
     * @param nLocalUdpPort The local UDP port which should be used. 
     * Setting it to 0 makes OS select a port number (recommended).
     * @see TT_Connect */
    TEAMTALKDLL_API BOOL TT_ConnectEx(IN TTInstance* lpTTInstance,
                                      IN const TTCHAR* szHostAddress,
                                      IN INT32 nTcpPort,
                                      IN INT32 nUdpPort,
                                      IN const TTCHAR* szBindIPAddr,
                                      IN INT32 nLocalTcpPort,
                                      IN INT32 nLocalUdpPort);

    /**
     * @brief Disconnect from the server.
     * 
     * This will clear the flag #CLIENT_CONNECTED and #CLIENT_CONNECTING.
     *
     * Use #TT_CloseTeamTalk to release all resources allocated by the
     * client instance.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk. */
    TEAMTALKDLL_API BOOL TT_Disconnect(IN TTInstance* lpTTInstance);

    /** 
     * @brief Set how often the client should ping the server on its TCP
     * and UDP connection.
     *
     * Ensure that both the TCP or the UDP ping interval is less than
     * the server's user-timeout specified by @a nUserTimeout in
     * #ServerProperties. UDP keep-alive packets also updates the
     * ping-time to the server in @a nUdpPingTimeMs of
     * #ClientStatistics.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nTcpPingIntervalSec Seconds between issuing the ping-command.
     * Passing 0 will make the client instance use default settings.
     * @param nUdpPingIntervalSec Seconds between sending UDP keepalive
     * packets to the server (and p2p users). Passing 0 will make the 
     * client instance use default settings.
     * @see ServerProperties */
    TEAMTALKDLL_API BOOL TT_SetKeepAliveInterval(IN TTInstance* lpTTInstance,
                                                 IN INT32 nTcpPingIntervalSec,
                                                 IN INT32 nUdpPingIntervalSec);

    /** 
     * @brief Gets how often the client is sending keep-alive information to the 
     * server.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param lpnTcpPingIntervalSec The number of seconds between issuing the 
     * 'ping' command.
     * @param lpnUdpPingIntervalSec The number of seconds between sending UDP
     * keepalive packets.
     * @see TT_SetKeepAliveInterval */
    TEAMTALKDLL_API BOOL TT_GetKeepAliveInterval(IN TTInstance* lpTTInstance,
                                                 OUT INT32* lpnTcpPingIntervalSec,
                                                 OUT INT32* lpnUdpPingIntervalSec);

    /**
     * @brief Set server timeout for the client instance.
     * 
     * Set the number of seconds the client should allow the server
     * not to respond to keepalive requests
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nTimeoutSec Seconds before dropping connection if server hasn't 
     * replied. Passing 0 will use default (180 seconds). */
    TEAMTALKDLL_API BOOL TT_SetServerTimeout(IN TTInstance* lpTTInstance, 
                                             IN INT32 nTimeoutSec);

    /**
     * @brief Get the server timeout for the client instance.
     * 
     * Get the number of seconds the client should allow the server
     * not to respond to keepalive requests
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @return The number of seconds. On error -1. */
    TEAMTALKDLL_API INT32 TT_GetServerTimeout(IN TTInstance* lpTTInstance);

    /**
     * @brief Enable/disable peer to peer data transmission.
     * 
     * Peer to peer data transmission will reduce data transmission time,
     * since packets will not be forwarded through the server. This, however,
     * increases the bandwidth usage for clients since a separate data packet
     * will be sent to each user in a channel, instead of just sending a 
     * single packet to the server which would then broadcast the packet.
     *
     * Note that peer to peer data transmission is very unreliable and will 
     * only work with simple NAT-devices. Once a peer to peer connection 
     * succeeds or fails the event #WM_TEAMTALK_CON_P2P is posted.
     *
     * If the client instance is unable to connect with peer to peer
     * to a user it will try and forward the data packet through the
     * server if #USERRIGHT_FORWARD_AUDIO or #USERRIGHT_FORWARD_VIDEO are
     * enabled.
     *
     * @see TT_GetServerProperties
     * @see UserRights */
    TEAMTALKDLL_API BOOL TT_EnablePeerToPeer(IN TTInstance* lpTTInstance,
                                             IN TransmitTypes mask,
                                             IN BOOL bEnable);
    /**
     * @brief Retrieve client statistics of bandwidth usage and
     * response times.
     *
     * @see ClientStatistics */
     TEAMTALKDLL_API BOOL TT_GetStatistics(IN TTInstance* lpTTInstance,
                                           OUT ClientStatistics* lpStats);

    /** 
     * @brief Get the number of bytes in a packet with the specified
     * audio codec. 
     *
     * Note that this is only an estimate which doesn't include
     * headers of underlying protocols.
     *
     * @param lpCodec The codec settings to test for packet size
     * @see AudioCodec */
    TEAMTALKDLL_API INT32 TT_GetPacketSize(IN const AudioCodec* lpCodec);
    /** @} */

    /** @addtogroup commands
     * @{ */

    /**
     * @brief Logon to a server.
     * 
     * Once connected to a server call this function to logon. If
     * the login is successful #WM_TEAMTALK_CMD_MYSELF_LOGGEDIN is
     * posted, otherwise #WM_TEAMTALK_CMD_ERROR. Once logged on it's
     * not possible to talk to other users until the client instance
     * joins a channel. Call #TT_DoJoinChannel to join a channel.
     *
     * Possible errors:
     * - #CMDERR_INCORRECT_CHANNEL_PASSWORD
     * - #CMDERR_INVALID_ACCOUNT
     * - #CMDERR_MAX_SERVER_USERS_EXCEEDED
     * - #CMDERR_SERVER_BANNED
     * - #CMDERR_ALREADY_LOGGEDIN
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param szNickname The nickname to use.
     * @param szServerPassword The server's password. Leave empty if user has 
     * account on the server.
     * @param szUsername If #USERRIGHT_GUEST_LOGIN is disabled a username and 
     * password must be specified in order to login. Leave blank if guest 
     * logins are ok.
     * @param szPassword The password of the user account on the server. Leave 
     * blank if no account is needed on the server.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see USERRIGHT_GUEST_LOGIN
     * @see ServerProperties
     * @see TT_DoJoinChannel
     * @see WM_TEAMTALK_CMD_MYSELF_LOGGEDIN
     * @see WM_TEAMTALK_CMD_ERROR */
    TEAMTALKDLL_API INT32 TT_DoLogin(IN TTInstance* lpTTInstance,
                                     IN const TTCHAR* szNickname, 
                                     IN const TTCHAR* szServerPassword,
                                     IN const TTCHAR* szUsername,
                                     IN const TTCHAR* szPassword);

    /**
     * @brief Logout of the server.
     *
     * If successful the event #WM_TEAMTALK_CMD_MYSELF_LOGGEDOUT
     * will be posted.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see WM_TEAMTALK_CMD_MYSELF_LOGGEDOUT */
    TEAMTALKDLL_API INT32 TT_DoLogout(IN TTInstance* lpTTInstance);

    /**
     * @brief Create a new channel and join it. This command requires
     * that the flag #USERRIGHT_CHANNEL_CREATION is set in @a uUserRights
     * of #ServerProperties.
     *
     * This function can also be used to join an existing channel and
     * in this case the parameters @a szTopic and @a szOpPassword are
     * ignored.
     *
     * When #TT_DoJoinChannel is used to create channels it works
     * similar to IRC. If the client instance tres to join a channel
     * which does not exist it will be created as a new channel. If
     * the client instance is the last user to leave a channel the
     * channel will be removed on the server. Only administrators can
     * create static (persistent) channels, namely by using
     * #TT_DoMakeChannel.
     *
     * If the channel is created successfully the event
     * #WM_TEAMTALK_CMD_CHANNEL_NEW will be sent, followed by
     * #WM_TEAMTALK_CMD_MYSELF_JOINED.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_INCORRECT_CHANNEL_PASSWORD
     * - #CMDERR_MAX_CHANNEL_USERS_EXCEEDED
     * - #CMDERR_ALREADY_IN_CHANNEL
     * - #CMDERR_AUDIOCODEC_BITRATE_LIMIT_EXCEEDED
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpChannel The channel to join or create if it doesn't already
     * exist.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoLeaveChannel
     * @see TT_DoMakeChannel
     * @see WM_TEAMTALK_CMD_CHANNEL_NEW
     * @see WM_TEAMTALK_CMD_MYSELF_JOINED */
    TEAMTALKDLL_API INT32 TT_DoJoinChannel(IN TTInstance* lpTTInstance,
                                           IN const Channel* lpChannel);

    /**
     * @brief Join an existing channel.
     * 
     * This command basically calls #TT_DoJoinChannel but omits the
     * unnecessary parameters for only joining a channel and not
     * creating a new one.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_INCORRECT_CHANNEL_PASSWORD
     * - #CMDERR_MAX_CHANNEL_USERS_EXCEEDED
     * - #CMDERR_ALREADY_IN_CHANNEL
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The ID of the channel to join.
     * @param szPassword The password for the channel to join.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoLeaveChannel
     * @see TT_DoMakeChannel
     * @see WM_TEAMTALK_CMD_CHANNEL_NEW
     * @see WM_TEAMTALK_CMD_MYSELF_JOINED */
    TEAMTALKDLL_API INT32 TT_DoJoinChannelByID(IN TTInstance* lpTTInstance,
                                               IN INT32 nChannelID, 
                                               IN const TTCHAR* szPassword);

    /**
     * @brief Leave the current channel.
     *
     * Note that #TT_DoLeaveChannel() doesn't take any parameters
     * since a user can only participate in one channel at the time.
     * If command is successful the event #WM_TEAMTALK_CMD_MYSELF_LEFT
     * will be posted.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_NOT_IN_CHANNEL
     * - #CMDERR_CHANNEL_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoJoinChannel
     * @see WM_TEAMTALK_CMD_MYSELF_LEFT */
    TEAMTALKDLL_API INT32 TT_DoLeaveChannel(IN TTInstance* lpTTInstance);

    /**
     * @brief Change the client instance's nick name.
     *
     * The event #WM_TEAMTALK_CMD_USER_UPDATE will be posted if the
     * update was successful.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param szNewNick is the new nick name to use.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see WM_TEAMTALK_CMD_USER_UPDATE */
    TEAMTALKDLL_API INT32 TT_DoChangeNickname(IN TTInstance* lpTTInstance, 
                                              IN const TTCHAR* szNewNick);

    /**
     * @brief Change the client instance's currect status
     *
     * The event #WM_TEAMTALK_CMD_USER_UPDATE will be posted if the update
     * was successful.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nStatusMode The value for the status mode.
     * @param szStatusMessage The user's message associated with the status 
     * mode.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see WM_TEAMTALK_CMD_USER_UPDATE */
    TEAMTALKDLL_API INT32 TT_DoChangeStatus(IN TTInstance* lpTTInstance,
                                            IN INT32 nStatusMode, 
                                            IN const TTCHAR* szStatusMessage);

    /**
     * @brief Send a text message to either a user or a channel. 
     *
     * Can also be a broadcast message which is received by all users
     * on the server.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED 
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_USER_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpTextMessage A preallocated text-message struct.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see USERRIGHT_CLIENT_BROADCAST */
    TEAMTALKDLL_API INT32 TT_DoTextMessage(IN TTInstance* lpTTInstance,
                                           IN const TextMessage* lpTextMessage);

    /**
     * @brief Make another user operator of a channel. 
     * 
     * Requires that the client instance must already be operator of
     * the channel or is logged in as an administrator.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_USER_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The user who should become channel operator. 
     * @param nChannelID The channel where the user should become operator.
     * @param bMakeOperator Whether user should be op'ed or deop'ed.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoChannelOpEx */
    TEAMTALKDLL_API INT32 TT_DoChannelOp(IN TTInstance* lpTTInstance,
                                         IN INT32 nUserID,
                                         IN INT32 nChannelID,
                                         IN BOOL bMakeOperator);

    /**
     * @brief Make another user operator of a channel using the 
     * @a szOpPassword of #Channel.
     * 
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_USER_NOT_FOUND
     * - #CMDERR_INCORRECT_OP_PASSWORD
     *
     * @note
     * Requires server version 4.1.0.994 or later.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The user who should become channel operator. 
     * @param nChannelID The channel where the user should become operator.
     * @param szOpPassword The @a szOpPassword of #Channel.
     * @param bMakeOperator Whether user should be op'ed or deop'ed.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoChannelOp */
    TEAMTALKDLL_API INT32 TT_DoChannelOpEx(IN TTInstance* lpTTInstance,
                                           IN INT32 nUserID,
                                           IN INT32 nChannelID,
                                           IN const TTCHAR* szOpPassword,
                                           IN BOOL bMakeOperator);

    /**
     * @brief Kick user out of channel. 
     *
     * Only a channel operator or administration can kick users. To
     * ban a user call #TT_DoBanUser before #TT_DoKickUser.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_USER_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user to kick. 
     * @param nChannelID The channel where the user is participating. If
     * local instance is admin and @a nChannelID is 0, the user will be kicked
     * off the server. 
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoBanUser */
    TEAMTALKDLL_API INT32 TT_DoKickUser(IN TTInstance* lpTTInstance,
                                        IN INT32 nUserID,
                                        IN INT32 nChannelID);

    /**
     * @brief Send a file to the specified channel. 
     *
     * If user is logged on as an admin the file can be located in any 
     * channel. If the user is not an admin the file must be located in 
     * the same channel as the user is currently participating in.
     * The file being uploaded
     * must have a file size which is less than the disk quota of the channel, 
     * minus the sum of all the files in the channel. The disk quota of a
     * channel can be obtained in the nDiskQuota of the #Channel struct passed 
     * to #TT_GetChannel.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_MAX_DISKUSAGE_EXCEEDED
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_FILETRANSFER_NOT_FOUND
     * - #CMDERR_OPENFILE_FAILED
     * - #CMDERR_FILE_NOT_FOUND
     * - #CMDERR_FILE_ALREADY_EXISTS
     * - #CMDERR_FILESHARING_DISABLED
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The ID of the channel of where to put the file. Only 
     * admins can upload in channel other then their own.
     * @param szLocalFilePath The path of the file to upload, e.g. C:\\myfile.txt.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see Channel
     * @see TT_GetChannel */
    TEAMTALKDLL_API INT32 TT_DoSendFile(IN TTInstance* lpTTInstance,
                                        IN INT32 nChannelID,
                                        IN const TTCHAR* szLocalFilePath);

    /**
     * @brief Download a file from the specified channel. 
     *
     * If user is logged on as an admin the file can be located in any 
     * channel. If the user is not an admin the file must be located in 
     * the same channel as the user is currently participating in.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_FILETRANSFER_NOT_FOUND
     * - #CMDERR_OPENFILE_FAILED
     * - #CMDERR_FILE_NOT_FOUND
     * - #CMDERR_FILESHARING_DISABLED
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The ID of the channel of where to get the file. Only 
     * admins can download in channel other then their own.
     * @param nFileID The ID of the file which is passed by #WM_TEAMTALK_CMD_FILE_NEW.
     * @param szLocalFilePath The path of where to store the file, e.g. 
     * C:\\myfile.txt.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see WM_TEAMTALK_CMD_FILE_NEW
     * @see TT_GetChannelFiles */
    TEAMTALKDLL_API INT32 TT_DoRecvFile(IN TTInstance* lpTTInstance,
                                        IN INT32 nChannelID,
                                        IN INT32 nFileID, 
                                        IN const TTCHAR* szLocalFilePath);

    /**
     * @brief Delete a file from a channel. 
     *
     * A user is allowed to delete a file from a channel if either the
     * user is an admin, operator of the channel or owner of the
     * file. To be owner of the file a user must have a #UserAccount
     * on the server.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED 
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_FILE_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The ID of the channel where the file is located.
     * @param nFileID The ID of the file to delete. The ID of the file which 
     * is passed by #WM_TEAMTALK_CMD_FILE_NEW.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see WM_TEAMTALK_CMD_FILE_NEW
     * @see TT_GetChannelFiles */
    TEAMTALKDLL_API INT32 TT_DoDeleteFile(IN TTInstance* lpTTInstance,
                                          IN INT32 nChannelID,
                                          IN INT32 nFileID);

    /**
     * @brief Subscribe to user events and/or data.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED 
     * - #CMDERR_SUBSCRIPTIONS_DISABLED
     * - #CMDERR_USER_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user this should affect.
     * @param uSubscriptions Union of #Subscription to subscribe to.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see Subscription */
    TEAMTALKDLL_API INT32 TT_DoSubscribe(IN TTInstance* lpTTInstance,
                                         IN INT32 nUserID, 
                                         IN Subscriptions uSubscriptions);

    /**
     * @brief Unsubscribe to user events/data. This can be used to ignore messages
     * or voice data from a specific user.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED 
     * - #CMDERR_SUBSCRIPTIONS_DISABLED
     * - #CMDERR_USER_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user this should affect.
     * @param uSubscriptions Union of #Subscription to unsubscribe.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see Subscription */
    TEAMTALKDLL_API INT32 TT_DoUnsubscribe(IN TTInstance* lpTTInstance,
                                           IN INT32 nUserID, 
                                           IN Subscriptions uSubscriptions);

    /**
     * @brief Make a static (persistent) channel.
     * 
     * This command only works for admins since it creates a persistent
     * channel on the server which will be stored in the server's
     * config file.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_CHANNEL_ALREADY_EXISTS
     * - #CMDERR_CHANNEL_NOT_FOUND If channel's combined path is longer than
     *   #TT_STRLEN.
     * - #CMDERR_INCORRECT_CHANNEL_PASSWORD If the password is longer than
     *   #TT_STRLEN.
     * - #CMDERR_UNKNOWN_AUDIOCODEC If the server doesn't support the audio
     *   codec. Introduced in version 4.1.0.1264.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpChanInfo A Channel-structure containing information about
     * the channel being created. The Channel's member @a nChannelID is ignored.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoJoinChannel */
    TEAMTALKDLL_API INT32 TT_DoMakeChannel(IN TTInstance* lpTTInstance,
                                           IN const Channel* lpChanInfo);

    /**
     * @brief Update a channel's properties.
     *
     * Admin and operators of channel can update a channel's
     * properties. Note that a channel's #AudioCodec cannot be changed
     * if there's currently users in the channel.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED 
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_CHANNEL_HAS_USERS
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpChanInfo A Channel-structure containing information about
     * the channel being modified. The channel member's \a nParentID
     * and \a szName are ignored.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoMakeChannel */
    TEAMTALKDLL_API INT32 TT_DoUpdateChannel(IN TTInstance* lpTTInstance,
                                             IN const Channel* lpChanInfo);

    /**
     * @brief Remove a channel from a server. 
     *
     * This command only applies to admins.
     *
     * If there's any users in the channel they will be kicked and
     * subchannels will be deleted as well.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_CHANNEL_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.  
     * @param nChannelID The ID of the channel to remove.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoMakeChannel */
    TEAMTALKDLL_API INT32 TT_DoRemoveChannel(IN TTInstance* lpTTInstance,
                                             IN INT32 nChannelID);

    /**
     * @brief Issue command to move a user from one channel to
     * another.
     *
     * This command only applies to admins.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_USER_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID User to be moved.
     * @param nChannelID Channel where user should be put into.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoMoveUserByID */
    TEAMTALKDLL_API INT32 TT_DoMoveUser(IN TTInstance* lpTTInstance,
                                        IN INT32 nUserID, 
                                        IN INT32 nChannelID);

    /**
     * @brief Update server properties.
     *
     * This command only applies to admins.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_SERVER_HAS_USERS
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpServerInfo A structure holding the information to be set 
     * on the server.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_GetServerProperties */
    TEAMTALKDLL_API INT32 TT_DoUpdateServer(IN TTInstance* lpTTInstance,
                                            IN const ServerProperties* lpServerInfo);

    /**
     * @brief Issue command to list user accounts on the server.
     *
     * User accounts can be used to create different user types like
     * e.g. administrators.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nIndex Index of first user to display.
     * @param nCount The number of users to retrieve.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see UserAccount
     * @see UserType
     * @see TT_GetUserAccounts */
    TEAMTALKDLL_API INT32 TT_DoListUserAccounts(IN TTInstance* lpTTInstance,
                                                IN INT32 nIndex,
                                                IN INT32 nCount);

    /**
     * @brief Issue command to create a new user account on the
     * server.
     *
     * Check out section @ref useradmin to see how the server handles
     * users.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_INVALID_ACCOUNT
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpUserAccount The properties of the user account to create.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoListUserAccounts
     * @see TT_DoDeleteUserAccount
     * @see UserAccount
     * @see UserType */
    TEAMTALKDLL_API INT32 TT_DoNewUserAccount(IN TTInstance* lpTTInstance,
                                              IN const UserAccount* lpUserAccount);

    /**
     * @brief Issue command to delete a user account on the server.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_ACCOUNT_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param szUsername The username of the user account to delete.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoListUserAccounts
     * @see TT_DoNewUserAccount
     * @see UserAccount
     * @see UserType */
    TEAMTALKDLL_API INT32 TT_DoDeleteUserAccount(IN TTInstance* lpTTInstance,
                                                 IN const TTCHAR* szUsername);

    /**
     * @brief Issue a ban command on a user in a specific channel. 
     *
     * The ban applies to the user's IP-address. Call #TT_DoKickUser
     * to kick the user off the server. Only admins can ban users.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_CHANNEL_NOT_FOUND
     * - #CMDERR_USER_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user to ban.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoKickUser
     * @see TT_DoListBans */
    TEAMTALKDLL_API INT32 TT_DoBanUser(IN TTInstance* lpTTInstance,
                                       IN INT32 nUserID);

    /**
     * @brief Unban the user with the specified IP-address.
     *
     * Only admins can remove a ban.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_BAN_NOT_FOUND
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param szIpAddress The IP-address to unban.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoBanUser
     * @see TT_DoListBans */
    TEAMTALKDLL_API INT32 TT_DoUnBanUser(IN TTInstance* lpTTInstance,
                                         IN const TTCHAR* szIpAddress);

    /**
     * @brief Issue a command to list the banned users.
     *
     * Only admins can list bans. Once completed call the function
     * #TT_GetBannedUsers to get the list of users.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nIndex Index of first ban to display.
     * @param nCount The number of bans to display.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_InitTeamTalk
     * @see TT_GetBannedUsers */
    TEAMTALKDLL_API INT32 TT_DoListBans(IN TTInstance* lpTTInstance,
                                        IN INT32 nIndex,
                                        IN INT32 nCount);

    /**
     * @brief Save the server's current state to its settings file (typically 
     * the server's .xml file).
     *
     * Note that the server only saves channels with the flag
     * #CHANNEL_STATIC.
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error.
     * @see TT_DoUpdateServer */
    TEAMTALKDLL_API INT32 TT_DoSaveConfig(IN TTInstance* lpTTInstance);

    /**
     * @brief Get the server's current statistics obtained through
     * TT_GetServerStatistics().
     *
     * Possible errors:
     * - #CMDERR_NOT_LOGGEDIN
     * - #CMDERR_NOT_AUTHORIZED
     * - #CMDERR_UNKNOWN_COMMAND
     *
     * @note
     * Requires server version 4.1.0.1089 or later.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error. */
    TEAMTALKDLL_API INT32 TT_DoQueryServerStats(IN TTInstance* lpTTInstance);

    /**
     * @brief Quit from server. 
     *
     * Possible errors:
     * - none
     *
     * This will generate a #WM_TEAMTALK_CON_LOST since the server
     * will drop the client.
     *
     * @return Returns command ID which will be passed in 
     * #WM_TEAMTALK_CMD_PROCESSING event when the server is processing the 
     * command. -1 is returned in case of error. */
    TEAMTALKDLL_API INT32 TT_DoQuit(IN TTInstance* lpTTInstance);
    /** @} */

    /** @addtogroup server
     *
     * @brief Get the server's properties.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpProperties A struct to hold the server's properties. */
    TEAMTALKDLL_API BOOL TT_GetServerProperties(IN TTInstance* lpTTInstance,
                                                OUT ServerProperties* lpProperties);

    /**
     * @brief Get the server's statistics, i.e. bandwidth usage etc.

     * @see TT_DoQueryServerStatistics
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpStatistics A struct to hold the server's statistics. */
    TEAMTALKDLL_API BOOL TT_GetServerStatistics(IN TTInstance* lpTTInstance,
                                                OUT ServerStatistics* lpStatistics);

    /**
     * @brief Get the IDs of all the users on the server.
     *
     * Extracts the user IDs of all the users on the server. If only users in
     * a specific channel is needed call TT_GetChannelUsers()
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpUserIDs A preallocated array which has room for @a lpnHowMany 
     * user ID elements. Pass NULL to query the number of users in channel.
     * @param lpnHowMany The number of elements in the array @a lpUserIDs. If
     * @a lpUserIDs is NULL @a lpnHowMany will receive the number of users in
     * the channel.
     * @see TT_GetChannelUsers
     * @see TT_GetUser 
     * @see TT_GetServerChannels*/
    TEAMTALKDLL_API BOOL TT_GetServerUsers(IN TTInstance* lpTTInstance,
                                           IN OUT INT32* lpUserIDs,
                                           IN OUT INT32* lpnHowMany);
    /** @} */

    /** @addtogroup channels
     * @{ */

    /**
     * @brief Get the root channel's ID
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Returns the ID of the root channel. If 0 is returned no root 
     * channel exists.
     * @see TT_GetMyChannelID
     * @see TT_GetChannelPath */
    TEAMTALKDLL_API INT32 TT_GetRootChannelID(IN TTInstance* lpTTInstance);

    /**
     * @brief Get the channel which the local client instance is
     * currently participating in.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Returns the ID of the current channel. If 0 is returned the 
     * user is not participating in a channel. */
    TEAMTALKDLL_API INT32 TT_GetMyChannelID(IN TTInstance* lpTTInstance);

    /**
     * @brief Get the channel with a specific ID.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The ID of the channel to get information about.
     * @param lpChannel A preallocated struct which will receive the 
     * channel's properties.
     * @return FALSE if unable to retrieve channel otherwise TRUE. */
    TEAMTALKDLL_API BOOL TT_GetChannel(IN TTInstance* lpTTInstance,
                                       IN INT32 nChannelID, 
                                       OUT Channel* lpChannel );

    /**
     * @brief Get the channel's path. Channels are separated by '/'.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The channel's ID.
     * @param szChannelPath Will receive the channel's path.
     * @return Returns TRUE if channel exists. */
    TEAMTALKDLL_API BOOL TT_GetChannelPath(IN TTInstance* lpTTInstance,
                                           IN INT32 nChannelID, 
                                           OUT TTCHAR szChannelPath[TT_STRLEN]);

    /**
     * @brief Get the channel ID of the supplied path. Channels are
     * separated by '/'
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param szChannelPath Will receive the channel's path.
     * @return The channel's ID or 0 on error. */
    TEAMTALKDLL_API INT32 TT_GetChannelIDFromPath(IN TTInstance* lpTTInstance,
                                                  IN const TTCHAR* szChannelPath);

    /**
     * @brief Get the IDs of all users in a channel.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The channel's ID.
     * @param lpUserIDs A preallocated array which has room for @a lpnHowMany 
     * user ID elements. Pass NULL to query the number of users in channel.
     * @param lpnHowMany The number of elements in the array @a lpUserIDs. If
     * @a lpUserIDs is NULL @a lpnHowMany will receive the number of users in
     * the channel.
     * @see User 
     * @see TT_GetChannel */
    TEAMTALKDLL_API BOOL TT_GetChannelUsers(IN TTInstance* lpTTInstance,
                                            IN INT32 nChannelID,
                                            IN OUT INT32* lpUserIDs,
                                            IN OUT INT32* lpnHowMany);

    /**
     * @brief Get the list of the files in a channel which can be
     * downloaded.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The ID of the channel to extract the files from.
     * @param lpFileInfos A preallocated struct which will receive 
     * file information. If @a lpFileInfo is NULL then @a lpnHowMany will
     * receive the number of files in the channel.
     * @param lpnHowMany Use for both querying and specifying the number of
     * files. If @a lpFileInfos is NULL then lpnHowMany will receive the number
     * of files in the channel. If @a lpFileInfos is not NULL then
     * @a lpnHowMany should specify the size of the @a lpFileInfos array.
     * @see TT_GetChannelFileInfo */
    TEAMTALKDLL_API BOOL TT_GetChannelFiles(IN TTInstance* lpTTInstance,
                                            IN INT32 nChannelID, 
                                            IN OUT FileInfo* lpFileInfos,
                                            IN OUT INT32* lpnHowMany);

    /**
     * @brief Get information about a file which can be downloaded.
     *
     * Typically this is called after receiving
     * #WM_TEAMTALK_CMD_FILE_NEW.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nChannelID The ID of the channel to extract the file from.
     * @param nFileID The ID of the file.
     * @param lpFileInfo A preallocated struct which will receive 
     * file information. */
    TEAMTALKDLL_API BOOL TT_GetChannelFileInfo(IN TTInstance* lpTTInstance,
                                               IN INT32 nChannelID, 
                                               IN INT32 nFileID, 
                                               OUT FileInfo* lpFileInfo); 
    
    /**
     * @brief Check whether user is operator of a channel
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID the ID of the user to check.
     * @param nChannelID the ID of the channel to check whether user
     * is operator of. */
    TEAMTALKDLL_API BOOL TT_IsChannelOperator(IN TTInstance* lpTTInstance,
                                              IN INT32 nUserID, 
                                              IN INT32 nChannelID);

    /** 
     * @brief Get the IDs of all the channels on the server.
     *
     * Use TT_GetChannel() to get more information about each of the
     * channels. 
     * @see TT_GetServerUsers() */
    TEAMTALKDLL_API BOOL TT_GetServerChannels(IN TTInstance* lpTTInstance,
                                              IN OUT INT32* lpChannelIDs,
                                              IN OUT INT32* lpnHowMany);
    /** @} */

    /** @addtogroup users
     * @{ */

    /**
     * @brief Get the local client instance's user ID. 
     *
     * This information can be retrieved after the
     * #WM_TEAMTALK_CMD_MYSELF_LOGGEDIN event.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return Returns the user ID assigned to the current user on the server.
     * -1 is returned if no ID has been assigned to the user. */
    TEAMTALKDLL_API INT32 TT_GetMyUserID(IN TTInstance* lpTTInstance);

    /**
     * @brief Get the local client instance's #UserAccount.
     *
     * This information can be retrieved after
     * #WM_TEAMTALK_CMD_MYSELF_LOGGEDIN event.
     *
     * @note
     * Requires server version 4.0.1.970 or later.
     * 
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpUserAccount The local client's user account registered on
     * the server. Note that the @a szPassword field of #UserAccount
     * will not be set.
     * @see TT_DoLogin */
    TEAMTALKDLL_API BOOL TT_GetMyUserAccount(IN TTInstance* lpTTInstance,
                                             OUT UserAccount* lpUserAccount);

    /**
     * @brief Get the client instance's user type. 
     *
     * This information can be retrieved after
     * #WM_TEAMTALK_CMD_MYSELF_LOGGEDIN event.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return A bitmask of the type of user based on #UserType.
     * @see TT_GetMyUserAccount
     * @see TT_DoLogin
     * @see UserType */
    TEAMTALKDLL_API UserTypes TT_GetMyUserType(IN TTInstance* lpTTInstance);

    /**
     * @brief If an account was used in #TT_DoLogin then this value will 
     * return the @a nUserData from the #UserAccount.
     *
     * This information can be retrieved after
     * #WM_TEAMTALK_CMD_MYSELF_LOGGEDIN event.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @return If set, @a nUserData from #UserAccount, otherwise 0.
     * @see TT_GetMyUserAccount */
    TEAMTALKDLL_API INT32 TT_GetMyUserData(IN TTInstance* lpTTInstance);

    /**
     * @brief Get the user with the specified ID.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user to extract.
     * @param lpUser A preallocated #User struct.
     * @see TT_GetUserByUsername */
    TEAMTALKDLL_API BOOL TT_GetUser(IN TTInstance* lpTTInstance,
                                    IN INT32 nUserID, OUT User* lpUser);
    
    /**
     * @brief Get statistics for data and packet reception from a user.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user to extract.
     * @param lpStats A preallocated #UserStatistics struct. */
    TEAMTALKDLL_API BOOL TT_GetUserStatistics(IN TTInstance* lpTTInstance,
                                              IN INT32 nUserID, 
                                              OUT UserStatistics* lpStats);
    /**
     * @brief Get the user with the specified username.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param szUsername The user's username (from #UserAccount).
     * @param lpUser A preallocated #User struct. */
    TEAMTALKDLL_API BOOL TT_GetUserByUsername(IN TTInstance* lpTTInstance,
                                              IN const TTCHAR* szUsername, 
                                              OUT User* lpUser);
    /**
     * @brief Get a text-message sent by a user.
     *
     * The client instance uses a cyclic buffer in order not to drain
     * resources and can hold a maximum of 65535 text-messages.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nMsgID The ID of the message to extract.
     * @param bRemoveMsg Clear the text-message to release resources.
     * @param lpTextMessage Will receive the content of the message extracted. 
     * Pass NULL if only called to remove the message. */
    TEAMTALKDLL_API BOOL TT_GetTextMessage(IN TTInstance* lpTTInstance,
                                           IN INT32 nMsgID, 
                                           IN BOOL bRemoveMsg,
                                           OUT TextMessage* lpTextMessage);
    /** @} */

    /** @addtogroup sounddevices
     * @{ */

    /**
     * @brief Set the volume of a user.
     *
     * Note that it's a virtual volume which is being set since the
     * master volume affects the user volume.
     * 
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user whose volume will be changed.
     * @param nVolume Must be between #SOUND_VOLUME_MIN and #SOUND_VOLUME_MAX.
     * @see TT_SetSoundOutputVolume */
    TEAMTALKDLL_API BOOL TT_SetUserVolume(IN TTInstance* lpTTInstance,
                                          IN INT32 nUserID, IN INT32 nVolume);

    /**
     * @brief Get the volume of a user. 
     *
     * Note that it's a virtual volume which is being set since the
     * master volume affects the user volume.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user.
     * @return A value between #SOUND_VOLUME_MIN and #SOUND_VOLUME_MAX */
    TEAMTALKDLL_API INT32 TT_GetUserVolume(IN TTInstance* lpTTInstance,
                                           IN INT32 nUserID);

    /**
     * @brief Use software to gain a user's volume.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user who should have sound gained.
     * @param nGainLevel The gain level for the user. A value from
     * #SOUND_GAIN_MIN to #SOUND_GAIN_MAX
     * @see TT_GetUserGainLevel
     * @see SOUND_GAIN_DEFAULT */
    TEAMTALKDLL_API BOOL TT_SetUserGainLevel(IN TTInstance* lpTTInstance,
                                             IN INT32 nUserID, 
                                             IN INT32 nGainLevel);

    /**
     * @brief Get the software gain level for a user.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the user whose gain level should be retrieved.
     * @see TT_SetUserGainLevel
     * @return The gain level for the user. A value from #SOUND_GAIN_MIN to 
     * #SOUND_GAIN_MAX */
    TEAMTALKDLL_API INT32 TT_GetUserGainLevel(IN TTInstance* lpTTInstance,
                                              IN INT32 nUserID);

    /**
     * @brief Mute a user.
     *
     * To stop receiving audio from a user call #TT_DoUnsubscribe with
     * #SUBSCRIBE_AUDIO.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The user ID of the user to mute (or unmute).
     * @param bMute TRUE will mute, FALSE will unmute.
     * @see TT_SetSoundOutputMute */
    TEAMTALKDLL_API BOOL TT_SetUserMute(IN TTInstance* lpTTInstance,
                                        IN INT32 nUserID, IN BOOL bMute);

    /**
     * @brief Set the delay of when a user should be considered to no
     * longer be talking.
     *
     * When a user starts talking the #WM_TEAMTALK_USER_TALKING is
     * triggered with its parameter set to active. A user will remain
     * in this active state until no packets are received from this
     * user, plus a delay (due to network interruptions). This delay
     * is by default set to 500 msec but can be changed by calling
     * TT_SetUserStoppedTalkingDelay().
     *
     * @see TT_GetUserStoppedTalkingDelay */
    TEAMTALKDLL_API BOOL TT_SetUserStoppedTalkingDelay(IN TTInstance* lpTTInstance,
                                                       IN INT32 nUserID, 
                                                       IN INT32 nDelayMSec);

    /**
     * @brief Get the delay of when a user should no longer be considered as talking.
     *
     * @return The delay in miliseconds. -1 on error.
     * @see TT_SetUserStoppedTalkingDelay */
    TEAMTALKDLL_API INT32 TT_GetUserStoppedTalkingDelay(IN TTInstance* lpTTInstance,
                                                        IN INT32 nUserID);

    /**
     * @brief Set the position of a user.
     *
     * This can only be done using DirectSound (#SOUNDSYSTEM_DSOUND)
     * and with sound duplex mode (#CLIENT_SNDINOUTPUT_DUPLEX)
     * disabled.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID ID of user.
     * @param x Distance in meters to user (left/right).
     * @param y Distance in meters to user (back/forward).
     * @param z Distance in meters to user (up/down). */
    TEAMTALKDLL_API BOOL TT_SetUserPosition(IN TTInstance* lpTTInstance,
                                            IN INT32 nUserID, 
                                            IN float x,
                                            IN float y, 
                                            IN float z);

    /**
     * @brief Get a user's position.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID ID of user.
     * @param x Distance in meters to user (left/right).
     * @param y Distance in meters to user (back/forward).
     * @param z Distance in meters to user (up/down). */
    TEAMTALKDLL_API BOOL TT_GetUserPosition(IN TTInstance* lpTTInstance,
                                            IN INT32 nUserID, 
                                            OUT float* x, 
                                            OUT float* y, 
                                            OUT float* z);

    /**
     * @brief Set whether a user should speak in the left, right or
     * both speakers. This function only works if #AudioCodec has been
     * set to use stereo.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID ID of user.
     * @param bLeftSpeaker TRUE if user should be played in left speaker.
     * @param bRightSpeaker TRUE if user should be played in right speaker.
     * @see TT_GetUserStereo */
    TEAMTALKDLL_API BOOL TT_SetUserStereo(IN TTInstance* lpTTInstance,
                                          IN INT32 nUserID, 
                                          IN BOOL bLeftSpeaker, 
                                          IN BOOL bRightSpeaker);

    /**
     * @brief Check what speaker a user is outputting to.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID ID of user.
     * @param lpbLeftSpeaker TRUE if playing in left speaker.
     * @param lpbRightSpeaker TRUE if playing in right speaker.
     * @see TT_SetUserStereo */
    TEAMTALKDLL_API BOOL TT_GetUserStereo(IN TTInstance* lpTTInstance,
                                          IN INT32 nUserID, 
                                          OUT BOOL* lpbLeftSpeaker, 
                                          OUT BOOL* lpbRightSpeaker);
    /**
     * @brief Store user's audio to disk.
     * 
     * Set the path of where to store audio from a user to disk. To
     * store in MP3 format instead of .wav format ensure that the LAME
     * MP3 encoder file lame_enc.dll is placed in the same directory
     * as the SDKs DLL files. To stop recording set @a szFolderPath
     * to an empty string.
     *
     * To store audio of users not in current channel of the client
     * instance check out the section @ref spying.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nUserID The ID of the #User which should store audio to
     * disk.
     * @param szFolderPath The path on disk to where files should be stored.
     * This value will be stored in @a szAudioFolder of #User.
     * @return FALSE if path is invalid, otherwise TRUE.
     * @see User
     * @see WM_TEAMTALK_USER_AUDIOFILE */
    TEAMTALKDLL_API BOOL TT_SetUserAudioFolder(IN TTInstance* lpTTInstance,
                                               IN INT32 nUserID,
                                               IN const TTCHAR* szFolderPath);
    /** @} */

    /** @ingroup channels
     * @brief Get information about an active file transfer.  
     *
     * An active file transfer is one which has been posted through the
     * event #WM_TEAMTALK_FILETRANSFER.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nTransferID The ID of the file transfer to investigate. Transfer
     * ID is passed by #WM_TEAMTALK_FILETRANSFER.
     * @param lpTransfer A preallocated struct which will receive the file 
     * transfer information.
     * @see TT_CancelFileTransfer */
    TEAMTALKDLL_API BOOL TT_GetFileTransferInfo(IN TTInstance* lpTTInstance,
                                                IN INT32 nTransferID, 
                                                OUT FileTransfer* lpTransfer);

    /** @ingroup channels
     * @brief Cancel an active file transfer. 
     *
     * An active file transfer is one which has been post through the
     * event #WM_TEAMTALK_FILETRANSFER.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nTransferID The ID of the file transfer to investigate. Transfer 
     * ID is passed by #WM_TEAMTALK_FILETRANSFER. */
    TEAMTALKDLL_API BOOL TT_CancelFileTransfer(IN TTInstance* lpTTInstance,
                                               IN INT32 nTransferID);


    /** @ingroup server
     * @brief Get the list of banned users.
     * 
     * After the command #TT_DoListBans has completed, this function
     * can be called to retrieve the list of banned users. The list of
     * banned users can only be retrieved once after which the
     * internal representation of the users is deleted (to save
     * memory).
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpBannedUsers A preallocated array to hold banned users or NULL
     * to query how many banned users are listed.
     * @param lpnHowMany This is both an input and an output
     * parameter. If @a lpBannedUsers is NULL lpnHowMany will receive
     * the number of banned users. If @a lpBannedUsers is not NULL @a
     * lpnHowMany must contain how many banned users which will fit in
     * the @a lpBannedUsers array. Upon returning @a lpnHowMany will
     * contain how many were actually written to the array.
     * @see TT_DoBanUser  */
    TEAMTALKDLL_API BOOL TT_GetBannedUsers(IN TTInstance* lpTTInstance,
                                           IN OUT BannedUser* lpBannedUsers, 
                                           IN OUT INT32* lpnHowMany);

    /** @ingroup server
     * @brief Get the list of user accounts.
     * 
     * After the command #TT_DoListUserAccounts has competed, this
     * function can be called to retrieve the list of user accounts on
     * the server. The list of user accounts can only be retrieved
     * once after which the internal representation of the users is
     * deleted (to save memory).
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param lpUserAccounts A preallocated array to hold user accounts or NULL
     * to query how many user accounts are available for retrieval.
     * @param lpnHowMany This is both an input and an output
     * parameter. If @a lpUserAccounts is NULL lpnHowMany will receive
     * the number of user accounts. If @a lpUserAccounts is not NULL
     * @a lpnHowMany must contain how many user accounts which will
     * fit in the @a lpUserAccounts array. Upon returning lpnHowMany
     * will contain how many were actually written to the array.
     * @see TT_DoListUserAccounts
     * @see UserAccount */
    TEAMTALKDLL_API BOOL TT_GetUserAccounts(IN TTInstance* lpTTInstance,
                                            IN OUT UserAccount* lpUserAccounts, 
                                            IN OUT INT32* lpnHowMany);

    /** @ingroup errorhandling
     * @brief Get textual discription of an error message.
     * 
     * Get a description of an error code posted by either
     * #WM_TEAMTALK_CMD_ERROR or #WM_TEAMTALK_INTERNAL_ERROR.
     *
     * @param nError The number of the error.
     * @param szErrorMsg A text description of the error.
     * @see WM_TEAMTALK_CMD_ERROR
     * @see WM_TEAMTALK_INTERNAL_ERROR */
    TEAMTALKDLL_API void TT_GetErrorMessage(IN INT32 nError, 
                                            OUT TTCHAR szErrorMsg[TT_STRLEN]);

#if defined(WIN32)
    
    /** @addtogroup hotkey
     * @{ */

    /**
     * @brief Register a global hotkey. 
     *
     * When the hotkey becomes active or inactive it will send
     * #WM_TEAMTALK_HOTKEY to the HWND passed to #TT_InitTeamTalk.
     *
     * A hotkey can e.g. be used as a push-to-talk key
     * combination. When the hotkey becomes active call
     * #TT_EnableTransmission.
     *
     * Note that having a hotkey enabled makes the Visual Studio
     * debugger really slow to respond, so when debugging it's best
     * not to have hotkeys enabled.
     *
     * @param lpTTInstance Pointer to client instance created by 
     * #TT_InitTeamTalk.
     * @param nHotKeyID The ID of the hotkey to register. It will be
     * passed as the WPARAM when the hotkey becomes either active or inactive.
     * @param lpnVKCodes An array of virtual key codes which constitute the
     * hotkey. This document outlines the virtual key codes:
     * http://msdn.microsoft.com/en-us/library/ms645540(VS.85).aspx
     * A hotkey consisting of Left Control+A would have the array consist of 
     * [162, 65].
     * @param nVKCodeCount The number of virtual key codes in the array
     * (in other words the size of the @a lpnVKCodes array).
     * @see TT_InitTeamTalk
     * @see TT_HotKey_Unregister
     * @see TT_HotKey_InstallTestHook */
    TEAMTALKDLL_API BOOL TT_HotKey_Register(IN TTInstance* lpTTInstance,
                                            IN INT32 nHotKeyID, 
                                            IN const INT32* lpnVKCodes,
                                            IN INT32 nVKCodeCount);

    /**
     * @brief Unregister a registered hotkey.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nHotKeyID is the ID of the hotkey to unregister.
     * @see TT_HotKey_Register */
    TEAMTALKDLL_API BOOL TT_HotKey_Unregister(IN TTInstance* lpTTInstance,
                                              IN INT32 nHotKeyID);

    /**
     * @brief Check whether hotkey is active.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nHotKeyID is the ID of the registered hotkey. 
     * @return 1 if active, 0 if inactive, -1 if hotkey-ID is invalid */
    TEAMTALKDLL_API INT32 TT_HotKey_IsActive(IN TTInstance* lpTTInstance,
                                             IN INT32 nHotKeyID);

    /**
     * @brief Install a test hook so the HWND will be messaged
     * whenever a key or mouse button is pressed.
     *
     * Capture the event #WM_TEAMTALK_HOTKEY_TEST.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param hWnd is the handle of the window which will be
     * notified.
     * @see TT_HotKey_RemoveTestHook
     * @see WM_TEAMTALK_HOTKEY_TEST */
    TEAMTALKDLL_API BOOL TT_HotKey_InstallTestHook(IN TTInstance* lpTTInstance,
                                                   IN HWND hWnd);

    /**
     * @brief Remove the test hook again so the @a hWnd in
     * #TT_HotKey_InstallTestHook will no longer be notified.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @see TT_HotKey_InstallTestHook */
    TEAMTALKDLL_API BOOL TT_HotKey_RemoveTestHook(IN TTInstance* lpTTInstance);

    /**
     * @brief Get a string description of the virtual-key code.
     *
     * @param lpTTInstance Pointer to client instance created by
     * #TT_InitTeamTalk.
     * @param nVKCode The virtual key code passed in #WM_TEAMTALK_HOTKEY_TEST.
     * @param szKeyName Will receive key description in local language.
     * @see TT_HotKey_Register */
    TEAMTALKDLL_API BOOL TT_HotKey_GetKeyString(IN TTInstance* lpTTInstance,
                                                IN INT32 nVKCode,
                                                OUT TTCHAR szKeyName[TT_STRLEN]);
    /** @} */
#endif

    /* List of structures used internally by TeamTalk. */
    typedef enum TTStructType
    {
        __AUDIOCODEC,
        __BANNEDUSER,
        __CAPTUREFORMAT,
        __CELTCODEC,
        __CHANNEL,
        __CLIENTSTATISTICS,
        __FILEINFO,
        __FILETRANSFER,
        __SERVERPROPERTIES,
        __SERVERSTATISTICS,
        __SOUNDDEVICE,
        __SPEEXCODEC,
        __TEXTMESSAGE,
        __THEORACODEC,
        __TTMESSAGE,
        __USER,
        __USERACCOUNT,
        __USERSTATISTICS,
        __VIDEOCAPTUREDEVICE,
        __VIDEOCODEC,
        __AUDIOCONFIG,
        __CELTVBRCODEC,
        __SPEEXVBRCODEC,
        __VIDEOFRAME
    } TTStructType;

    /* Get the 'sizeof' of a structure used by TeamTalk. Useful to ensuring 
     * binary compatibility when integrating with other programming 
     * languages. */
    TEAMTALKDLL_API INT32 TT_DBG_SIZEOF(IN TTStructType nType);
    /* Get last #ClientEvent WM_TEAMTALK_DUMMY_LAST. */
    TEAMTALKDLL_API ClientEvent TT_DBG_EVENT_LAST();


#if defined(WIN32) /* Exclude mixer and firewall functions from
                    * non-Windows platforms */

    /** @addtogroup mixer
     * @{ */

    /**
     * @brief The Windows mixer controls which can be queried by the
     * TT_Mixer_* functions.
     *
     * Wave-In devices which are not in the enum-structure can be
     * accessed by #TT_Mixer_GetWaveInControlCount which allows the user to
     * query selection based on an index.
     *
     * Note that Windows Vista has deprecated mixer controls.
     *
     * @see TT_Mixer_SetWaveOutMute
     * @see TT_Mixer_SetWaveOutVolume
     * @see TT_Mixer_SetWaveInSelected
     * @see TT_Mixer_SetWaveInVolume
     * @see TT_Mixer_GetWaveInControlName
     * @see TT_Mixer_SetWaveInControlSelected */
    typedef enum MixerControl
    {
        WAVEOUT_MASTER,
        WAVEOUT_WAVE,
        WAVEOUT_MICROPHONE,

        WAVEIN_MICROPHONE,
        WAVEIN_LINEIN,
        WAVEIN_WAVEOUT,
    } MixerControl;

    /**
     * @brief Get the number of Windows Mixers available.
     *
     * The index from 0 to #TT_Mixer_GetMixerCount()-1 should be passed to the
     * TT_Mixer_* functions.
     * @see TT_Mixer_GetMixerName */
    TEAMTALKDLL_API INT32 TT_Mixer_GetMixerCount();

    /**
     * @brief Get the name of a Windows Mixer based on its name.
     *
     * @param nMixerIndex The index of the mixer. Ranging from 0 to 
     * #TT_Mixer_GetMixerCount()-1.
     * @param szMixerName The output string receiving the name of the device. */
    TEAMTALKDLL_API BOOL TT_Mixer_GetMixerName(IN INT32 nMixerIndex,
                                               OUT TTCHAR szMixerName[TT_STRLEN]);

    /**
     * @brief Get the name of the mixer associated with a wave-in device.
     * 
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param szMixerName The output string receiving the name of the device. 
     * @see TT_GetSoundInputDevices */
    TEAMTALKDLL_API BOOL TT_Mixer_GetWaveInName(IN INT32 nWaveDeviceID,
                                                OUT TTCHAR szMixerName[TT_STRLEN]);

    /**
     * @brief Get the name of the mixer associated with a wave-out device.
     * 
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param szMixerName The output string receiving the name of the device. 
     * @see TT_GetSoundOutputDevices */
    TEAMTALKDLL_API BOOL TT_Mixer_GetWaveOutName(IN INT32 nWaveDeviceID,
                                                 OUT TTCHAR szMixerName[TT_STRLEN]);

    /**
     * @brief Mute or unmute a Windows Mixer Wave-Out device from the
     * 'enum' of devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControl A mixer control.
     * @param bMute True if device should be muted, False if it should be
     * unmuted.
     * @see TT_Mixer_GetWaveOutMute */
    TEAMTALKDLL_API BOOL TT_Mixer_SetWaveOutMute(IN INT32 nWaveDeviceID, 
                                                 IN MixerControl nControl, 
                                                 IN BOOL bMute);

    /**
     * @brief Get the mute state of a Windows Mixer Wave-Out device
     * from the 'enum' of devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControl A mixer control.
     * @return TRUE if mute, FALSE if unmuted, -1 on error.
     * @see TT_Mixer_SetWaveOutMute */
    TEAMTALKDLL_API INT32 TT_Mixer_GetWaveOutMute(IN INT32 nWaveDeviceID, 
                                                  IN MixerControl nControl);

    /**
     * @brief Set the volume of a Windows Mixer Wave-Out device from
     * the 'enum' of devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControl A mixer control.
     * @param nVolume A value ranging from 0 to 65535. */
    TEAMTALKDLL_API BOOL TT_Mixer_SetWaveOutVolume(IN INT32 nWaveDeviceID, 
                                                   IN MixerControl nControl, 
                                                   IN INT32 nVolume);

    /**
     * @brief Get the volume of a Windows Mixer Wave-Out device from
     * the 'enum' of devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControl A mixer control.
     * @return A value ranging from 0 to 65535, or -1 on error. */
    TEAMTALKDLL_API INT32 TT_Mixer_GetWaveOutVolume(IN INT32 nWaveDeviceID, 
                                                    IN MixerControl nControl);

    /**
     * @brief Set the selected state of a Windows Mixer Wave-In
     * device from the 'enum' of devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControl A mixer control. */
    TEAMTALKDLL_API BOOL TT_Mixer_SetWaveInSelected(IN INT32 nWaveDeviceID, 
                                                    IN MixerControl nControl);

    /**
     * @brief Get the selected state of a Windows Mixer Wave-In device
     * from the 'enum' of devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControl A mixer control.
     * @return TRUE if mute, FALSE if unmuted, -1 on error. */
    TEAMTALKDLL_API INT32 TT_Mixer_GetWaveInSelected(IN INT32 nWaveDeviceID, 
                                                     IN MixerControl nControl);

    /**
     * @brief Set the volume of a Windows Mixer Wave-In device from
     * the 'enum' of devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControl A mixer control.
     * @param nVolume A value ranging from 0 to 65535. */
    TEAMTALKDLL_API BOOL TT_Mixer_SetWaveInVolume(IN INT32 nWaveDeviceID, 
                                                  IN MixerControl nControl, 
                                                  IN INT32 nVolume);

    /**
     * @brief Get the volume of a Windows Mixer Wave-In device from
     * the 'enum' of devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControl A mixer control.
     * @return A value ranging from 0 to 65535, or -1 on error. */
    TEAMTALKDLL_API INT32 TT_Mixer_GetWaveInVolume(IN INT32 nWaveDeviceID, 
                                                   IN MixerControl nControl);

    /**
     * @brief Enable and disable microphone boost.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param bEnable TRUE to enable, FALSE to disable. */
    TEAMTALKDLL_API BOOL TT_Mixer_SetWaveInBoost(IN INT32 nWaveDeviceID, 
                                                 IN BOOL bEnable);
    /**
     * @brief See if microphone boost is enabled.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @return TRUE if boost is enabled, FALSE if disabled, -1 on error. */
    TEAMTALKDLL_API INT32 TT_Mixer_GetWaveInBoost(IN INT32 nWaveDeviceID);

    /**
     * @brief Mute/unmute microphone input.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param bEnable TRUE to enable, FALSE to disable. */
    TEAMTALKDLL_API BOOL TT_Mixer_SetWaveInMute(IN INT32 nWaveDeviceID, 
                                                IN BOOL bEnable);

    /**
     * @brief See if microphone is muted.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @return TRUE if mute is enabled, FALSE if disabled, -1 on error. */
    TEAMTALKDLL_API INT32 TT_Mixer_GetWaveInMute(IN INT32 nWaveDeviceID);

    /**
     * @brief Get the number of Windows Mixer Wave-In devices.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @return Number of controls, or -1 on error.
     * @see TT_Mixer_GetWaveInControlName
     * @see TT_Mixer_SetWaveInControlSelected
     * @see TT_Mixer_GetWaveInControlSelected */
    TEAMTALKDLL_API INT32 TT_Mixer_GetWaveInControlCount(IN INT32 nWaveDeviceID);

    /**
     * @brief Get the name of the Wave-In device with the specified
     * index.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControlIndex The index of the control. Randing from 0 to 
     * #TT_Mixer_GetWaveInControlCount()-1.
     * @param szDeviceName The output string of the name of the device.
     * @see TT_Mixer_GetWaveInControlCount */
    TEAMTALKDLL_API BOOL TT_Mixer_GetWaveInControlName(IN INT32 nWaveDeviceID, 
                                                       IN INT32 nControlIndex, 
                                                       OUT TTCHAR szDeviceName[TT_STRLEN]);

    /**
     * @brief Set the selected state of a Wave-In device in the
     * Windows Mixer.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControlIndex The index of the device. Randing from 0 to 
     * #TT_Mixer_GetWaveInControlCount()-1.
     * @see TT_Mixer_GetWaveInControlCount */
    TEAMTALKDLL_API BOOL TT_Mixer_SetWaveInControlSelected(IN INT32 nWaveDeviceID, 
                                                           IN INT32 nControlIndex);

    /**
     * @brief Get the selected state of a Wave-In device in the
     * Windows Mixer.
     *
     * @param nWaveDeviceID The @a nWaveDeviceID from the #SoundDevice
     * struct.
     * @param nControlIndex The index of the device. Randing from 0 to  
     * #TT_Mixer_GetWaveInControlCount()-1.
     * @return TRUE if selected, FALSE if unselected, -1 on error.
     * @see TT_Mixer_GetWaveInControlCount */
    TEAMTALKDLL_API BOOL TT_Mixer_GetWaveInControlSelected(IN INT32 nWaveDeviceID, 
                                                           IN INT32 nControlIndex);
    /** @} */

    /** @addtogroup firewall
     * @{ */

    /**
     * @brief Check if the Windows Firewall is currently enabled.
     *
     * This function does not invoke UAC on Windows Vista/7.
     * @see TT_Firewall_Enable */
    TEAMTALKDLL_API BOOL TT_Firewall_IsEnabled();
    
    /**
     * @brief Enable/disable the Windows Firewall.
     *
     * The Windows Firewall was introduced in Windows XP SP2.
     *
     * On Windows XP (SP2+) the user calling this function is assumed
     * to have administrator rights. On Windows Vista/7 UAC is invoked
     * to ask the user for administrator rights.
     * @see TT_Firewall_IsEnabled */
    TEAMTALKDLL_API BOOL TT_Firewall_Enable(IN BOOL bEnable);

    /**
     * @brief Check if an executable is already in the Windows
     * Firewall exception list.
     *
     * This function does not invoke UAC on Windows Vista/7.
     * @see TT_Firewall_AddAppException */
    TEAMTALKDLL_API BOOL TT_Firewall_AppExceptionExists(IN const TTCHAR* szExecutable);

    /**
     * @brief Add an application to the Windows Firewall exception
     * list.
     *
     * On Windows XP (SP2+) the user calling this function is assumed
     * to have administrator rights. On Windows Vista/7 UAC is invoked
     * to ask the user for administrator rights.
     * @see TT_Firewall_AppExceptionExists
     * @see TT_Firewall_RemoveAppException */
    TEAMTALKDLL_API BOOL TT_Firewall_AddAppException(IN const TTCHAR* szName, 
                                                     IN const TTCHAR* szExecutable);

    /**
     * @brief Remove an application from the Windows Firewall exception
     * list.
     *
     * On Windows XP (SP2+) the user calling this function is assumed
     * to have administrator rights. On Windows Vista/7 UAC is invoked
     * to ask the user for administrator rights.
     * @see TT_Firewall_AppExceptionExists
     * @see TT_Firewall_AddAppException */
    TEAMTALKDLL_API BOOL TT_Firewall_RemoveAppException(IN const TTCHAR* szExecutable);
    /** @} */

#endif /** WIN32 */

#ifdef __cplusplus
}
#endif

#endif /** TEAMTALKDLL_H */

#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for YSPlugin
#
#\**********************************************************/

set(PLUGIN_NAME "YouStun")
set(PLUGIN_PREFIX "YS_")
set(COMPANY_NAME "YouStun")

# ActiveX constants:
set(FBTYPELIB_NAME YouStunLib)
set(FBTYPELIB_DESC "YouStun 1.0 Type Library")
set(IFBControl_DESC "YouStun Control Interface")
set(FBControl_DESC "YouStun Control Class")
set(IFBComJavascriptObject_DESC "YouStun IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "YouStun ComJavascriptObject Class")
set(IFBComEventSource_DESC "YouStun IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 5f40f627-c6ed-5fdd-8cf3-34658d38c477)
set(IFBControl_GUID 8153835e-6d3d-5336-9a59-9ed2d3eaf023)
set(FBControl_GUID c0e31033-f0bf-545f-972f-a83094992ffd)
set(IFBComJavascriptObject_GUID 109c1130-ea9d-5460-af6d-c5f20375e32d)
set(FBComJavascriptObject_GUID 22c481f2-a5ab-51b3-9e7a-a913358ee216)
set(IFBComEventSource_GUID b53224ad-ed12-58f9-a026-d8380c3dc8fb)

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "YouStun.YouStun")
set(MOZILLA_PLUGINID "youstun.com/YouStun")

# strings
set(FBSTRING_CompanyName "YouStun")
set(FBSTRING_FileDescription "Plugin for advanced function of YouStun")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2011 YouStun")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}.dll")
set(FBSTRING_ProductName "YSPlugin")
set(FBSTRING_FileExtents "")
set(FBSTRING_PluginName "YSPlugin")
set(FBSTRING_MIMEType "application/x-youstun")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)

add_firebreath_library(log4cplus)
#include "swf_client.h"

#define TAG CLIENT_TAG("windows")

static UINT wf_rail_server_execute_result(RailClientContext* context,
	RAIL_EXEC_RESULT_ORDER* execResult)
{
	WLog_DBG(TAG, "RailServerExecuteResult: 0x%08X", execResult->rawResult);
	return CHANNEL_RC_OK;
}

static UINT wf_rail_server_system_param(RailClientContext* context,
	RAIL_SYSPARAM_ORDER* sysparam)
{
	return CHANNEL_RC_OK;
}


/**
* Function description
*
* @return 0 on success, otherwise a Win32 error code
*/
static UINT wf_rail_server_handshake(RailClientContext* context,
	RAIL_HANDSHAKE_ORDER* handshake)
{
	RAIL_EXEC_ORDER exec;
	RAIL_SYSPARAM_ORDER sysparam;
	RAIL_HANDSHAKE_ORDER clientHandshake;
	RAIL_CLIENT_STATUS_ORDER clientStatus;
	swfContext* wfc = (swfContext*)context->custom;
	rdpSettings* settings = wfc->context.settings;
	clientHandshake.buildNumber = 0x00001DB0;
	context->ClientHandshake(context, &clientHandshake);
	ZeroMemory(&clientStatus, sizeof(RAIL_CLIENT_STATUS_ORDER));
	clientStatus.flags = RAIL_CLIENTSTATUS_ALLOWLOCALMOVESIZE;
	context->ClientInformation(context, &clientStatus);

	if (settings->RemoteAppLanguageBarSupported)
	{
		RAIL_LANGBAR_INFO_ORDER langBarInfo;
		langBarInfo.languageBarStatus = 0x00000008; /* TF_SFT_HIDDEN */
		context->ClientLanguageBarInfo(context, &langBarInfo);
	}

	ZeroMemory(&sysparam, sizeof(RAIL_SYSPARAM_ORDER));
	sysparam.params = 0;
	sysparam.params |= SPI_MASK_SET_HIGH_CONTRAST;
	sysparam.highContrast.colorScheme.string = NULL;
	sysparam.highContrast.colorScheme.length = 0;
	sysparam.highContrast.flags = 0x7E;
	sysparam.params |= SPI_MASK_SET_MOUSE_BUTTON_SWAP;
	sysparam.mouseButtonSwap = FALSE;
	sysparam.params |= SPI_MASK_SET_KEYBOARD_PREF;
	sysparam.keyboardPref = FALSE;
	sysparam.params |= SPI_MASK_SET_DRAG_FULL_WINDOWS;
	sysparam.dragFullWindows = FALSE;
	sysparam.params |= SPI_MASK_SET_KEYBOARD_CUES;
	sysparam.keyboardCues = FALSE;
	sysparam.params |= SPI_MASK_SET_WORK_AREA;
	sysparam.workArea.left = 0;
	sysparam.workArea.top = 0;
	sysparam.workArea.right = settings->DesktopWidth;
	sysparam.workArea.bottom = settings->DesktopHeight;
	sysparam.dragFullWindows = FALSE;
	context->ClientSystemParam(context, &sysparam);
	ZeroMemory(&exec, sizeof(RAIL_EXEC_ORDER));
	exec.RemoteApplicationProgram = settings->RemoteApplicationProgram;
	exec.RemoteApplicationWorkingDir = settings->ShellWorkingDirectory;
	exec.RemoteApplicationArguments = settings->RemoteApplicationCmdLine;
	context->ClientExecute(context, &exec);
	return CHANNEL_RC_OK;
}

/**
* Function description
*
* @return 0 on success, otherwise a Win32 error code
*/
static UINT wf_rail_server_handshake_ex(RailClientContext* context,
	RAIL_HANDSHAKE_EX_ORDER* handshakeEx)
{
	return CHANNEL_RC_OK;
}

/**
* Function description
*
* @return 0 on success, otherwise a Win32 error code
*/
static UINT wf_rail_server_local_move_size(RailClientContext* context,
	RAIL_LOCALMOVESIZE_ORDER* localMoveSize)
{
	return CHANNEL_RC_OK;
}


/**
* Function description
*
* @return 0 on success, otherwise a Win32 error code
*/
static UINT wf_rail_server_min_max_info(RailClientContext* context,
	RAIL_MINMAXINFO_ORDER* minMaxInfo)
{
	return CHANNEL_RC_OK;
}


/**
* Function description
*
* @return 0 on success, otherwise a Win32 error code
*/
static UINT wf_rail_server_language_bar_info(RailClientContext* context,
	RAIL_LANGBAR_INFO_ORDER* langBarInfo)
{
	return CHANNEL_RC_OK;
}

/**
* Function description
*
* @return 0 on success, otherwise a Win32 error code
*/
static UINT wf_rail_server_get_appid_response(RailClientContext* context,
	RAIL_GET_APPID_RESP_ORDER* getAppIdResp)
{
	return CHANNEL_RC_OK;
}


static BOOL wf_rail_window_common(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo, WINDOW_STATE_ORDER* windowState)
{
	
	return TRUE;
}

static BOOL wf_rail_window_delete(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo)
{

	return TRUE;
}

static BOOL wf_rail_window_icon(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo, WINDOW_ICON_ORDER* windowIcon) {

	return TRUE;
}

static BOOL wf_rail_window_cached_icon(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo, WINDOW_CACHED_ICON_ORDER* windowCachedIcon)
{
	WLog_DBG(TAG, "RailWindowCachedIcon");
	return TRUE;
}

static BOOL wf_rail_notify_icon_create(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo, NOTIFY_ICON_STATE_ORDER* notifyIconState)
{
	return TRUE;
}

static BOOL wf_rail_notify_icon_update(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo, NOTIFY_ICON_STATE_ORDER* notifyIconState)
{
	return TRUE;
}

static BOOL wf_rail_notify_icon_delete(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo)
{
	swfContext* wfc = (swfContext*)context;
	RailClientContext* rail = wfc->rail;
	WLog_DBG(TAG, "RailNotifyIconDelete");
	return TRUE;
}

static BOOL wf_rail_monitored_desktop(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo, MONITORED_DESKTOP_ORDER* monitoredDesktop)
{
	swfContext* wfc = (swfContext*)context;
	RailClientContext* rail = wfc->rail;
	WLog_DBG(TAG, "RailMonitorDesktop");
	return TRUE;
}

static BOOL wf_rail_non_monitored_desktop(rdpContext* context,
	WINDOW_ORDER_INFO* orderInfo)
{
	swfContext* wfc = (swfContext*)context;
	RailClientContext* rail = wfc->rail;
	WLog_DBG(TAG, "RailNonMonitorDesktop");
	return TRUE;
}



void wf_rail_register_update_callbacks(rdpUpdate* update)
{
	rdpWindowUpdate* window = update->window;
	window->WindowCreate = wf_rail_window_common;
	window->WindowUpdate = wf_rail_window_common;
	window->WindowDelete = wf_rail_window_delete;
	window->WindowIcon = wf_rail_window_icon;
	window->WindowCachedIcon = wf_rail_window_cached_icon;
	window->NotifyIconCreate = wf_rail_notify_icon_create;
	window->NotifyIconUpdate = wf_rail_notify_icon_update;
	window->NotifyIconDelete = wf_rail_notify_icon_delete;
	window->MonitoredDesktop = wf_rail_monitored_desktop;
	window->NonMonitoredDesktop = wf_rail_non_monitored_desktop;
}





BOOL swf_rail_init(swfContext* swfc, RailClientContext* rail)
{
	rdpContext* context = (rdpContext*)swfc;
	swfc->rail = rail;
	rail->custom = (void*)swfc;
	rail->ServerExecuteResult = wf_rail_server_execute_result;
	rail->ServerSystemParam = wf_rail_server_system_param;
	rail->ServerHandshake = wf_rail_server_handshake;
	rail->ServerHandshakeEx = wf_rail_server_handshake_ex;
	rail->ServerLocalMoveSize = wf_rail_server_local_move_size;
	rail->ServerMinMaxInfo = wf_rail_server_min_max_info;
	rail->ServerLanguageBarInfo = wf_rail_server_language_bar_info;
	rail->ServerGetAppIdResponse = wf_rail_server_get_appid_response;
	//wf_rail_register_update_callbacks(context->update);
	return TRUE;
}


void swf_rail_uninit(swfContext* wfc, RailClientContext* rail)
{
	wfc->rail = NULL;
	rail->custom = NULL;
}

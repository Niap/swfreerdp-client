#include <winpr/windows.h>
#include <freerdp/freerdp.h>
#include <freerdp/log.h>
#include <freerdp/gdi/gdi.h>
#include <freerdp/client/channels.h>

#include "swf_event.h"
#include "swf_channels.h"

#define TAG CLIENT_TAG("swindows")

static BOOL swfreerdp_client_global_init(void)
{
	WSADATA wsaData;
	WSAStartup(0x101, &wsaData);

	freerdp_register_addin_provider(freerdp_channels_load_static_addin_entry, 0);
	return TRUE;
}

static void swfreerdp_client_global_uninit(void)
{
	WSACleanup();
}


static BOOL swf_pre_connect(freerdp* instance)
{
	swfContext* swfc;
	rdpContext* context;
	rdpSettings* settings;

	if (!instance || !instance->context || !instance->settings)
		return FALSE;

	context = instance->context;
	swfc = (swfContext*)instance->context;
	settings = instance->settings;
	settings->OsMajorType = OSMAJORTYPE_WINDOWS;
	settings->OsMinorType = OSMINORTYPE_WINDOWS_NT;
	settings->OrderSupport[NEG_DSTBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_PATBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_SCRBLT_INDEX] = TRUE;
	settings->OrderSupport[NEG_OPAQUE_RECT_INDEX] = TRUE;
	settings->OrderSupport[NEG_DRAWNINEGRID_INDEX] = FALSE;
	settings->OrderSupport[NEG_MULTIDSTBLT_INDEX] = FALSE;
	settings->OrderSupport[NEG_MULTIPATBLT_INDEX] = FALSE;
	settings->OrderSupport[NEG_MULTISCRBLT_INDEX] = FALSE;
	settings->OrderSupport[NEG_MULTIOPAQUERECT_INDEX] = TRUE;
	settings->OrderSupport[NEG_MULTI_DRAWNINEGRID_INDEX] = FALSE;
	settings->OrderSupport[NEG_LINETO_INDEX] = TRUE;
	settings->OrderSupport[NEG_POLYLINE_INDEX] = TRUE;
	settings->OrderSupport[NEG_MEMBLT_INDEX] = settings->BitmapCacheEnabled;
	settings->OrderSupport[NEG_MEM3BLT_INDEX] = settings->BitmapCacheEnabled;
	settings->OrderSupport[NEG_MEMBLT_V2_INDEX] = settings->BitmapCacheEnabled;
	settings->OrderSupport[NEG_MEM3BLT_V2_INDEX] = settings->BitmapCacheEnabled;
	settings->OrderSupport[NEG_SAVEBITMAP_INDEX] = FALSE;
	settings->OrderSupport[NEG_GLYPH_INDEX_INDEX] = TRUE;
	settings->OrderSupport[NEG_FAST_INDEX_INDEX] = TRUE;
	settings->OrderSupport[NEG_FAST_GLYPH_INDEX] = TRUE;
	settings->OrderSupport[NEG_POLYGON_SC_INDEX] = TRUE;
	settings->OrderSupport[NEG_POLYGON_CB_INDEX] = TRUE;
	settings->OrderSupport[NEG_ELLIPSE_SC_INDEX] = FALSE;
	settings->OrderSupport[NEG_ELLIPSE_CB_INDEX] = FALSE;

	if (!freerdp_client_load_addins(context->channels, instance->settings))
		return -1;


	PubSub_SubscribeChannelConnected(instance->context->pubSub,swf_OnChannelConnectedEventHandler);
	PubSub_SubscribeChannelDisconnected(instance->context->pubSub,swf_OnChannelDisconnectedEventHandler);

	return TRUE;
}


static BOOL swf_begin_paint(rdpContext* context)
{
	HGDI_DC hdc;

	if (!context || !context->gdi || !context->gdi->primary || !context->gdi->primary->hdc)
		return FALSE;

	hdc = context->gdi->primary->hdc;

	if (!hdc || !hdc->hwnd || !hdc->hwnd->invalid)
		return FALSE;

	hdc->hwnd->invalid->null = TRUE;
	hdc->hwnd->ninvalid = 0;
	return TRUE;
}


static BOOL swf_end_paint(rdpContext* context)
{
	int i;
	rdpGdi* gdi;
	int ninvalid;
	RECT updateRect;
	HGDI_RGN cinvalid;
	REGION16 invalidRegion;
	RECTANGLE_16 invalidRect;
	const RECTANGLE_16* extents;
	swfContext* swfc = (swfContext*)context;
	gdi = context->gdi;
	ninvalid = gdi->primary->hdc->hwnd->ninvalid;
	cinvalid = gdi->primary->hdc->hwnd->cinvalid;

	if (ninvalid < 1)
		return TRUE;

	region16_init(&invalidRegion);

	for (i = 0; i < ninvalid; i++)
	{
		invalidRect.left = cinvalid[i].x;
		invalidRect.top = cinvalid[i].y;
		invalidRect.right = cinvalid[i].x + cinvalid[i].w;
		invalidRect.bottom = cinvalid[i].y + cinvalid[i].h;
		region16_union_rect(&invalidRegion, &invalidRegion, &invalidRect);
	}

	if (!region16_is_empty(&invalidRegion))
	{
		extents = region16_extents(&invalidRegion);
		updateRect.left = extents->left;
		updateRect.top = extents->top;
		updateRect.right = extents->right;
		updateRect.bottom = extents->bottom;
		InvalidateRect(swfc->hwnd, &updateRect, FALSE);
	}

	region16_uninit(&invalidRegion);
	return TRUE;
}


static BOOL wf_post_connect(freerdp* instance)
{
	rdpGdi* gdi;
	DWORD dwStyle;
	swfContext* wfc;
	WCHAR lpWindowName[512];
	const UINT32 format = PIXEL_FORMAT_BGRX32;
	wfc = (swfContext*)instance->context;
	rdpSettings * settings = instance->settings;

	wfc->primary = wf_image_new(wfc, settings->DesktopWidth, settings->DesktopHeight, format, NULL);

	if (!gdi_init_ex(instance, format, 0, wfc->primary->pdata, NULL))
		return FALSE;


	_snwprintf_s(lpWindowName, ARRAYSIZE(lpWindowName), _TRUNCATE, L"FreeRDP");

	dwStyle = WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX
		| WS_MAXIMIZEBOX;

	if (!wfc->hwnd)
	{
		wfc->hwnd = CreateWindowEx((DWORD)NULL, wfc->wndClassName, lpWindowName,
			dwStyle,
			0, 0, 0, 0, wfc->hWndParent, NULL, wfc->hInstance, NULL);
		SetWindowLongPtr(wfc->hwnd, GWLP_USERDATA, (LONG_PTR)wfc);
	}

	swf_resize_window(wfc);

	//BitBlt(wfc->primary->hdc, 0, 0, settings->DesktopWidth, settings->DesktopHeight, NULL, 0, 0, BLACKNESS);

	ShowWindow(wfc->hwnd, SW_SHOWNORMAL);
	UpdateWindow(wfc->hwnd);
	instance->update->BeginPaint = swf_begin_paint;
	instance->update->EndPaint = swf_end_paint;

	return TRUE;
}

static BOOL swf_post_disconnect(freerdp* instance)
{
	return TRUE;
}


static BOOL swfreerdp_client_new(freerdp* instance, rdpContext* context)
{
	if (!(swfreerdp_client_global_init()))
		return FALSE;

	instance->PreConnect = swf_pre_connect;
	instance->PostConnect = wf_post_connect;
	instance->PostDisconnect = swf_post_disconnect;
	return TRUE;
}

static void swfreerdp_client_free(freerdp* instance, rdpContext* context)
{
	if (!context)
		return;
}



static DWORD WINAPI swf_client_thread(LPVOID lpParam)
{
	MSG msg;
	int width;
	int height;
	BOOL msg_ret;
	int quit_msg;
	DWORD nCount;
	DWORD error;
	HANDLE handles[64];
	swfContext* swfc;
	freerdp* instance;
	rdpContext* context;
	rdpChannels* channels;
	rdpSettings* settings;
	BOOL async_input;
	HANDLE input_thread;
	instance = (freerdp*)lpParam;
	context = instance->context;
	swfc = (swfContext*)instance->context;

	if (!freerdp_connect(instance))
		goto end;

	channels = instance->context->channels;
	settings = instance->context->settings;

	while (1)
	{
		nCount = 0;


		{
			DWORD tmp = freerdp_get_event_handles(context, &handles[nCount], 64 - nCount);

			if (tmp == 0)
			{
				WLog_ERR(TAG, "freerdp_get_event_handles failed");
				break;
			}

			nCount += tmp;
		}

		if (MsgWaitForMultipleObjects(nCount, handles, FALSE, 1000,
			QS_ALLINPUT) == WAIT_FAILED)
		{
			WLog_ERR(TAG, "wfreerdp_run: WaitForMultipleObjects failed: 0x%08lX",
				GetLastError());
			break;
		}

		{
			if (!freerdp_check_event_handles(context))
			{
				if (client_auto_reconnect(instance))
					continue;

				WLog_ERR(TAG, "Failed to check FreeRDP file descriptor");
				break;
			}
		}

		if (freerdp_shall_disconnect(instance))
			break;

		quit_msg = FALSE;

		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			msg_ret = GetMessage(&msg, NULL, 0, 0);


			if ((msg_ret == 0) || (msg_ret == -1))
			{
				quit_msg = TRUE;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (quit_msg)
			break;
	}

disconnect:
	freerdp_disconnect(instance);

end:
	error = freerdp_get_last_error(instance->context);
	WLog_DBG(TAG, "Main thread exited with %" PRIu32, error);
	ExitThread(error);
	return error;
}

static int swfreerdp_client_start(rdpContext* context)
{
	HWND hWndParent;
	HINSTANCE hInstance;
	swfContext* swfc = (swfContext*)context;
	freerdp* instance = context->instance;
	hInstance = GetModuleHandle(NULL);

	swfc->hInstance = hInstance;

	swfc->wndClassName = TEXT("FreeRDP");
	swfc->wndClass.cbSize = sizeof(WNDCLASSEX);
	swfc->wndClass.style = CS_HREDRAW | CS_VREDRAW;
	swfc->wndClass.lpfnWndProc = swf_event_proc;
	swfc->wndClass.cbClsExtra = 0;
	swfc->wndClass.cbWndExtra = 0;
	swfc->wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	swfc->wndClass.lpszMenuName = NULL;
	swfc->wndClass.lpszClassName = swfc->wndClassName;
	swfc->wndClass.hInstance = hInstance;
	RegisterClassEx(&(swfc->wndClass));
	

	swfc->thread = CreateThread(NULL, 0, swf_client_thread, (void*)instance, 0,
		&swfc->mainThreadId);

	if (!swfc->thread)
		return -1;

	return 0;
}


static int swfreerdp_client_stop(rdpContext* context)
{
	swfContext* swfc = (swfContext*)context;

	if (swfc->thread)
	{
		PostThreadMessage(swfc->mainThreadId, WM_QUIT, 0, 0);
		WaitForSingleObject(swfc->thread, INFINITE);
		CloseHandle(swfc->thread);
		swfc->thread = NULL;
		swfc->mainThreadId = 0;
	}

	return 0;
}

int RdpClientEntry(RDP_CLIENT_ENTRY_POINTS* pEntryPoints)
{
	pEntryPoints->Version = 1;
	pEntryPoints->Size = sizeof(RDP_CLIENT_ENTRY_POINTS_V1);
	pEntryPoints->GlobalInit = swfreerdp_client_global_init;
	pEntryPoints->GlobalUninit = swfreerdp_client_global_uninit;
	pEntryPoints->ContextSize = sizeof(swfContext);
	pEntryPoints->ClientNew = swfreerdp_client_new;
	pEntryPoints->ClientFree = swfreerdp_client_free;
	pEntryPoints->ClientStart = swfreerdp_client_start;
	pEntryPoints->ClientStop = swfreerdp_client_stop;
	return 0;
}

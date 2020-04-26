#ifndef FREERDP_CLIENT_WIN_EVENT_H
#define FREERDP_CLIENT_WIN_EVENT_H

#include "swf_client.h"

LRESULT CALLBACK swf_event_proc(HWND hWnd, UINT Msg, WPARAM wParam,
	LPARAM lParam);

#endif /* FREERDP_CLIENT_WIN_EVENT_H */

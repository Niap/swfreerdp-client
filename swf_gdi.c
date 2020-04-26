#include "swf_gdi.h"

void swf_update_canvas_diff(swfContext* swfc)
{
	RECT rc_client, rc_wnd;
	int dx, dy;
	GetClientRect(swfc->hwnd, &rc_client);
	GetWindowRect(swfc->hwnd, &rc_wnd);
	dx = (rc_wnd.right - rc_wnd.left) - rc_client.right;
	dy = (rc_wnd.bottom - rc_wnd.top) - rc_client.bottom;

	swfc->diff.x = dx;
	swfc->diff.y = dy;
}


void swf_resize_window(swfContext* swfc)
{
	rdpSettings* settings;
	settings = swfc->context.settings;

	SetWindowLongPtr(swfc->hwnd, GWL_STYLE,
		WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX |
		WS_MAXIMIZEBOX);

	if (!swfc->client_height)
		swfc->client_height = settings->DesktopHeight;

	if (!swfc->client_width)
		swfc->client_width = settings->DesktopWidth;

	if (!swfc->client_x)
		swfc->client_x = 10;

	if (!swfc->client_y)
		swfc->client_y = 10;
	swf_update_canvas_diff(swfc);
	SetWindowPos(swfc->hwnd, HWND_TOP, swfc->client_x, swfc->client_y,
		swfc->client_width + swfc->diff.x, swfc->client_height + swfc->diff.y,
		0 /*SWP_FRAMECHANGED*/);
}

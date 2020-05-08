#include "swf_gdi.h"


BOOL swf_rdp_gdi_dstblt(rdpContext* context, const DSTBLT_ORDER* dstblt) {
	return TRUE;
}

BOOL swf_rdp_gdi_scrblt(rdpContext* context, const SCRBLT_ORDER* scrblt) {
	return TRUE;
}

BOOL swf_rdp_gdi_memblt(rdpContext* context, MEMBLT_ORDER* memblt) {

	int x = memblt->nLeftRect;
	int y = memblt->nTopRect;
	int w = memblt->nWidth;
	int h = memblt->nHeight;

	int x_src = memblt->nXSrc;
	int y_src = memblt->nYSrc;
	switch (memblt->bRop) {
		

	}

	return TRUE;
}


BOOL swf_rdp_gdi_mem3blt(rdpContext* context, MEM3BLT_ORDER* mem3blt) {
	return TRUE;
}

BOOL swf_rdp_gdi_patblt(rdpContext* context, PATBLT_ORDER* patblt) {
	return TRUE;
}
BOOL swf_rdp_gdi_opaque_rect(rdpContext* context,const OPAQUE_RECT_ORDER* opaque_rect) {
	return TRUE;
}

BOOL swf_rdp_gdi_multi_opaque_rect(rdpContext* context, const MULTI_OPAQUE_RECT_ORDER* multi_opaque_rect) {
	return TRUE;
}


BOOL swf_rdp_gdi_line_to(rdpContext* context, const LINE_TO_ORDER* line_to) {
	return TRUE;
}


BOOL swf_rdp_gdi_polyline(rdpContext* context, const POLYLINE_ORDER* polyline) {
	return TRUE;
}

BOOL swf_rdp_gdi_surface_frame_marker(rdpContext* context, const SURFACE_FRAME_MARKER* surface_frame_marker) {
	return TRUE;
}

BOOL swf_rdp_gdi_palette_update(rdpContext* context,const PALETTE_UPDATE* palette)
{
	return TRUE;
}

BOOL swf_rdp_gdi_set_bounds(rdpContext* context, const rdpBounds* bounds) {
	return TRUE;
}

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

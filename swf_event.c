#include "swf_event.h"

LRESULT CALLBACK swf_event_proc(HWND hWnd, UINT Msg, WPARAM wParam,
	LPARAM lParam) {

	HDC hdc;
	PAINTSTRUCT ps;
	LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	swfContext* swfc = (swfContext*)ptr;
	UINT32 ww, wh, dw, dh;
	rdpSettings* settings = NULL;

	switch (Msg)
	{
		case WM_DESTROY:
			PostQuitMessage(WM_QUIT);
		break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);			
			settings = swfc->context.settings;
			ww = swfc->client_width;
			wh = swfc->client_height;
			dw = settings->DesktopWidth;
			dh = settings->DesktopHeight;

			StretchBlt(hdc, 0, 0, ww, wh, swfc->primary->hdc, 0, 0, dw, dh, SRCCOPY);

			EndPaint(hWnd, &ps);
		break;
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
			break;
	}
}
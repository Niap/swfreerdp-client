#include "swf_event.h"
#include "swf_clipboard.h"

typedef BOOL(WINAPI* fnAddClipboardFormatListener)(HWND hwnd);



#define X_POS(lParam) ((UINT16) (lParam & 0xFFFF))
#define Y_POS(lParam) ((UINT16) ((lParam >> 16) & 0xFFFF))

LRESULT CALLBACK swf_event_proc(HWND hWnd, UINT Msg, WPARAM wParam,
	LPARAM lParam) {

	HDC hdc;
	PAINTSTRUCT ps;
	LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
	swfContext* swfc = (swfContext*)ptr;
	UINT32 ww, wh, dw, dh;
	rdpSettings* settings = NULL;
	HMODULE hUser32;
	fnAddClipboardFormatListener AddClipboardFormatListener;


	int count;
	CLIPRDR_FORMAT* formats;
	CLIPRDR_FORMAT_LIST formatList;
	UINT32 numFormats;
	UINT32 formatId = 0;
	UINT32 index=0;
	char formatName[1024];

	DWORD rdp_scancode;
	rdpInput* input;

	switch (Msg)
	{
		case WM_CREATE:
			hUser32 = LoadLibraryA("user32.dll");
			AddClipboardFormatListener = (fnAddClipboardFormatListener)GetProcAddress(hUser32, "AddClipboardFormatListener");
			AddClipboardFormatListener(hWnd);
		break;
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
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			rdp_scancode = (lParam >> 16) & 0xff;
			input = swfc->context.input;
			freerdp_input_send_keyboard_event_ex(input, TRUE,rdp_scancode);
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			rdp_scancode = lParam >> 16 & 0xff;
			input = swfc->context.input;
			freerdp_input_send_keyboard_event_ex(input, FALSE, rdp_scancode);
			break;
		case WM_MOUSEMOVE:
			input = swfc->context.input;
			freerdp_input_send_mouse_event(input, PTR_FLAGS_MOVE, X_POS(lParam), Y_POS(lParam));
			break;
		case WM_LBUTTONDOWN:
			input = swfc->context.input;
			freerdp_input_send_mouse_event(input, PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON1,X_POS(lParam), Y_POS(lParam));
			break;
		case WM_LBUTTONUP:
			input = swfc->context.input;
			freerdp_input_send_mouse_event(input, PTR_FLAGS_BUTTON1, X_POS(lParam), Y_POS(lParam));
			break;

		case WM_RBUTTONDOWN:
			input = swfc->context.input;
			freerdp_input_send_mouse_event(input, PTR_FLAGS_DOWN | PTR_FLAGS_BUTTON2,X_POS(lParam), Y_POS(lParam));
			break;

		case WM_RBUTTONUP:
			input = swfc->context.input;
			freerdp_input_send_mouse_event(input, PTR_FLAGS_BUTTON2, X_POS(lParam), Y_POS(lParam));
			break;


		case WM_CLIPBOARDUPDATE:
			cliprdr_send_format_list(swfc->clipboard->context);
			break;
		
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
			break;
	}
}
#ifndef FREERDP_CLIENT_WIN_INTERFACE_H
#define FREERDP_CLIENT_WIN_INTERFACE_H
#include <freerdp/freerdp.h>
#include <freerdp/api.h>
#include <freerdp/freerdp.h>
#include <freerdp/gdi/gdi.h>
#include <freerdp/gdi/dc.h>
#include <freerdp/gdi/region.h>
#include <freerdp/cache/cache.h>
#include <freerdp/codec/color.h>

#include <freerdp/client/rail.h>
#include <freerdp/channels/channels.h>
#include <freerdp/codec/rfx.h>
#include <freerdp/codec/nsc.h>
#include <freerdp/client/file.h>
#include <freerdp/client/cliprdr.h>

typedef struct _swfClipboard {
	char mimetype[256];
	char* buffer;
	int length;
	CliprdrClientContext* context;
} swfClipboard;

typedef struct swf_bitmap
{
	rdpBitmap _bitmap;
	HDC hdc;
	HBITMAP bitmap;
	HBITMAP org_bitmap;
	BYTE* pdata;
}swfBitmap;

typedef struct swf_context
{
	rdpContext context;
	DEFINE_RDP_CLIENT_COMMON();
	swfBitmap* primary;

	HWND hWndParent;
	HINSTANCE hInstance;
	WNDCLASSEX wndClass;
	LPCTSTR wndClassName;
	HCURSOR hDefaultCursor;

	DWORD mainThreadId;

	HWND hwnd;

	int client_x;
	int client_y;
	int client_width;
	int client_height;


	POINT diff;


	RailClientContext* rail;
	swfClipboard * clipboard;
} swfContext;


FREERDP_API int RdpClientEntry(RDP_CLIENT_ENTRY_POINTS* pEntryPoints);

#endif
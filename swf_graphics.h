#ifndef FREERDP_CLIENT_WIN_GRAPHICS_H
#define FREERDP_CLIENT_WIN_GRAPHICS_H

#include "swf_client.h"


swfBitmap* swf_image_new(swfContext* wfc, UINT32 width, UINT32 height,
	UINT32 format, const BYTE* data);


#endif /* FREERDP_CLIENT_WIN_GRAPHICS_H */
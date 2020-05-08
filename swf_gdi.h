
#ifndef FREERDP_CLIENT_WIN_GDI_H
#define FREERDP_CLIENT_WIN_GDI_H

#include "swf_client.h"

//void swf_invalidate_region(swfContext* swfc, UINT32 x, UINT32 y, UINT32 width,UINT32 height);
void swf_resize_window(swfContext* swfc);
BOOL swf_rdp_gdi_dstblt(rdpContext* context, const DSTBLT_ORDER* dstblt);
BOOL swf_rdp_gdi_scrblt(rdpContext* context, const SCRBLT_ORDER* scrblt);
BOOL swf_rdp_gdi_memblt(rdpContext* context, MEMBLT_ORDER* memblt);
BOOL swf_rdp_gdi_mem3blt(rdpContext* context, MEM3BLT_ORDER* mem3blt);
BOOL swf_rdp_gdi_patblt(rdpContext* context, PATBLT_ORDER* patblt);
BOOL swf_rdp_gdi_opaque_rect(rdpContext* context, const OPAQUE_RECT_ORDER* opaque_rect);
BOOL swf_rdp_gdi_multi_opaque_rect(rdpContext* context, const MULTI_OPAQUE_RECT_ORDER* multi_opaque_rect);
BOOL swf_rdp_gdi_line_to(rdpContext* context, const LINE_TO_ORDER* line_to);
BOOL swf_rdp_gdi_polyline(rdpContext* context, const POLYLINE_ORDER* polyline);
BOOL swf_rdp_gdi_surface_frame_marker(rdpContext* context, const SURFACE_FRAME_MARKER* surface_frame_marker);

BOOL swf_rdp_gdi_palette_update(rdpContext* context, const PALETTE_UPDATE* palette);
BOOL swf_rdp_gdi_set_bounds(rdpContext* context, const rdpBounds* bounds);

#endif /* FREERDP_CLIENT_WIN_GDI_H */

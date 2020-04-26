#include "swf_graphics.h"


HBITMAP swf_create_dib(swfContext* swfc, UINT32 width, UINT32 height,
	UINT32 srcFormat, const BYTE* data, BYTE** pdata)
{
	HDC hdc;
	int negHeight;
	HBITMAP bitmap;
	BITMAPINFO bmi;
	BYTE* cdata = NULL;
	UINT32 dstFormat = srcFormat;
	/**
	* See: http://msdn.microsoft.com/en-us/library/dd183376
	* if biHeight is positive, the bitmap is bottom-up
	* if biHeight is negative, the bitmap is top-down
	* Since we get top-down bitmaps, let's keep it that way
	*/
	negHeight = (height < 0) ? height : height * (-1);
	hdc = GetDC(NULL);
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = negHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = GetBitsPerPixel(dstFormat);
	bmi.bmiHeader.biCompression = BI_RGB;
	bitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&cdata, NULL, 0);

	if (data)
		freerdp_image_copy(cdata, dstFormat, 0, 0, 0, width, height, data, srcFormat, 0,
			0, 0, &swfc->context.gdi->palette, FREERDP_FLIP_NONE);

	if (pdata)
		*pdata = cdata;

	ReleaseDC(NULL, hdc);
	GdiFlush();
	return bitmap;
}


swfBitmap* wf_image_new(swfContext* wfc, UINT32 width, UINT32 height,
	UINT32 format, const BYTE* data)
{
	HDC hdc;
	swfBitmap* image;
	hdc = GetDC(NULL);
	image = (swfBitmap*)malloc(sizeof(swfBitmap));
	image->hdc = CreateCompatibleDC(hdc);
	image->bitmap = swf_create_dib(wfc, width, height, format, data,
		&(image->pdata));
	image->org_bitmap = (HBITMAP)SelectObject(image->hdc, image->bitmap);
	ReleaseDC(NULL, hdc);
	return image;
}

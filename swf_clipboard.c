#include "swf_clipboard.h"


UINT cliprdr_send_capabilities(CliprdrClientContext* clipboard) {

	CLIPRDR_GENERAL_CAPABILITY_SET cap_set = {
		.capabilitySetType = CB_CAPSTYPE_GENERAL, /* CLIPRDR specification requires that this is CB_CAPSTYPE_GENERAL, the only defined set type */
		.capabilitySetLength = 12, /* The size of the capability set within the PDU - for CB_CAPSTYPE_GENERAL, this is ALWAYS 12 bytes */
		.version = CB_CAPS_VERSION_2, /* The version of the CLIPRDR specification supported */
		.generalFlags = CB_USE_LONG_FORMAT_NAMES /* Bitwise OR of all supported feature flags */
	};

	CLIPRDR_CAPABILITIES caps = {
		.cCapabilitiesSets = 1,
		.capabilitySets = (CLIPRDR_CAPABILITY_SET*)&cap_set
	};
	return clipboard->ClientCapabilities(clipboard, &caps);
}



UINT cliprdr_send_format_list(CliprdrClientContext* cliprdr) {

	CLIPRDR_FORMAT_LIST format_list = {
		.formats = (CLIPRDR_FORMAT[]) {
			{.formatId = CF_TEXT },
			{ .formatId = CF_UNICODETEXT }
		},
		.numFormats = 2
	};

	return cliprdr->ClientFormatList(cliprdr, &format_list);
}

UINT cliprdr_send_format_data_request(CliprdrClientContext* cliprdr, UINT32 format) {

	CLIPRDR_FORMAT_LIST_RESPONSE format_list_response = {
		.msgFlags = CB_RESPONSE_OK
	};

	/* Report successful processing of format list */
	cliprdr->ClientFormatListResponse(cliprdr, &format_list_response);
}


UINT swf_cliprdr_monitor_ready(CliprdrClientContext* context,
	CLIPRDR_MONITOR_READY* monitorReady)
{
	UINT rc = ERROR_INTERNAL_ERROR;
	swfClipboard* clipboard = (swfClipboard*)context->custom;

	if (!context || !monitorReady)
		return ERROR_INTERNAL_ERROR;

	rc = cliprdr_send_capabilities(context);
	if (rc != CHANNEL_RC_OK)
		return rc;

	return cliprdr_send_format_list(context);

}

UINT swf_cliprdr_server_format_data_request(CliprdrClientContext* context, CLIPRDR_FORMAT_DATA_REQUEST* formatDataRequest) {
	UINT rc = CHANNEL_RC_OK;
	HANDLE hClipdata;
	swfContext * swfContxt = (swfContext *)context->custom;
	UINT32 requestedFormatId = formatDataRequest->requestedFormatId;
	CLIPRDR_FORMAT_DATA_RESPONSE response;
	size_t size = 0;
	char* globlemem = NULL;
	void* buff = NULL;

	if (!OpenClipboard(swfContxt->hwnd))
		return ERROR_INTERNAL_ERROR;

	hClipdata = GetClipboardData(requestedFormatId);

	if (!hClipdata)
	{
		CloseClipboard();
		return ERROR_INTERNAL_ERROR;
	}
	globlemem = (char*)GlobalLock(hClipdata);
	size = (int)GlobalSize(hClipdata);
	buff = malloc(size);
	CopyMemory(buff, globlemem, size);
	GlobalUnlock(hClipdata);
	CloseClipboard();

	response.msgFlags = CB_RESPONSE_OK;
	response.dataLen = size;
	response.requestedFormatData = (BYTE*)buff;
	rc = context->ClientFormatDataResponse(context,&response);
	free(buff);
	return rc;
}

UINT swf_cliprdr_server_format_data_response(CliprdrClientContext* context,CLIPRDR_FORMAT_DATA_RESPONSE* format_data_response) {

	return CHANNEL_RC_OK;
}

UINT swf_cliprdr_server_format_list(CliprdrClientContext* context,CLIPRDR_FORMAT_LIST* formatList) {
	UINT rc = CHANNEL_RC_OK;
	return rc;
}


BOOL swf_cliprdr_init(swfContext* swfc, CliprdrClientContext* cliprdr) {

	swfClipboard * clipboard = (swfClipboard*)calloc(1, sizeof(swfClipboard));
	clipboard->context = cliprdr;
	swfc->clipboard = clipboard;
	cliprdr->custom = clipboard;

	cliprdr->MonitorReady = swf_cliprdr_monitor_ready;
	//cliprdr->ServerFormatList = swf_cliprdr_server_format_list;
	cliprdr->ServerFormatDataRequest = swf_cliprdr_server_format_data_request;
	cliprdr->ServerFormatDataResponse = swf_cliprdr_server_format_data_response;


	return TRUE;
}

void swf_cliprdr_uninit(swfContext* swfc, CliprdrClientContext* cliprdr) {
	swfc->clipboard = NULL;
	cliprdr->custom = NULL;
}
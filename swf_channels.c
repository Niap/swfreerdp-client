#include "swf_client.h"
#include "swf_rail.h"

#include <freerdp/gdi/gfx.h>
#include <freerdp/log.h>
#include <freerdp/client/cliprdr.h>

#define TAG CLIENT_TAG("windows")

void swf_OnChannelConnectedEventHandler(void* context,
	ChannelConnectedEventArgs* e)
{
	swfContext* swfc = (swfContext*)context;
	rdpSettings* settings = swfc->context.settings;
	if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0)
	{
		swf_rail_init(swfc, (RailClientContext*)e->pInterface);
	}
	else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0)
	{
		swf_cliprdr_init(swfc, (CliprdrClientContext*)e->pInterface);
	}
	
}

void swf_OnChannelDisconnectedEventHandler(void* context,
	ChannelDisconnectedEventArgs* e)
{
	swfContext* swfc = (swfContext*)context;
	rdpSettings* settings = swfc->context.settings;

	if (strcmp(e->name, RAIL_SVC_CHANNEL_NAME) == 0)
	{
		swf_rail_uninit(swfc, (RailClientContext*)e->pInterface);
	}
	else if (strcmp(e->name, CLIPRDR_SVC_CHANNEL_NAME) == 0)
	{
		swf_cliprdr_uninit(swfc, (CliprdrClientContext*)e->pInterface);
	}
	
}

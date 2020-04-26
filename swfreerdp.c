#include <winpr/windows.h>

#include <shellapi.h>
#include <freerdp/freerdp.h>
#include "swf_client.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "freerdp2.lib")
#pragma comment(lib, "freerdp-client2.lib")
#pragma comment(lib, "winpr2.lib")


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	int status;
	HANDLE thread;
	swfContext* swfc;
	DWORD dwExitCode;
	rdpContext* context;
	rdpSettings* settings;
	RDP_CLIENT_ENTRY_POINTS clientEntryPoints;
	int ret = 1;
	int argc = 0, i;
	LPWSTR* args = NULL;
	LPWSTR cmd;
	char** argv=NULL;
	ZeroMemory(&clientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	clientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
	clientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
	RdpClientEntry(&clientEntryPoints);
	context = freerdp_client_context_new(&clientEntryPoints);

	if (!context)
		return -1;

	cmd = GetCommandLineW();

	if (!cmd)
		goto out;

	args = CommandLineToArgvW(cmd, &argc);

	if (!args)
		goto out;

	argv = calloc(argc, sizeof(char*));

	if (!argv)
		goto out;

	for (i = 0; i < argc; i++)
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, args[i], -1, NULL, 0, NULL, NULL);
		argv[i] = calloc(size, sizeof(char));

		if (!argv[i])
			goto out;

		if (WideCharToMultiByte(CP_UTF8, 0, args[i], -1, argv[i], size, NULL,
			NULL) != size)
			goto out;
	}

	settings = context->settings;
	swfc = (swfContext*)context;

	if (!settings || !swfc)
		goto out;

	status = freerdp_client_settings_parse_command_line(settings, argc, argv,
		FALSE);

	if (status)
	{
		freerdp_client_settings_command_line_status_print(settings, status, argc, argv);
		goto out;
	}

	if (freerdp_client_start(context) != 0)
		goto out;

	thread = freerdp_client_get_thread(context);

	if (thread)
	{
		if (WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0)
		{
			GetExitCodeThread(thread, &dwExitCode);
			ret = dwExitCode;
		}
	}

	if (freerdp_client_stop(context) != 0)
		goto out;

out:
	freerdp_client_context_free(context);

	if (argv)
	{
		for (i = 0; i < argc; i++)
			free(argv[i]);

		free(argv);
	}

	LocalFree(args);
	return ret;
}

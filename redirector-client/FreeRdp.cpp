#include "FreeRdp.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <winpr/windows.h>
#include "wf_client.h"

#include <winpr/crt.h>
#include <winpr/credui.h>

#include <freerdp/freerdp.h>
#include <freerdp/constants.h>

#include <freerdp/client/file.h>
#include <freerdp/client/cmdline.h>
#include <freerdp/client/channels.h>
#include <freerdp/channels/channels.h>

int InitalArguments(rdpContext* context, LPSTR lpCmdLine)
{
	int count = 1;
	for (int i = 0; i < strlen(lpCmdLine); i++)
	{
		if (lpCmdLine[i] == ' ')
		{
			count++;
		}
	}
	context->argc = 1 + count;
	context->argv = (char**)malloc(sizeof(char*) * count);
	if (!context->argv)
	{
		return 1;
	}
	context->argv[0] = _strdup("");
	count = 1;
	char *charcontext = NULL;
	char* p = strtok_s(lpCmdLine, " ", &charcontext);
	while (p != NULL)
	{
		OutputDebugStringA(p);
		OutputDebugStringA("\r\n");
		context->argv[count] = _strdup(p);
		p = strtok_s(NULL, " ", &charcontext);
		count++;
	}
	return 0;
}
FreeRdp::FreeRdp()
{
}
//
//
FreeRdp::~FreeRdp()
{
}
//
int FreeRdp::start(char* para)
{
	int index;
	int status;
	
	rdpSettings* settings;
	RDP_CLIENT_ENTRY_POINTS clientEntryPoints;
	int ret = 0;

	ZeroMemory(&clientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	clientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
	clientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;

	RdpClientEntry(&clientEntryPoints);

	context = freerdp_client_context_new(&clientEntryPoints);

	settings = context->settings;

	settings->SoftwareGdi = TRUE;
	settings->ColorDepth = 32; // WTF???

	if (InitalArguments(context, para) == 1)
	{
		return 1;
	}

	status = freerdp_client_settings_parse_command_line(settings, context->argc, context->argv, FALSE);
	freerdp_client_start(context);

	return 0;
}
//
int FreeRdp::stop()
{
	freerdp_client_stop(context);
	return 0;
}

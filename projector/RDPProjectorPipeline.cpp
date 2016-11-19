#include "RDPProjectorPipeline.h"
extern "C"
{
#include"rdpgfx_main.h"
#include "wf_client.h"
#include "drdynvc_main.h"
#include "rdpdr_main.h"
#include "rdpsnd_main.h"
__declspec(dllimport) drdynvcPlugin* g_drdynvc;
__declspec(dllimport) BOOL g_IsAlive;
__declspec(dllimport) rdpdrPlugin* g_rdpdrPlugin;
__declspec(dllimport) rdpsndPlugin* g_rdpsndPlugin;
}
#include <string>
rdpContext* g_context = NULL;
const int Max_Hostname_Len = 40;
RDPProjectorPipeline::RDPProjectorPipeline() :m_rdynvcPlugin(nullptr), m_rdpdrPlugin(nullptr), m_rdpsndPlugin(nullptr)
{ 
}										 

RDPProjectorPipeline::~RDPProjectorPipeline()
{
}
void  RDPProjectorPipeline::Stop()
{
	if (g_context)
	{
		freerdp_client_stop(g_context);
		freerdp_client_context_free(g_context);
		g_context = NULL;
	}
	g_rdpdrPlugin = NULL;
	g_rdpsndPlugin = NULL;
	g_IsAlive = FALSE;
	g_drdynvc = NULL;
}
void RDPProjectorPipeline::Start()
{
	int index;
	int status;
	HANDLE thread;
	DWORD dwExitCode;
	rdpSettings* settings;
	RDP_CLIENT_ENTRY_POINTS clientEntryPoints;
	int ret = 0;

	ZeroMemory(&clientEntryPoints, sizeof(RDP_CLIENT_ENTRY_POINTS));
	clientEntryPoints.Size = sizeof(RDP_CLIENT_ENTRY_POINTS);
	clientEntryPoints.Version = RDP_CLIENT_INTERFACE_VERSION;
	RdpClientEntry(&clientEntryPoints);
	g_context = freerdp_client_context_new(&clientEntryPoints);
	settings = g_context->settings;
	char *hostname = (char*)malloc(Max_Hostname_Len*sizeof(char));
	std::string shostname("RdpProjector");
	memcpy(hostname, shostname.c_str(), shostname.length()+1);
	settings->ServerHostname = hostname;
	settings->SupportGraphicsPipeline = TRUE;
	settings->SoftwareGdi = TRUE;
	settings->Fullscreen = TRUE;
	g_IsAlive = TRUE;
	g_context->argc = __argc;
	g_context->argv = (char**)malloc(sizeof(char*) * (__argc + 1)); // to fix 

	if (!g_context->argv)
	{
		ret = 1;
		printf("cmdline input error\n");
		return;
	}
	for (index = 0; index < g_context->argc; index++)
	{
		g_context->argv[index] = _strdup("");
		if (!g_context->argv[index])
		{
			ret = 1;
			for (--index; index >= 0; --index)
				free(g_context->argv[index]);
			free(g_context->argv);
			g_context->argv = NULL;
		}

	}
	// to fix
	g_context->argc = 2;
	g_context->argv[1] = "/vc:rdpsnd";
	status = freerdp_client_settings_parse_command_line(settings, g_context->argc, g_context->argv, FALSE);
	if (status)//Initialized failed
	{
		freerdp_client_context_free(g_context);
		return;
	}
	ret = freerdp_client_start(g_context);
		
	if (g_drdynvc == NULL || g_rdpdrPlugin == NULL || g_rdpsndPlugin == NULL || ret)
	{
		printf("g_drdynvc,g_rdpdrPlugin ,g_rdpsndPlugin could not be NULL \n");
		return;
	}
	m_rdynvcPlugin = g_drdynvc;
	m_rdpdrPlugin = g_rdpdrPlugin;
	m_rdpsndPlugin = g_rdpsndPlugin;
	return;

}
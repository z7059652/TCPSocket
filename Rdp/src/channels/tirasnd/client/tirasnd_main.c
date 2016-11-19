/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Audio Output Virtual Channel
 *
 * Copyright 2009-2011 Jay Sorg
 * Copyright 2010-2011 Vic Lee
 * Copyright 2012-2013 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2015 Thincast Technologies GmbH
 * Copyright 2015 DI (FH) Martin Haimberger <martin.haimberger@thincast.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _WIN32
#include <sys/time.h>
#include <signal.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winpr/crt.h>
#include <winpr/wlog.h>
#include <winpr/stream.h>
#include <winpr/cmdline.h>
#include <winpr/sysinfo.h>
#include <winpr/collections.h>

#include <freerdp/types.h>
#include <freerdp/addin.h>

#include "tirasnd_main.h"

#define TIME_DELAY_MS	65
void(*gSendAudioplayData)(void *data, unsigned int size);
_declspec(dllexport) void(*ginjectAudioFormatInfo)(void *data, unsigned int size);
rdpsndPlugin* grdpsnd = NULL;
void injectAudioFormatInfo(void *data, unsigned int size)
{
	wStream* pdu = Stream_New(NULL, size);
	Stream_Write(pdu, data, size);
	if (grdpsnd)
	tirasnd_virtual_channel_write(grdpsnd, pdu);
}

struct rdpsnd_plugin
{
	CHANNEL_DEF channelDef;
	CHANNEL_ENTRY_POINTS_FREERDP channelEntryPoints;

	HANDLE thread;
	wStream* data_in;
	void* InitHandle;
	DWORD OpenHandle;
	wMessagePipe* MsgPipe;

	wLog* log;
	HANDLE stopEvent;
	HANDLE ScheduleThread;

	BYTE cBlockNo;
	UINT16 wQualityMode;
	int wCurrentFormatNo;

	AUDIO_FORMAT* ServerFormats;
	UINT16 NumberOfServerFormats;

	AUDIO_FORMAT* ClientFormats;
	UINT16 NumberOfClientFormats;

	BOOL expectingWave;
	BYTE waveData[4];
	UINT16 waveDataSize;
	UINT32 wTimeStamp;

	int latency;
	BOOL isOpen;
	UINT16 fixedFormat;
	UINT16 fixedChannel;
	UINT32 fixedRate;

	char* subsystem;
	char* device_name;

	/* Device plugin */
	rdpsndDevicePlugin* device;
	rdpContext* rdpcontext;
};

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_confirm_wave(rdpsndPlugin* rdpsnd, RDPSND_WAVE* wave);

static void* tirardpsnd_schedule_thread(void* arg)
{
	wMessage message;
	UINT16 wTimeDiff;
	UINT16 wTimeStamp;
	UINT16 wCurrentTime;
	RDPSND_WAVE* wave;
	rdpsndPlugin* rdpsnd = (rdpsndPlugin*) arg;
	HANDLE events[2];
	UINT error = CHANNEL_RC_OK;
    DWORD status;

	events[0] = MessageQueue_Event(rdpsnd->MsgPipe->Out);
	events[1] = rdpsnd->stopEvent;

	while (1)
	{
        status = WaitForMultipleObjects(2, events, FALSE, INFINITE);

        if (status == WAIT_FAILED)
        {
            error = GetLastError();
            WLog_ERR(TAG, "WaitForMultipleObjects failed with error %lu!", error);
            break;
        }

        status = WaitForSingleObject(rdpsnd->stopEvent, 0);

        if (status == WAIT_FAILED)
        {
            error = GetLastError();
            WLog_ERR(TAG, "WaitForSingleObject failed with error %lu!", error);
            break;
        }

        if (status == WAIT_OBJECT_0)
            break;


        status = WaitForSingleObject(events[0], 0);

        if (status == WAIT_FAILED)
        {
            error = GetLastError();
            WLog_ERR(TAG, "WaitForSingleObject failed with error %lu!", error);
            break;
        }


        if (!MessageQueue_Peek(rdpsnd->MsgPipe->Out, &message, TRUE))
		{
			WLog_ERR(TAG, "MessageQueue_Peek failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (message.id == WMQ_QUIT)
			break;

		wave = (RDPSND_WAVE*) message.wParam;
		wCurrentTime = (UINT16) GetTickCount();
		wTimeStamp = wave->wLocalTimeB;

		if (wCurrentTime <= wTimeStamp)
		{
			wTimeDiff = wTimeStamp - wCurrentTime;
			Sleep(wTimeDiff);
		}

		if ((error = tirardpsnd_confirm_wave(rdpsnd, wave)))
		{
			WLog_ERR(TAG, "error confirming wave");
			break;
		}

		message.wParam = NULL;
		free(wave);
	}

	if (error && rdpsnd->rdpcontext)
		setChannelError(rdpsnd->rdpcontext, error, "rdpsnd_schedule_thread reported an error");

	ExitThread((DWORD)error);
	return NULL;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
UINT tirardpsnd_send_quality_mode_pdu(rdpsndPlugin* rdpsnd)
{
	wStream* pdu;

	pdu = Stream_New(NULL, 8);
	if (!pdu)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}
	Stream_Write_UINT8(pdu, SNDC_QUALITYMODE); /* msgType */
	Stream_Write_UINT8(pdu, 0); /* bPad */
	Stream_Write_UINT16(pdu, 4); /* BodySize */
	Stream_Write_UINT16(pdu, rdpsnd->wQualityMode); /* wQualityMode */
	Stream_Write_UINT16(pdu, 0); /* Reserved */

	WLog_Print(rdpsnd->log, WLOG_DEBUG, "QualityMode: %d", rdpsnd->wQualityMode);

	return tirasnd_virtual_channel_write(rdpsnd, pdu);
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
UINT tirardpsnd_send_training_confirm_pdu(rdpsndPlugin* rdpsnd, UINT16 wTimeStamp, UINT16 wPackSize)
{
	wStream* pdu;

	pdu = Stream_New(NULL, 8);
	if (!pdu)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	Stream_Write_UINT8(pdu, SNDC_TRAINING); /* msgType */
	Stream_Write_UINT8(pdu, 0); /* bPad */
	Stream_Write_UINT16(pdu, 4); /* BodySize */
	Stream_Write_UINT16(pdu, wTimeStamp);
	Stream_Write_UINT16(pdu, wPackSize);

	WLog_Print(rdpsnd->log, WLOG_DEBUG, "Training Response: wTimeStamp: %d wPackSize: %d",
			wTimeStamp, wPackSize);

	return tirasnd_virtual_channel_write(rdpsnd, pdu);
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_recv_training_pdu(rdpsndPlugin* rdpsnd, wStream* s)
{
	UINT16 wTimeStamp;
	UINT16 wPackSize;

	if (Stream_GetRemainingLength(s) < 4)
		return ERROR_BAD_LENGTH;

	Stream_Read_UINT16(s, wTimeStamp);
	Stream_Read_UINT16(s, wPackSize);

	WLog_Print(rdpsnd->log, WLOG_DEBUG, "Training Request: wTimeStamp: %d wPackSize: %d",
			wTimeStamp, wPackSize);

	return rdpsnd_send_training_confirm_pdu(rdpsnd, wTimeStamp, wPackSize);
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
UINT tirardpsnd_send_wave_confirm_pdu(rdpsndPlugin* rdpsnd, UINT16 wTimeStamp, BYTE cConfirmedBlockNo)
{
	wStream* pdu;

	pdu = Stream_New(NULL, 8);
	if (!pdu)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	Stream_Write_UINT8(pdu, SNDC_WAVECONFIRM);
	Stream_Write_UINT8(pdu, 0);
	Stream_Write_UINT16(pdu, 4);
	Stream_Write_UINT16(pdu, wTimeStamp);
	Stream_Write_UINT8(pdu, cConfirmedBlockNo); /* cConfirmedBlockNo */
	Stream_Write_UINT8(pdu, 0); /* bPad */

	return tirasnd_virtual_channel_write(rdpsnd, pdu);
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
UINT tirardpsnd_confirm_wave(rdpsndPlugin* rdpsnd, RDPSND_WAVE* wave)
{
	WLog_Print(rdpsnd->log, WLOG_DEBUG, "WaveConfirm: cBlockNo: %d wTimeStamp: %d wTimeDiff: %d",
			wave->cBlockNo, wave->wTimeStampB, wave->wTimeStampB - wave->wTimeStampA);

	return rdpsnd_send_wave_confirm_pdu(rdpsnd, wave->wTimeStampB, wave->cBlockNo);
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_device_send_wave_confirm_pdu(rdpsndDevicePlugin* device, RDPSND_WAVE* wave)
{
	if (device->DisableConfirmThread)
		return tirardpsnd_confirm_wave(device->rdpsnd, wave);

	if (!MessageQueue_Post(device->rdpsnd->MsgPipe->Out, NULL, 0, (void*) wave, NULL))
	{
		WLog_ERR(TAG, "MessageQueue_Post failed!");
		return ERROR_INTERNAL_ERROR;
	}

	return CHANNEL_RC_OK;

}


/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_recv_pdu(rdpsndPlugin* rdpsnd, wStream* s)
{
	BYTE msgType;
	UINT16 BodySize;
	UINT status = CHANNEL_RC_OK;

	if (rdpsnd->expectingWave)
		goto out;

	if (Stream_GetRemainingLength(s) < 4)
	{
		status = ERROR_BAD_LENGTH;
		goto out;
	}

	Stream_Read_UINT8(s, msgType); /* msgType */
	Stream_Seek_UINT8(s); /* bPad */
	Stream_Read_UINT16(s, BodySize);


	switch (msgType)
	{
	case SNDC_TRAINING:
		status = tirardpsnd_recv_training_pdu(rdpsnd, s);
		break;

		default:
			break;
	}

out:
	Stream_Free(s, TRUE);
	return status;
}

static void tirardpsnd_register_device_plugin(rdpsndPlugin* rdpsnd, rdpsndDevicePlugin* device)
{
	if (rdpsnd->device)
	{
		WLog_ERR(TAG, "existing device, abort.");
		return;
	}

	rdpsnd->device = device;
	device->rdpsnd = rdpsnd;

	device->WaveConfirm = tirardpsnd_device_send_wave_confirm_pdu;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_load_device_plugin(rdpsndPlugin* rdpsnd, const char* name, ADDIN_ARGV* args)
{
	PFREERDP_RDPSND_DEVICE_ENTRY entry;
	FREERDP_RDPSND_DEVICE_ENTRY_POINTS entryPoints;\
	UINT error;

	entry = (PFREERDP_RDPSND_DEVICE_ENTRY) freerdp_load_channel_addin_entry("rdpsnd", (LPSTR) name, NULL, 0);

	if (!entry)
		return ERROR_INTERNAL_ERROR;

	entryPoints.rdpsnd = rdpsnd;
	entryPoints.pRegisterRdpsndDevice = tirardpsnd_register_device_plugin;
	entryPoints.args = args;

	if ((error = entry(&entryPoints)))
		WLog_ERR(TAG, "%s entry returns error %lu", name, error);

	return error;
}

BOOL tirardpsnd_set_subsystem(rdpsndPlugin* rdpsnd, const char* subsystem)
{
	free(rdpsnd->subsystem);
	rdpsnd->subsystem = _strdup(subsystem);
	return (rdpsnd->subsystem != NULL);
}

BOOL tirardpsnd_set_device_name(rdpsndPlugin* rdpsnd, const char* device_name)
{
	free(rdpsnd->device_name);
	rdpsnd->device_name = _strdup(device_name);
	return (rdpsnd->device_name != NULL);
}

COMMAND_LINE_ARGUMENT_A tirardpsnd_args[] =
{
	{ "sys", COMMAND_LINE_VALUE_REQUIRED, "<subsystem>", NULL, NULL, -1, NULL, "subsystem" },
	{ "dev", COMMAND_LINE_VALUE_REQUIRED, "<device>", NULL, NULL, -1, NULL, "device" },
	{ "format", COMMAND_LINE_VALUE_REQUIRED, "<format>", NULL, NULL, -1, NULL, "format" },
	{ "rate", COMMAND_LINE_VALUE_REQUIRED, "<rate>", NULL, NULL, -1, NULL, "rate" },
	{ "channel", COMMAND_LINE_VALUE_REQUIRED, "<channel>", NULL, NULL, -1, NULL, "channel" },
	{ "latency", COMMAND_LINE_VALUE_REQUIRED, "<latency>", NULL, NULL, -1, NULL, "latency" },
	{ "quality", COMMAND_LINE_VALUE_REQUIRED, "<quality mode>", NULL, NULL, -1, NULL, "quality mode" },
	{ NULL, 0, NULL, NULL, NULL, -1, NULL, NULL }
};

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_process_addin_args(rdpsndPlugin* rdpsnd, ADDIN_ARGV* args)
{
	int status;
	DWORD flags;
	COMMAND_LINE_ARGUMENT_A* arg;

	rdpsnd->wQualityMode = HIGH_QUALITY; /* default quality mode */


	if (args->argc > 1)
	{
		flags = COMMAND_LINE_SIGIL_NONE | COMMAND_LINE_SEPARATOR_COLON;
		status = CommandLineParseArgumentsA(args->argc, (const char **) args->argv,
					tirardpsnd_args, flags, rdpsnd, NULL, NULL);

		if (status < 0)
			return CHANNEL_RC_INITIALIZATION_ERROR;

		arg = tirardpsnd_args;

		do
		{
			if (!(arg->Flags & COMMAND_LINE_VALUE_PRESENT))
				continue;

			CommandLineSwitchStart(arg)

			CommandLineSwitchCase(arg, "sys")
			{
				if (!rdpsnd_set_subsystem(rdpsnd, arg->Value))
					return CHANNEL_RC_NO_MEMORY;
			}
			CommandLineSwitchCase(arg, "dev")
			{
				if (!rdpsnd_set_device_name(rdpsnd, arg->Value))
					return CHANNEL_RC_NO_MEMORY;
			}
			CommandLineSwitchCase(arg, "format")
			{
				rdpsnd->fixedFormat = atoi(arg->Value);
			}
			CommandLineSwitchCase(arg, "rate")
			{
				rdpsnd->fixedRate = atoi(arg->Value);
			}
			CommandLineSwitchCase(arg, "channel")
			{
				rdpsnd->fixedChannel = atoi(arg->Value);
			}
			CommandLineSwitchCase(arg, "latency")
			{
				rdpsnd->latency = atoi(arg->Value);
			}
			CommandLineSwitchCase(arg, "quality")
			{
				int wQualityMode = DYNAMIC_QUALITY;

				if (_stricmp(arg->Value, "dynamic") == 0)
					wQualityMode = DYNAMIC_QUALITY;
				else if (_stricmp(arg->Value, "medium") == 0)
					wQualityMode = MEDIUM_QUALITY;
				else if (_stricmp(arg->Value, "high") == 0)
					wQualityMode = HIGH_QUALITY;
				else
					wQualityMode = atoi(arg->Value);

				if ((wQualityMode < 0) || (wQualityMode > 2))
					wQualityMode = DYNAMIC_QUALITY;

				rdpsnd->wQualityMode = (UINT16) wQualityMode;
			}
			CommandLineSwitchDefault(arg)
			{

			}

			CommandLineSwitchEnd(arg)
		}
		while ((arg = CommandLineFindNextArgumentA(arg)) != NULL);
	}

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_process_connect(rdpsndPlugin* rdpsnd)
{
	ADDIN_ARGV* args;
	UINT status = ERROR_INTERNAL_ERROR;
	char *subsystem_name = NULL, *device_name = NULL;

	rdpsnd->latency = -1;

	args = (ADDIN_ARGV*) rdpsnd->channelEntryPoints.pExtendedData;
	if (args)
	{
		status = tirardpsnd_process_addin_args(rdpsnd, args);
		if (status != CHANNEL_RC_OK)
			return status;
	}

	rdpsnd->stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!rdpsnd->stopEvent)
	{
		WLog_ERR(TAG, "CreateEvent failed!");
		return CHANNEL_RC_INITIALIZATION_ERROR;
	}

	rdpsnd->ScheduleThread = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)tirardpsnd_schedule_thread,
		(void*) rdpsnd, 0, NULL);
	if (!rdpsnd->ScheduleThread)
	{
		WLog_ERR(TAG, "CreateThread failed!");
		return CHANNEL_RC_INITIALIZATION_ERROR;
	}

	return CHANNEL_RC_OK;
}

static void tirardpsnd_process_disconnect(rdpsndPlugin* rdpsnd)
{
	if (rdpsnd->ScheduleThread)
	{
		SetEvent(rdpsnd->stopEvent);
		if (WaitForSingleObject(rdpsnd->ScheduleThread, INFINITE) == WAIT_FAILED)
        {
            WLog_ERR(TAG, "WaitForSingleObject failed with error %lu!", GetLastError());
            return;
        }
		CloseHandle(rdpsnd->ScheduleThread);
		CloseHandle(rdpsnd->stopEvent);
	}
}

/****************************************************************************************/


static wListDictionary* g_InitHandles = NULL;
static wListDictionary* g_OpenHandles = NULL;

BOOL tirardpsnd_add_init_handle_data(void* pInitHandle, void* pUserData)
{
	if (!g_InitHandles)
	{
		g_InitHandles = ListDictionary_New(TRUE);
		if (!g_InitHandles)
			return FALSE;
	}

	return ListDictionary_Add(g_InitHandles, pInitHandle, pUserData);
}

void* tirardpsnd_get_init_handle_data(void* pInitHandle)
{
	void* pUserData = NULL;
	pUserData = ListDictionary_GetItemValue(g_InitHandles, pInitHandle);
	return pUserData;
}

void tirardpsnd_remove_init_handle_data(void* pInitHandle)
{
	ListDictionary_Remove(g_InitHandles, pInitHandle);
	if (ListDictionary_Count(g_InitHandles) < 1)
	{
		ListDictionary_Free(g_InitHandles);
		g_InitHandles = NULL;
	}
}

BOOL tirardpsnd_add_open_handle_data(DWORD openHandle, void* pUserData)
{
	void* pOpenHandle = (void*) (size_t) openHandle;

	if (!g_OpenHandles)
	{
		g_OpenHandles = ListDictionary_New(TRUE);
		if (!g_OpenHandles)
			return FALSE;
	}

	return ListDictionary_Add(g_OpenHandles, pOpenHandle, pUserData);
}

void* tirardpsnd_get_open_handle_data(DWORD openHandle)
{
	void* pUserData = NULL;
	void* pOpenHandle = (void*) (size_t) openHandle;
	pUserData = ListDictionary_GetItemValue(g_OpenHandles, pOpenHandle);
	return pUserData;
}

void tirardpsnd_remove_open_handle_data(DWORD openHandle)
{
	void* pOpenHandle = (void*) (size_t) openHandle;
	ListDictionary_Remove(g_OpenHandles, pOpenHandle);
	if (ListDictionary_Count(g_OpenHandles) < 1)
	{
		ListDictionary_Free(g_OpenHandles);
		g_OpenHandles = NULL;
	}
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
UINT tirasnd_virtual_channel_write(rdpsndPlugin* rdpsnd, wStream* s)
{
	UINT status;

	if (!rdpsnd)
	{
		status = CHANNEL_RC_BAD_INIT_HANDLE;
	}
	else
	{
		status = rdpsnd->channelEntryPoints.pVirtualChannelWrite(rdpsnd->OpenHandle,
			Stream_Buffer(s), (UINT32) Stream_GetPosition(s), s);
	}

	if (status != CHANNEL_RC_OK)
	{
		Stream_Free(s, TRUE);
		WLog_ERR(TAG,  "VirtualChannelWrite failed with %s [%08X]",
				 WTSErrorToString(status), status);
	}

	return status;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_virtual_channel_event_data_received(rdpsndPlugin* plugin,
		void* pData, UINT32 dataLength, UINT32 totalLength, UINT32 dataFlags)
{
	wStream* s;

	if ((dataFlags & CHANNEL_FLAG_SUSPEND) || (dataFlags & CHANNEL_FLAG_RESUME))
	{
		return CHANNEL_RC_OK;
	}

	if (dataFlags & CHANNEL_FLAG_FIRST)
	{
		if (plugin->data_in != NULL)
			Stream_Free(plugin->data_in, TRUE);

		plugin->data_in = Stream_New(NULL, totalLength);
		if (!plugin->data_in)
		{
			WLog_ERR(TAG,  "Stream_New failed!");
			return CHANNEL_RC_NO_MEMORY;
		}
	}

	s = plugin->data_in;

	if (!Stream_EnsureRemainingCapacity(s, (int) dataLength))
	{
		WLog_ERR(TAG,  "Stream_EnsureRemainingCapacity failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	Stream_Write(s, pData, dataLength);

	if (dataFlags & CHANNEL_FLAG_LAST)
	{
		if (Stream_Capacity(s) != Stream_GetPosition(s))
		{
			WLog_ERR(TAG,  "rdpsnd_virtual_channel_event_data_received: read error");
			return ERROR_INTERNAL_ERROR;
		}

		plugin->data_in = NULL;
		Stream_SealLength(s);
		Stream_SetPosition(s, 0);

		if (!MessageQueue_Post(plugin->MsgPipe->In, NULL, 0, (void*) s, NULL))
		{
			WLog_ERR(TAG,  "MessageQueue_Post failed!");
			return ERROR_INTERNAL_ERROR;
		}
	}
	return CHANNEL_RC_OK;
}

static VOID VCAPITYPE tirardpsnd_virtual_channel_open_event(DWORD openHandle, UINT event,
		LPVOID pData, UINT32 dataLength, UINT32 totalLength, UINT32 dataFlags)
{
	rdpsndPlugin* rdpsnd;
	UINT error = CHANNEL_RC_OK;

	rdpsnd = (rdpsndPlugin*)tirardpsnd_get_open_handle_data(openHandle);

	if (!rdpsnd)
	{
		WLog_ERR(TAG,  "rdpsnd_virtual_channel_open_event: error no match");
		return;
	}

	switch (event)
	{
		case CHANNEL_EVENT_DATA_RECEIVED:
			if ((error = tirardpsnd_virtual_channel_event_data_received(rdpsnd, pData, dataLength, totalLength, dataFlags)))
				WLog_ERR(TAG, "rdpsnd_virtual_channel_event_data_received failed with error %lu", error);
			break;

		case CHANNEL_EVENT_WRITE_COMPLETE:
			Stream_Free((wStream*) pData, TRUE);
			break;

		case CHANNEL_EVENT_USER:
			break;
	}
	if (error && rdpsnd->rdpcontext)
		setChannelError(rdpsnd->rdpcontext, error, "rdpsnd_virtual_channel_open_event reported an error");

}

static void* tirardpsnd_virtual_channel_client_thread(void* arg)
{
	wStream* data;
	wMessage message;
	rdpsndPlugin* rdpsnd = (rdpsndPlugin*) arg;
	UINT error;
	grdpsnd = rdpsnd;
	ginjectAudioFormatInfo = injectAudioFormatInfo;

	if ((error = tirardpsnd_process_connect(rdpsnd)))
	{
		WLog_ERR(TAG, "error connecting sound channel");
		goto out;
	}

	while (1)
	{
		if (!MessageQueue_Wait(rdpsnd->MsgPipe->In))
		{
			WLog_ERR(TAG, "MessageQueue_Wait failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (!MessageQueue_Peek(rdpsnd->MsgPipe->In, &message, TRUE))
		{
			WLog_ERR(TAG, "MessageQueue_Peek failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (message.id == WMQ_QUIT)
			break;

		if (message.id == 0)
		{
			data = (wStream*) message.wParam;
			gSendAudioplayData(data->pointer,data->length);
			if ((error = tirardpsnd_recv_pdu(rdpsnd, data)))
			{
				WLog_ERR(TAG, "error treating sound channel message");
				break;
			}
		}
	}

out:
	if (error && rdpsnd->rdpcontext)
		setChannelError(rdpsnd->rdpcontext, error, "rdpsnd_virtual_channel_client_thread reported an error");
	grdpsnd = NULL;
	ginjectAudioFormatInfo = NULL;
	tirardpsnd_process_disconnect(rdpsnd);

	ExitThread((DWORD)error);
	return NULL;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_virtual_channel_event_connected(rdpsndPlugin* plugin, LPVOID pData, UINT32 dataLength)
{
	UINT32 status;

	status = plugin->channelEntryPoints.pVirtualChannelOpen(plugin->InitHandle,
		&plugin->OpenHandle, plugin->channelDef.name, tirardpsnd_virtual_channel_open_event);

	if (status != CHANNEL_RC_OK)
	{
		WLog_ERR(TAG, "pVirtualChannelOpen failed with %s [%08X]",
				 WTSErrorToString(status), status);
		return status;
	}

	if (!tirardpsnd_add_open_handle_data(plugin->OpenHandle, plugin))
	{
		WLog_ERR(TAG, "unable to register opened handle");
		return  ERROR_INTERNAL_ERROR;
	}

	plugin->MsgPipe = MessagePipe_New();
	if (!plugin->MsgPipe)
	{
		WLog_ERR(TAG, "unable to create message pipe");
		return CHANNEL_RC_NO_MEMORY;
	}

	plugin->thread = CreateThread(NULL, 0,
		(LPTHREAD_START_ROUTINE)tirardpsnd_virtual_channel_client_thread, (void*)plugin, 0, NULL);
	if (!plugin->thread)
	{
		WLog_ERR(TAG, "unable to create thread");
		MessagePipe_Free(plugin->MsgPipe);
		plugin->MsgPipe = NULL;
		return ERROR_INTERNAL_ERROR;
	}

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpsnd_virtual_channel_event_disconnected(rdpsndPlugin* rdpsnd)
{
	UINT error;

	MessagePipe_PostQuit(rdpsnd->MsgPipe, 0);
	if (WaitForSingleObject(rdpsnd->thread, INFINITE) == WAIT_FAILED)
    {
        error = GetLastError();
        WLog_ERR(TAG, "WaitForSingleObject failed with error %lu!", error);
        return error;
    }

	CloseHandle(rdpsnd->thread);
	rdpsnd->thread = NULL;

	error = rdpsnd->channelEntryPoints.pVirtualChannelClose(rdpsnd->OpenHandle);
	if (CHANNEL_RC_OK != error)
	{
		WLog_ERR(TAG, "pVirtualChannelClose failed with %s [%08X]",
				 WTSErrorToString(error), error);
		return error;
	}

	if (rdpsnd->data_in)
	{
		Stream_Free(rdpsnd->data_in, TRUE);
		rdpsnd->data_in = NULL;
	}

	MessagePipe_Free(rdpsnd->MsgPipe);
	rdpsnd->MsgPipe = NULL;

	rdpsnd_free_audio_formats(rdpsnd->ClientFormats, rdpsnd->NumberOfClientFormats);
	rdpsnd->NumberOfClientFormats = 0;
	rdpsnd->ClientFormats = NULL;

    rdpsnd_free_audio_formats(rdpsnd->ServerFormats, rdpsnd->NumberOfServerFormats);
	rdpsnd->NumberOfServerFormats = 0;
	rdpsnd->ServerFormats = NULL;

	if (rdpsnd->device)
	{
		IFCALL(rdpsnd->device->Free, rdpsnd->device);
		rdpsnd->device = NULL;
	}

	if (rdpsnd->subsystem)
	{
		free(rdpsnd->subsystem);
		rdpsnd->subsystem = NULL;
	}

	if (rdpsnd->device_name)
	{
		free(rdpsnd->device_name);
		rdpsnd->device_name = NULL;
	}

	tirardpsnd_remove_open_handle_data(rdpsnd->OpenHandle);

	return CHANNEL_RC_OK;
}

static void tirardpsnd_virtual_channel_event_terminated(rdpsndPlugin* rdpsnd)
{
	tirardpsnd_remove_init_handle_data(rdpsnd->InitHandle);

	free(rdpsnd);
}

static VOID VCAPITYPE tirardpsnd_virtual_channel_init_event(LPVOID pInitHandle, UINT event, LPVOID pData, UINT dataLength)
{
	rdpsndPlugin* plugin;
	UINT error = CHANNEL_RC_OK;

	plugin = (rdpsndPlugin*) rdpsnd_get_init_handle_data(pInitHandle);

	if (!plugin)
	{
		WLog_ERR(TAG,  "rdpsnd_virtual_channel_init_event: error no match");
		return;
	}

	switch (event)
	{
		case CHANNEL_EVENT_CONNECTED:
			if ((error = tirardpsnd_virtual_channel_event_connected(plugin, pData, dataLength)))
				WLog_ERR(TAG, "rdpsnd_virtual_channel_event_connected failed with error %lu!", error);
			break;

		case CHANNEL_EVENT_DISCONNECTED:
			if ((error = tirardpsnd_virtual_channel_event_disconnected(plugin)))
				WLog_ERR(TAG, "rdpsnd_virtual_channel_event_disconnected failed with error %lu!", error);
			break;

		case CHANNEL_EVENT_TERMINATED:
			tirardpsnd_virtual_channel_event_terminated(plugin);
			break;
	}
	if (error && plugin->rdpcontext)
		setChannelError(plugin->rdpcontext, error, "rdpsnd_virtual_channel_init_event reported an error");
}

/* rdpsnd is always built-in */
#define VirtualChannelEntry	tirasnd_VirtualChannelEntry

BOOL VCAPITYPE VirtualChannelEntry(PCHANNEL_ENTRY_POINTS pEntryPoints)
{
	UINT rc;

	rdpsndPlugin* rdpsnd;
	CHANNEL_ENTRY_POINTS_FREERDP* pEntryPointsEx;


	rdpsnd = (rdpsndPlugin*) calloc(1, sizeof(rdpsndPlugin));
	if (!rdpsnd)
	{
		WLog_ERR(TAG, "calloc failed!");
		return FALSE;
	}

#if !defined(_WIN32) && !defined(ANDROID)
	{
		sigset_t mask;
		sigemptyset(&mask);
		sigaddset(&mask, SIGIO);
		pthread_sigmask(SIG_BLOCK, &mask, NULL);
	}
#endif

	rdpsnd->channelDef.options =
			CHANNEL_OPTION_INITIALIZED |
			CHANNEL_OPTION_ENCRYPT_RDP;

	strcpy(rdpsnd->channelDef.name, "rdpsnd");

	pEntryPointsEx = (CHANNEL_ENTRY_POINTS_FREERDP*) pEntryPoints;

	if ((pEntryPointsEx->cbSize >= sizeof(CHANNEL_ENTRY_POINTS_FREERDP)) &&
		(pEntryPointsEx->MagicNumber == FREERDP_CHANNEL_MAGIC_NUMBER))
	{
		rdpsnd->rdpcontext = pEntryPointsEx->context;
	}

	CopyMemory(&(rdpsnd->channelEntryPoints), pEntryPoints, sizeof(CHANNEL_ENTRY_POINTS_FREERDP));

	rdpsnd->log = WLog_Get("com.freerdp.channels.rdpsnd.client");

	rc = rdpsnd->channelEntryPoints.pVirtualChannelInit(&rdpsnd->InitHandle,
		&rdpsnd->channelDef, 1, VIRTUAL_CHANNEL_VERSION_WIN2000, tirardpsnd_virtual_channel_init_event);
	if (CHANNEL_RC_OK != rc)
	{
		WLog_ERR(TAG, "pVirtualChannelInit failed with %s [%08X]",
				 WTSErrorToString(rc), rc);
		free(rdpsnd);
		return FALSE;
	}

	return rdpsnd_add_init_handle_data(rdpsnd->InitHandle, (void*) rdpsnd);
}

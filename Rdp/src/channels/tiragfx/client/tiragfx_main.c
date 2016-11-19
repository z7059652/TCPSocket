/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Graphics Pipeline Extension
 *
 * Copyright 2013-2014 Marc-Andre Moreau <marcandre.moreau@gmail.com>
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

#include <assert.h>

#include <winpr/crt.h>
#include <winpr/wlog.h>
#include <winpr/print.h>
#include <winpr/synch.h>
#include <winpr/thread.h>
#include <winpr/stream.h>
#include <winpr/sysinfo.h>
#include <winpr/cmdline.h>
#include <winpr/collections.h>

#include <freerdp/addin.h>
#include <freerdp/channels/log.h>

#include "tiragfx_common.h"

#include "tiragfx_main.h"

#define TAG CHANNELS_TAG("tiragfx.client")
#define REDIRECTORFILE "redirectorfile"

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
struct redirectormessage
{
	BYTE* bData;
	unsigned int size;
};
typedef struct redirectormessage REDIRECTORMESSAGE;

_declspec(dllexport) void(*gSendScreenData)(void *data, unsigned int size);

static UINT tirardpgfx_send_caps_advertise_pdu(RDPGFX_CHANNEL_CALLBACK* callback)
{
	UINT error;
	wStream* s;
	UINT16 index;
	RDPGFX_PLUGIN* gfx;
	RDPGFX_HEADER header;
	RDPGFX_CAPSET* capsSet;
	RDPGFX_CAPSET capsSets[2];
	RDPGFX_CAPS_ADVERTISE_PDU pdu;

	gfx = (RDPGFX_PLUGIN*) callback->plugin;

	header.flags = 0;
	header.cmdId = RDPGFX_CMDID_CAPSADVERTISE;

	pdu.capsSetCount = 0;
	pdu.capsSets = (RDPGFX_CAPSET*) capsSets;

	capsSet = &capsSets[pdu.capsSetCount++];
	capsSet->version = RDPGFX_CAPVERSION_8;
	capsSet->flags = 0;

	if (gfx->ThinClient)
		capsSet->flags |= RDPGFX_CAPS_FLAG_THINCLIENT;

	if (gfx->SmallCache)
		capsSet->flags |= RDPGFX_CAPS_FLAG_SMALL_CACHE;

	capsSet = &capsSets[pdu.capsSetCount++];
	capsSet->version = RDPGFX_CAPVERSION_81;
	capsSet->flags = 0;

	if (gfx->ThinClient)
		capsSet->flags |= RDPGFX_CAPS_FLAG_THINCLIENT;

	if (gfx->SmallCache)
		capsSet->flags |= RDPGFX_CAPS_FLAG_SMALL_CACHE;

	if (gfx->H264)
		capsSet->flags |= RDPGFX_CAPS_FLAG_H264ENABLED;

	header.pduLength = RDPGFX_HEADER_SIZE + 2 + (pdu.capsSetCount * RDPGFX_CAPSET_SIZE);

	WLog_DBG(TAG, "SendCapsAdvertisePdu");

	s = Stream_New(NULL, header.pduLength);
	if (!s)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	if ((error = rdpgfx_write_header(s, &header)))
	{
		WLog_ERR(TAG, "rdpgfx_write_header failed with error %lu!", error);
		return error;
	}

	/* RDPGFX_CAPS_ADVERTISE_PDU */

	Stream_Write_UINT16(s, pdu.capsSetCount); /* capsSetCount (2 bytes) */

	for (index = 0; index < pdu.capsSetCount; index++)
	{
		capsSet = &(pdu.capsSets[index]);
		Stream_Write_UINT32(s, capsSet->version); /* version (4 bytes) */
		Stream_Write_UINT32(s, 4); /* capsDataLength (4 bytes) */
		Stream_Write_UINT32(s, capsSet->flags); /* capsData (4 bytes) */
	}

	Stream_SealLength(s);

	error = callback->channel->Write(callback->channel, (UINT32) Stream_Length(s), Stream_Buffer(s), NULL);

	Stream_Free(s, TRUE);

	return error;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_send_frame_acknowledge_pdu(RDPGFX_CHANNEL_CALLBACK* callback, RDPGFX_FRAME_ACKNOWLEDGE_PDU* pdu)
{
	UINT error;
	wStream* s;
	RDPGFX_HEADER header;

	header.flags = 0;
	header.cmdId = RDPGFX_CMDID_FRAMEACKNOWLEDGE;
	header.pduLength = RDPGFX_HEADER_SIZE + 12;

	WLog_DBG(TAG, "SendFrameAcknowledgePdu: %d", pdu->frameId);

	s = Stream_New(NULL, header.pduLength);
	if (!s)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	if ((error = rdpgfx_write_header(s, &header)))
	{
		WLog_ERR(TAG, "rdpgfx_write_header failed with error %lu!", error);
		return error;
	}

	/* RDPGFX_FRAME_ACKNOWLEDGE_PDU */

	Stream_Write_UINT32(s, pdu->queueDepth); /* queueDepth (4 bytes) */
	Stream_Write_UINT32(s, pdu->frameId); /* frameId (4 bytes) */
	Stream_Write_UINT32(s, pdu->totalFramesDecoded); /* totalFramesDecoded (4 bytes) */

	error = callback->channel->Write(callback->channel, (UINT32) Stream_Length(s), Stream_Buffer(s), NULL);

	Stream_Free(s, TRUE);

	return error;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_recv_end_frame_pdu(RDPGFX_CHANNEL_CALLBACK* callback, wStream* s)
{
	RDPGFX_END_FRAME_PDU pdu;
	RDPGFX_FRAME_ACKNOWLEDGE_PDU ack;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) callback->plugin;
	UINT error = CHANNEL_RC_OK;

	if (Stream_GetRemainingLength(s) < 4)
	{
		WLog_ERR(TAG, "not enought data!");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT32(s, pdu.frameId); /* frameId (4 bytes) */

	WLog_DBG(TAG, "RecvEndFramePdu: frameId: %d", pdu.frameId);

	gfx->UnacknowledgedFrames--;
	gfx->TotalDecodedFrames++;

	ack.frameId = pdu.frameId;
	ack.totalFramesDecoded = gfx->TotalDecodedFrames;

	if (gfx->suspendFrameAcks)
	{
		ack.queueDepth = SUSPEND_FRAME_ACKNOWLEDGEMENT;

		if (gfx->TotalDecodedFrames == 1)
		if ((error = tirardpgfx_send_frame_acknowledge_pdu(callback, &ack)))
			WLog_ERR(TAG, "rdpgfx_send_frame_acknowledge_pdu failed with error %lu", error);
	}
	else
	{
		ack.queueDepth = QUEUE_DEPTH_UNAVAILABLE;
		if ((error = tirardpgfx_send_frame_acknowledge_pdu(callback, &ack)))
			WLog_ERR(TAG, "rdpgfx_send_frame_acknowledge_pdu failed with error %lu", error);
	}

	return error;
}
/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_recv_pdu(RDPGFX_CHANNEL_CALLBACK* callback, wStream* s)
{
	int beg, end;
	RDPGFX_HEADER header;
	UINT error;

	beg = Stream_GetPosition(s);

	if ((error = tirardpgfx_read_header(s, &header)))
	{
		WLog_ERR(TAG, "rdpgfx_read_header failed with error %lu!", error);
		return error;
	}

#if 1
	WLog_DBG(TAG, "cmdId: %s (0x%04X) flags: 0x%04X pduLength: %d",
		  tirardpgfx_get_cmd_id_string(header.cmdId), header.cmdId, header.flags, header.pduLength);
#endif

	switch (header.cmdId)
	{
		case RDPGFX_CMDID_WIRETOSURFACE_1:			
			break;

		case RDPGFX_CMDID_WIRETOSURFACE_2:
			break;

		case RDPGFX_CMDID_DELETEENCODINGCONTEXT:
			break;

		case RDPGFX_CMDID_SOLIDFILL:
			break;

		case RDPGFX_CMDID_SURFACETOSURFACE:
			break;

		case RDPGFX_CMDID_SURFACETOCACHE:
			break;

		case RDPGFX_CMDID_CACHETOSURFACE:
			break;

		case RDPGFX_CMDID_EVICTCACHEENTRY:
			break;

		case RDPGFX_CMDID_CREATESURFACE:
			break;

		case RDPGFX_CMDID_DELETESURFACE:
			break;

		case RDPGFX_CMDID_STARTFRAME:
			break;

		case RDPGFX_CMDID_ENDFRAME:
			if ((error = tirardpgfx_recv_end_frame_pdu(callback, s)))
				WLog_ERR(TAG, "rdpgfx_recv_end_frame_pdu failed with error %lu!", error);
			break;

		case RDPGFX_CMDID_RESETGRAPHICS:
			break;

		case RDPGFX_CMDID_MAPSURFACETOOUTPUT:
			break;

		case RDPGFX_CMDID_CACHEIMPORTREPLY:
			break;

		case RDPGFX_CMDID_CAPSCONFIRM:
			break;

		case RDPGFX_CMDID_MAPSURFACETOWINDOW:
			break;

		default:
			error = CHANNEL_RC_BAD_PROC;
			break;
	}

	if (error)
	{
		WLog_ERR(TAG,  "Error while parsing GFX cmdId: %s (0x%04X)",
			 tirardpgfx_get_cmd_id_string(header.cmdId), header.cmdId);
		return error;
	}

	end = Stream_GetPosition(s);

	if (end != (beg + header.pduLength))
		Stream_SetPosition(s, (beg + header.pduLength));

	return error;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */


static UINT tirardpgfx_on_data_received(IWTSVirtualChannelCallback* pChannelCallback, wStream* data)
{
	wStream* s;
	int status = 0;
	UINT32 DstSize = 0;
	BYTE* pDstData = NULL;
	RDPGFX_CHANNEL_CALLBACK* callback = (RDPGFX_CHANNEL_CALLBACK*) pChannelCallback;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) callback->plugin;
	UINT error = CHANNEL_RC_OK;

	gSendScreenData(Stream_Pointer(data), Stream_GetRemainingLength(data));

	status = zgfx_decompress(gfx->zgfx, Stream_Pointer(data), Stream_GetRemainingLength(data), &pDstData, &DstSize, 0);
	if (status < 0)
	{
		WLog_ERR(TAG, "zgfx_decompress failure! status: %d", status);
		return ERROR_INTERNAL_ERROR;
	}

	s = Stream_New(pDstData, DstSize);
	//////////////zhuxn//////////

	
	if (!s)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	while (((size_t) Stream_GetPosition(s)) < Stream_Length(s))
	{
		if ((error = tirardpgfx_recv_pdu(callback, s)))
		{
			WLog_ERR(TAG, "rdpgfx_recv_pdu failed with error %lu!", error);
			break;
		}
	}

	Stream_Free(s, TRUE);
	return error;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_on_open(IWTSVirtualChannelCallback* pChannelCallback)
{
	RDPGFX_CHANNEL_CALLBACK* callback = (RDPGFX_CHANNEL_CALLBACK*) pChannelCallback;

	WLog_DBG(TAG, "OnOpen");

	return tirardpgfx_send_caps_advertise_pdu(callback);
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_on_close(IWTSVirtualChannelCallback* pChannelCallback)
{
	int count;
	int index;
	ULONG_PTR* pKeys = NULL;
	RDPGFX_CHANNEL_CALLBACK* callback = (RDPGFX_CHANNEL_CALLBACK*) pChannelCallback;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) callback->plugin;
	RdpgfxClientContext* context = (RdpgfxClientContext*) gfx->iface.pInterface;

	WLog_DBG(TAG, "OnClose");

	free(callback);

	gfx->UnacknowledgedFrames = 0;
	gfx->TotalDecodedFrames = 0;

	if (gfx->zgfx)
	{
		zgfx_context_free(gfx->zgfx);
		gfx->zgfx = zgfx_context_new(FALSE);

		if (!gfx->zgfx)
			return CHANNEL_RC_NO_MEMORY;
	}

	count = HashTable_GetKeys(gfx->SurfaceTable, &pKeys);

	for (index = 0; index < count; index++)
	{
		RDPGFX_DELETE_SURFACE_PDU pdu;

		pdu.surfaceId = ((UINT16) pKeys[index]) - 1;

		if (context && context->DeleteSurface)
		{
			context->DeleteSurface(context, &pdu);
		}
	}

	free(pKeys);

	for (index = 0; index < gfx->MaxCacheSlot; index++)
	{
		if (gfx->CacheSlots[index])
		{
			RDPGFX_EVICT_CACHE_ENTRY_PDU pdu;

			pdu.cacheSlot = (UINT16) index;

			if (context && context->EvictCacheEntry)
			{
				context->EvictCacheEntry(context, &pdu);
			}

			gfx->CacheSlots[index] = NULL;
		}
	}

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_on_new_channel_connection(IWTSListenerCallback* pListenerCallback,
	IWTSVirtualChannel* pChannel, BYTE* Data, int* pbAccept,
	IWTSVirtualChannelCallback** ppCallback)
{
	RDPGFX_CHANNEL_CALLBACK* callback;
	RDPGFX_LISTENER_CALLBACK* listener_callback = (RDPGFX_LISTENER_CALLBACK*) pListenerCallback;

	callback = (RDPGFX_CHANNEL_CALLBACK*) calloc(1, sizeof(RDPGFX_CHANNEL_CALLBACK));

	if (!callback)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	callback->iface.OnDataReceived = tirardpgfx_on_data_received;
	callback->iface.OnOpen = tirardpgfx_on_open;
	callback->iface.OnClose = tirardpgfx_on_close;
	callback->plugin = listener_callback->plugin;
	callback->channel_mgr = listener_callback->channel_mgr;
	callback->channel = pChannel;
	listener_callback->channel_callback = callback;

	*ppCallback = (IWTSVirtualChannelCallback*) callback;

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_plugin_initialize(IWTSPlugin* pPlugin, IWTSVirtualChannelManager* pChannelMgr)
{
	UINT error;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) pPlugin;

	gfx->listener_callback = (RDPGFX_LISTENER_CALLBACK*) calloc(1, sizeof(RDPGFX_LISTENER_CALLBACK));

	if (!gfx->listener_callback)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	gfx->listener_callback->iface.OnNewChannelConnection = tirardpgfx_on_new_channel_connection;
	gfx->listener_callback->plugin = pPlugin;
	gfx->listener_callback->channel_mgr = pChannelMgr;

	error = pChannelMgr->CreateListener(pChannelMgr, RDPGFX_DVC_CHANNEL_NAME, 0,
		(IWTSListenerCallback*) gfx->listener_callback, &(gfx->listener));

	gfx->listener->pInterface = gfx->iface.pInterface;

	WLog_DBG(TAG, "Initialize");

	return error;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_plugin_terminated(IWTSPlugin* pPlugin)
{
	int count;
	int index;
	ULONG_PTR* pKeys = NULL;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) pPlugin;
	RdpgfxClientContext* context = (RdpgfxClientContext*) gfx->iface.pInterface;
	UINT error = CHANNEL_RC_OK;

	WLog_DBG(TAG, "Terminated");

	if (gfx->listener_callback)
	{
		free(gfx->listener_callback);
		gfx->listener_callback = NULL;
	}

	if (gfx->zgfx)
	{
		zgfx_context_free(gfx->zgfx);
		gfx->zgfx = NULL;
	}

	count = HashTable_GetKeys(gfx->SurfaceTable, &pKeys);

	for (index = 0; index < count; index++)
	{
		RDPGFX_DELETE_SURFACE_PDU pdu;

		pdu.surfaceId = ((UINT16) pKeys[index]) - 1;

		if (context)
		{
			IFCALLRET(context->DeleteSurface, error, context, &pdu);
			if (error)
			{
				WLog_ERR(TAG, "context->DeleteSurface failed with error %lu", error);
				free(pKeys);
				free(context);
				free(gfx);
				return error;
			}
		}
	}

	free(pKeys);

	HashTable_Free(gfx->SurfaceTable);

	for (index = 0; index < gfx->MaxCacheSlot; index++)
	{
		if (gfx->CacheSlots[index])
		{
			RDPGFX_EVICT_CACHE_ENTRY_PDU pdu;

			pdu.cacheSlot = (UINT16) index;

			if (context)
			{
				IFCALLRET(context->EvictCacheEntry, error, context, &pdu);
				if (error)
				{
					WLog_ERR(TAG, "context->EvictCacheEntry failed with error %lu", error);
					free(context);
					free(gfx);
					return error;
				}
			}

			gfx->CacheSlots[index] = NULL;
		}
	}

	free(context);

	free(gfx);

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_set_surface_data(RdpgfxClientContext* context, UINT16 surfaceId, void* pData)
{
	ULONG_PTR key;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) context->handle;

	key = ((ULONG_PTR) surfaceId) + 1;

	if (pData)
		HashTable_Add(gfx->SurfaceTable, (void*) key, pData);
	else
		HashTable_Remove(gfx->SurfaceTable, (void*) key);

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_get_surface_ids(RdpgfxClientContext* context, UINT16** ppSurfaceIds, UINT16* count_out)
{
	int count;
	int index;
	UINT16* pSurfaceIds;
	ULONG_PTR* pKeys = NULL;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) context->handle;

	count = HashTable_GetKeys(gfx->SurfaceTable, &pKeys);

	if (count < 1)
	{
		*count_out = 0;
		return CHANNEL_RC_OK;
	}

	pSurfaceIds = (UINT16*) malloc(count * sizeof(UINT16));

	if (!pSurfaceIds)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	for (index = 0; index < count; index++)
	{
		pSurfaceIds[index] = pKeys[index] - 1;
	}

	free(pKeys);
	*ppSurfaceIds = pSurfaceIds;
	*count_out = (UINT16)count;

	return CHANNEL_RC_OK;
}

static void* tirardpgfx_get_surface_data(RdpgfxClientContext* context, UINT16 surfaceId)
{
	ULONG_PTR key;
	void* pData = NULL;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) context->handle;

	key = ((ULONG_PTR) surfaceId) + 1;

	pData = HashTable_GetItemValue(gfx->SurfaceTable, (void*) key);

	return pData;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT tirardpgfx_set_cache_slot_data(RdpgfxClientContext* context, UINT16 cacheSlot, void* pData)
{
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) context->handle;

	if (cacheSlot >= gfx->MaxCacheSlot)
		return ERROR_INVALID_INDEX;

	gfx->CacheSlots[cacheSlot] = pData;

	return CHANNEL_RC_OK;
}

void* tirardpgfx_get_cache_slot_data(RdpgfxClientContext* context, UINT16 cacheSlot)
{
	void* pData = NULL;
	RDPGFX_PLUGIN* gfx = (RDPGFX_PLUGIN*) context->handle;

	if (cacheSlot >= gfx->MaxCacheSlot)
		return NULL;

	pData = gfx->CacheSlots[cacheSlot];

	return pData;
}


#define TITADVCPluginEntry		tiragfx_DVCPluginEntry


/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
UINT TITADVCPluginEntry(IDRDYNVC_ENTRY_POINTS* pEntryPoints)
{
	UINT error = CHANNEL_RC_OK;
	RDPGFX_PLUGIN* gfx;
	RdpgfxClientContext* context;

	gfx = (RDPGFX_PLUGIN*) pEntryPoints->GetPlugin(pEntryPoints, "rdpgfx");
	if (!gfx)
	{
		gfx = (RDPGFX_PLUGIN*) calloc(1, sizeof(RDPGFX_PLUGIN));

		if (!gfx)
		{
			WLog_ERR(TAG, "calloc failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		gfx->settings = (rdpSettings*) pEntryPoints->GetRdpSettings(pEntryPoints);

		gfx->iface.Initialize = tirardpgfx_plugin_initialize;
		gfx->iface.Connected = NULL;
		gfx->iface.Disconnected = NULL;
		gfx->iface.Terminated = tirardpgfx_plugin_terminated;

		gfx->SurfaceTable = HashTable_New(TRUE);

		if (!gfx->SurfaceTable)
		{
			free (gfx);
			WLog_ERR(TAG, "HashTable_New failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		gfx->ThinClient = gfx->settings->GfxThinClient;
		gfx->SmallCache = gfx->settings->GfxSmallCache;
		gfx->Progressive = gfx->settings->GfxProgressive;
		gfx->ProgressiveV2 = gfx->settings->GfxProgressiveV2;
		gfx->H264 = gfx->settings->GfxH264;

		if (gfx->H264)
			gfx->SmallCache = TRUE;

		if (gfx->SmallCache)
			gfx->ThinClient = FALSE;

		gfx->MaxCacheSlot = (gfx->ThinClient) ? 4096 : 25600;


		context = (RdpgfxClientContext*) calloc(1, sizeof(RdpgfxClientContext));

		if (!context)
		{
			free(gfx);
			WLog_ERR(TAG, "calloc failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		context->handle = (void*) gfx;

		context->GetSurfaceIds = tirardpgfx_get_surface_ids;
		context->SetSurfaceData = tirardpgfx_set_surface_data;
		context->GetSurfaceData =  tirardpgfx_get_surface_data;
		context->SetCacheSlotData = tirardpgfx_set_cache_slot_data;
		context->GetCacheSlotData = tirardpgfx_get_cache_slot_data;

		gfx->iface.pInterface = (void*) context;

		gfx->zgfx = zgfx_context_new(FALSE);

		if (!gfx->zgfx)
		{
			free(gfx);
			free(context);
			WLog_ERR(TAG, "zgfx_context_new failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		error = pEntryPoints->RegisterPlugin(pEntryPoints, "rdpgfx", (IWTSPlugin*) gfx);
	}

	return error;
}

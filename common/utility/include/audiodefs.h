#pragma once
#include <memory>
#include "target_os.h"

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 0x0001
#endif

namespace Titanium { namespace TIRA
{
#pragma pack(push, 1)
	class TI_AUDIO_FORMAT
	{
	public:
		WORD    wFormatTag;        /* format type */
		WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
		DWORD   nSamplesPerSec;    /* sample rate */
		DWORD   nAvgBytesPerSec;   /* for buffer estimation */
		WORD    nBlockAlign;       /* block size of data */
		WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
		WORD    cbSize;            /* The count in bytes of the size of extra information (after cbSize) */
		bool operator == (const TI_AUDIO_FORMAT& format) const
		{
			return
				wFormatTag == format.wFormatTag &&
				nChannels == format.nChannels &&
				nSamplesPerSec == format.nSamplesPerSec &&
				nAvgBytesPerSec == format.nAvgBytesPerSec &&
				nBlockAlign == format.nBlockAlign &&
				wBitsPerSample == format.wBitsPerSample &&
				cbSize == format.cbSize;
		}
	};
#pragma pack(pop)
}
}
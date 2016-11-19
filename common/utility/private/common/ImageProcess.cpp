#include "ImageProcess.h"

#ifndef _WIN64
//Conditional move opcode macros
#define cmoves  _asm _emit 0x0F  _asm _emit 0x48
#define cmovz   _asm _emit 0x0F  _asm _emit 0x44
#define cmove   _asm _emit 0x0F  _asm _emit 0x44
#define cmovne  _asm _emit 0x0F  _asm _emit 0x45
#define cmovnz  _asm _emit 0x0F  _asm _emit 0x45
#define cmovge  _asm _emit 0x0F  _asm _emit 0x4D
#define cmovg   _asm _emit 0x0F  _asm _emit 0x4F

//Registers encoding opcode macros
#define edx_edi _asm _emit 0xD7
#define edx_eax _asm _emit 0xD0
#define ebx_eax _asm _emit 0xD8
#define edi_eax _asm _emit 0xF8
#define esi_eax _asm _emit 0xF0
#define eax_edi _asm _emit 0xC7
#define eax_edx _asm _emit 0xC2



//Katmai new instructions opcodes
#define pavgb       _asm _emit 0x0F _asm _emit 0xE0  //packed byte avarage, takes 2 mmx registers  
#define psadbw      _asm _emit 0x0F _asm _emit 0xF6  //packed byte to word sun absolute diffrences, takes 2 mmx registers
#define pshufw      _asm _emit 0x0F _asm _emit 0x70  //pshufw takes 2 mmx registers and 1 byte which must be emited  
#define pmulhuw     _asm _emit 0x0F _asm _emit 0xE4  //pmulhuw packed unsigned multiply keep high words takes 2 mmx registers
#define pinsrw      _asm _emit 0x0F _asm _emit 0xC4
#define pmovmskb    _asm _emit 0x0F _asm _emit 0xD7  
#define pminub      _asm _emit 0x0F _asm _emit 0xDA
#define pmaxub      _asm _emit 0x0F _asm _emit 0xDE
#define pminsw      _asm _emit 0x0F _asm _emit 0xEA
#define pextrw      _asm _emit 0x0F _asm _emit 0xC5
#define pshufw      _asm _emit 0x0F _asm _emit 0x70
#define prefetchnta _asm _emit 0x0F _asm _emit 0x18 _asm _emit 0x44
#define prefetcht0  _asm _emit 0x0F _asm _emit 0x18 _asm _emit 0x4C
#define prefetcht1  _asm _emit 0x0F _asm _emit 0x18 _asm _emit 0x54
#define prefetcht2  _asm _emit 0x0F _asm _emit 0x18 _asm _emit 0x5C

//Katmai SIMD FP instruction opcodes
#define movaps  _asm _emit 0x0F _asm _emit 0x28  //128 bit aligned move

//defines opcodes for mm?,[esi+offset],imm to use with pinsrw
#define mm0_esi(offset,imm) _asm _emit 0x46 _asm _emit offset _asm _emit imm
#define mm1_esi(offset,imm) _asm _emit 0x4E _asm _emit offset _asm _emit imm
#define mm2_esi(offset,imm) _asm _emit 0x56 _asm _emit offset _asm _emit imm

//For mmx,eax,offset to use with pinsrw
#define mm0_eax_(offset) _asm _emit 0xC0 _asm _emit offset
#define mm1_eax_(offset) _asm _emit 0xC8 _asm _emit offset
#define mm2_eax_(offset) _asm _emit 0xD0 _asm _emit offset
#define mm3_eax_(offset) _asm _emit 0xD8 _asm _emit offset
#define mm4_eax_(offset) _asm _emit 0xE0 _asm _emit offset
#define mm5_eax_(offset) _asm _emit 0xE8 _asm _emit offset
#define mm6_eax_(offset) _asm _emit 0xF0 _asm _emit offset
#define mm7_eax_(offset) _asm _emit 0xF8 _asm _emit offset

//For mmx,ebx,offset to use with pinsrw
#define mm0_ebx_(offset) _asm _emit 0xC3 _asm _emit offset
#define mm1_ebx_(offset) _asm _emit 0xCB _asm _emit offset
#define mm2_ebx_(offset) _asm _emit 0xD3 _asm _emit offset
#define mm3_ebx_(offset) _asm _emit 0xDB _asm _emit offset
#define mm4_ebx_(offset) _asm _emit 0xE3 _asm _emit offset
#define mm5_ebx_(offset) _asm _emit 0xEB _asm _emit offset
#define mm6_ebx_(offset) _asm _emit 0xF3 _asm _emit offset
#define mm7_ebx_(offset) _asm _emit 0xFB _asm _emit offset

//For mmx,ecx,offset to use with pinsrw
#define mm0_ecx_(offset) _asm _emit 0xC1 _asm _emit offset
#define mm1_ecx_(offset) _asm _emit 0xC9 _asm _emit offset
#define mm2_ecx_(offset) _asm _emit 0xD1 _asm _emit offset
#define mm3_ecx_(offset) _asm _emit 0xD9 _asm _emit offset
#define mm4_ecx_(offset) _asm _emit 0xE1 _asm _emit offset
#define mm5_ecx_(offset) _asm _emit 0xE9 _asm _emit offset
#define mm6_ecx_(offset) _asm _emit 0xF1 _asm _emit offset
#define mm7_ecx_(offset) _asm _emit 0xF9 _asm _emit offset

//For eax,mmx,offset for use with pextrw
#define eax_mm0_(offset) _asm _emit 0xC0 _asm _emit offset
#define eax_mm1_(offset) _asm _emit 0xC1 _asm _emit offset
#define eax_mm2_(offset) _asm _emit 0xC2 _asm _emit offset
#define eax_mm3_(offset) _asm _emit 0xC3 _asm _emit offset
#define eax_mm4_(offset) _asm _emit 0xC4 _asm _emit offset
#define eax_mm5_(offset) _asm _emit 0xC5 _asm _emit offset
#define eax_mm6_(offset) _asm _emit 0xC6 _asm _emit offset
#define eax_mm7_(offset) _asm _emit 0xC7 _asm _emit offset

//For ebx,mmx,offset for use with pextrw
#define ebx_mm0_(offset) _asm _emit 0xD8 _asm _emit offset
#define ebx_mm1_(offset) _asm _emit 0xD9 _asm _emit offset
#define ebx_mm2_(offset) _asm _emit 0xDA _asm _emit offset
#define ebx_mm3_(offset) _asm _emit 0xDB _asm _emit offset
#define ebx_mm4_(offset) _asm _emit 0xDC _asm _emit offset
#define ebx_mm5_(offset) _asm _emit 0xDD _asm _emit offset
#define ebx_mm6_(offset) _asm _emit 0xDE _asm _emit offset
#define ebx_mm7_(offset) _asm _emit 0xDF _asm _emit offset

//For ecx,mmx,offset for use with pextrw
#define ecx_mm0_(offset) _asm _emit 0xC8 _asm _emit offset
#define ecx_mm1_(offset) _asm _emit 0xC9 _asm _emit offset
#define ecx_mm2_(offset) _asm _emit 0xCA _asm _emit offset
#define ecx_mm3_(offset) _asm _emit 0xCB _asm _emit offset
#define ecx_mm4_(offset) _asm _emit 0xCC _asm _emit offset
#define ecx_mm5_(offset) _asm _emit 0xCD _asm _emit offset
#define ecx_mm6_(offset) _asm _emit 0xCE _asm _emit offset
#define ecx_mm7_(offset) _asm _emit 0xCF _asm _emit offset

//For edx,mmx,offset for use with pextrw
#define edx_mm0_(offset) _asm _emit 0xD0 _asm _emit offset
#define edx_mm1_(offset) _asm _emit 0xD1 _asm _emit offset
#define edx_mm2_(offset) _asm _emit 0xD2 _asm _emit offset
#define edx_mm3_(offset) _asm _emit 0xD3 _asm _emit offset
#define edx_mm4_(offset) _asm _emit 0xD4 _asm _emit offset
#define edx_mm5_(offset) _asm _emit 0xD5 _asm _emit offset
#define edx_mm6_(offset) _asm _emit 0xD6 _asm _emit offset
#define edx_mm7_(offset) _asm _emit 0xD7 _asm _emit offset

//For mmx,[edi]
#define mm0_edi _asm _emit 0x07
#define mm1_edi _asm _emit 0x0F
#define mm2_edi _asm _emit 0x17
#define mm3_edi _asm _emit 0x1F
#define mm4_edi _asm _emit 0x27
#define mm6_edi _asm _emit 0x37 
#define mm7_edi _asm _emit 0x3F

//For mmx,[esi+edx+offset]
#define mm1_esi_edx_(offset) _asm emit 0x4C _asm _emit 0x16 _asm _emit offset 
#define mm7_esi_edx_(offset) _asm emit 0x7C _asm _emit 0x16 _asm _emit offset 

//For mmx,[esi+offset]
#define mm1_esi_(offset)    _asm _emit 0x4E _asm _emit offset

//For mmx,[edi+offset]
#define mm5_edi_(offset)    _asm _emit 0x6F _asm _emit offset
#define mm6_edi_(offset)    _asm _emit 0x77 _asm _emit offset
#define mm7_edi_(offset)    _asm _emit 0x7F _asm _emit offset

//for cmov
#define eax_ebx             _asm _emit 0xC3
#define ecx_ebx             _asm _emit 0xCB
#define edx_ebx             _asm _emit 0xD3

//for eax,mm4
#define eax_mm4             _asm _emit 0xC4
#define eax_mm7             _asm _emit 0xC7

// for prefetch
#define esi_eax_(scale,offset)  _asm _emit 0x6|scale<<6 _asm _emit offset

//MMX redister encoding opcode macros
#define mm0_mm0 _asm _emit 0xC0
#define mm0_mm1 _asm _emit 0xC1
#define mm0_mm2 _asm _emit 0xC2
#define mm0_mm3 _asm _emit 0xC3
#define mm0_mm4 _asm _emit 0xC4
#define mm0_mm5 _asm _emit 0xC5
#define mm0_mm6 _asm _emit 0xC6
#define mm0_mm7 _asm _emit 0xC7
#define mm1_mm0 _asm _emit 0xC8
#define mm1_mm1 _asm _emit 0xC9
#define mm1_mm2 _asm _emit 0xCA
#define mm1_mm3 _asm _emit 0xCB
#define mm1_mm4 _asm _emit 0xCC
#define mm1_mm5 _asm _emit 0xCD
#define mm1_mm6 _asm _emit 0xCE
#define mm1_mm7 _asm _emit 0xCF
#define mm2_mm0 _asm _emit 0xD0
#define mm2_mm1 _asm _emit 0xD1
#define mm2_mm2 _asm _emit 0xD2
#define mm2_mm3 _asm _emit 0xD3
#define mm2_mm4 _asm _emit 0xD4
#define mm2_mm5 _asm _emit 0xD5
#define mm2_mm6 _asm _emit 0xD6
#define mm2_mm7 _asm _emit 0xD7
#define mm3_mm0 _asm _emit 0xD8
#define mm3_mm1 _asm _emit 0xD9
#define mm3_mm2 _asm _emit 0xDA
#define mm3_mm3 _asm _emit 0xDB
#define mm3_mm4 _asm _emit 0xDC
#define mm3_mm5 _asm _emit 0xDD
#define mm3_mm7 _asm _emit 0xDF
#define mm4_mm0 _asm _emit 0xE0
#define mm4_mm1 _asm _emit 0xE1
#define mm4_mm2 _asm _emit 0xE2
#define mm4_mm5 _asm _emit 0xE5
#define mm4_mm6 _asm _emit 0xE6
#define mm4_mm7 _asm _emit 0xE7
#define mm5_mm0 _asm _emit 0xE8
#define mm5_mm1 _asm _emit 0xE9
#define mm5_mm2 _asm _emit 0xEA
#define mm5_mm3 _asm _emit 0xEB
#define mm5_mm4 _asm _emit 0xEC
#define mm5_mm5 _asm _emit 0xED
#define mm5_mm6 _asm _emit 0xEE
#define mm5_mm7 _asm _emit 0xEF
#define mm6_mm0 _asm _emit 0xF0
#define mm6_mm1 _asm _emit 0xF1
#define mm6_mm2 _asm _emit 0xF2
#define mm6_mm3 _asm _emit 0xF3
#define mm6_mm4 _asm _emit 0xF4
#define mm6_mm5 _asm _emit 0xF5
#define mm6_mm6 _asm _emit 0xF6
#define mm6_mm7 _asm _emit 0xF7
#define mm7_mm0 _asm _emit 0xF8
#define mm7_mm1 _asm _emit 0xF9
#define mm7_mm2 _asm _emit 0xFA
#define mm7_mm3 _asm _emit 0xFB
#define mm7_mm4 _asm _emit 0xFC
#define mm7_mm5 _asm _emit 0xFD
#define mm7_mm6 _asm _emit 0xFE
#define mm7_mm7 _asm _emit 0xFF

#define emit _asm _emit

#define CPU_ID _asm _emit 0x0f _asm _emit 0xa2
#define RDTSC  _asm _emit 0x0f _asm _emit 0x31
#endif

namespace Titanium {
	namespace TIRA
	{

		void ImageProcess::BGRA32ToYUV444(
			unsigned char * pRgb, unsigned int uRgbStride,
			unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int uYuvStride,
			unsigned int uWidth, unsigned int uHeight)
		{
			int iR, iG, iB;
			int iRgbOffset = uRgbStride - uWidth * 4;
			int iYuvOffset = uYuvStride - uWidth;

			for (unsigned int i = 0; i < uHeight; i++)
			{
				for (unsigned int j = 0; j < uWidth; j++)
				{
					iB = *pRgb++;
					iG = *pRgb++;
					iR = *pRgb++;
					pRgb++;

					*pY++ = (unsigned char)((19595 * iR + 38470 * iG + 7471 * iB + 32768) >> 16);
					*pU++ = (unsigned char)((-11056 * iR - 21712 * iG + 32768 * iB + 8388608) >> 16);
					*pV++ = (unsigned char)((32768 * iR - 27440 * iG - 5328 * iB + 8388608) >> 16);
				}
				pRgb += iRgbOffset;
				pY += iYuvOffset;
				pU += iYuvOffset;
				pV += iYuvOffset;
			}
		}

		void ImageProcess::BGRA32ToYUV444_SSE(
			unsigned char * pRgb, unsigned int uRgbStride,
			unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int uYuvStride,
			unsigned int uWidth, unsigned int uHeight)
		{
#ifdef _WIN64
			BGRA32ToYUV444_utility(pRgb, uRgbStride, pY, pU, pV, uYuvStride, uWidth, uHeight);
#else
			if (uWidth % 16 != 0 || uWidth == 0)
			{
				BGRA32ToYUV444(pRgb, uRgbStride, pY, pU, pV, uYuvStride, uWidth, uHeight);
				return;
			}

			unsigned int mbCountInLine = uWidth / 16;

			ALIGN(16) static const __int64    xFF00FF00FF00FF00 = 0xFF00FF00FF00FF00;
			ALIGN(16) static const __int64    x8080808080808080 = 0x8080808080808080;

			ALIGN(16) static const __int64    multRY = 0x4C8B4C8B4C8B4C8B;//multRY = 19595d
			ALIGN(16) static const __int64    multGY = 0x9646964696469646;//multGY = 38470d
			ALIGN(16) static const __int64    multBY = 0x1D2F1D2F1D2F1D2F;//multBY =  7471d
			ALIGN(16) static const __int64    offBY = 0x0080008000800080;//offBY  =  32768d >> 8

			ALIGN(16) static const __int64    multRU = 0x2B302B302B302B30;//multRU = 11056d
			ALIGN(16) static const __int64    multGU = 0x54D054D054D054D0;//multGU = 21712d
			ALIGN(16) static const __int64    multBU = 0x8000800080008000;//multBU = 32768d

			ALIGN(16) static const __int64    multRV = 0x8000800080008000;//multRV = 32768d, multRV = multBU
			ALIGN(16) static const __int64    multGV = 0x6B306B306B306B30;//multGV = 27440d
			ALIGN(16) static const __int64    multBV = 0x14D014D014D014D0;//multBV =  5328d

			int  rgbOffset = uRgbStride - uWidth * 4;
			int  yuvOffset = uYuvStride - uWidth;
			__asm {
				; Init registers to point to base addresses
					mov         esi, pRgb; esi = pRgb
					mov         edi, pY; edi = pY
					mov         eax, pU; eax = pU
					mov         ebx, pV; ebx = pV
					mov         ecx, uHeight; ecx = loopCounter
					mov			edx, mbCountInLine;
			LoopRefresh:
				///////////////////////////////////////////////////////////////////////////////////
			LoopInLine :
				; Get first 8 Y pixels

				movq        mm6, multRY; mm6 = [ry       ry    ry    ry]
				movq        mm7, xFF00FF00FF00FF00; mm7 = [FF 00 FF 00 FF 00 FF 00]

				pinsrw      mm2_esi(5, 0); mm2 = [xx xx xx xx xx xx R1 G1]
				pinsrw      mm2_esi(13, 1); mm2 = [xx xx xx xx R3 G3 R1 G1]
				pinsrw      mm2_esi(21, 2); mm2 = [xx xx R5 G5 R3 G3 R1 G1]
				pinsrw      mm2_esi(29, 3); mm2 = [R7 G7 R5 G5 R3 G3 R1 G1]
				movq        mm5, mm2; mm5 = [R7 G7 R5 G5 R3 G3 R1 G1]
				psllw       mm2, 8; mm2 = [G7 00 G5 00 G3 00 G1 00]
				pand        mm5, mm7; mm5 = [R7 00 R5 00 R3 00 R1 00]

				pinsrw      mm0_esi(0, 0); mm0 = [xx xx xx xx xx xx G0 B0]
				pinsrw      mm0_esi(8, 1); mm0 = [xx xx xx xx G2 B2 G0 B0]
				pinsrw      mm0_esi(16, 2); mm0 = [xx xx G4 B4 G2 B2 G0 B0]
				pinsrw      mm0_esi(24, 3); mm0 = [G6 B6 G4 B4 G2 B2 G0 B0]
				movq        mm3, mm0; mm3 = [G6 B6 G4 B4 G2 B2 G0 B0]
				psllw       mm0, 8; mm0 = [B6 00 B4 00 B2 00 B0 00]
				pand        mm3, mm7; mm3 = [G6 00 G4 00 G2 00 G0 00]

				pinsrw      mm1_esi(3, 0); mm1 = [xx xx xx xx xx xx B1 A0]
				pinsrw      mm1_esi(11, 1); mm1 = [xx xx xx xx B3 A2 B1 A0]
				pinsrw      mm1_esi(19, 2); mm1 = [xx xx B5 A4 B3 A2 B1 A0]
				pinsrw      mm1_esi(27, 3); mm1 = [B7 A6 B5 A4 B3 A2 B1 A0]
				movq        mm4, mm1; mm4 = [B7 A6 B5 A4 B3 A2 B1 A0]
				pand        mm4, mm7; mm4 = [B7 00 B5 00 B3 00 B1 00]

				pinsrw      mm1_esi(2, 0); mm1 = [xx xx xx xx xx xx A0 R0]
				pinsrw      mm1_esi(10, 1); mm1 = [xx xx xx xx A2 R2 A0 R0]
				pinsrw      mm1_esi(18, 2); mm1 = [xx xx A4 R4 A2 R2 A0 R0]
				pinsrw      mm1_esi(26, 3); mm1 = [A6 R6 A4 R4 A2 R2 A0 R0]
				psllw       mm1, 8; mm1 = [R6 00 R4 00 R2 00 R0 00]


				movq        mm7, multGY; mm7 = [gy    gy    gy    gy]

				pmulhuw     mm5_mm6; mm5 = [ry7   ry5   ry3   ry1]
				pmulhuw     mm2_mm7; mm2 = [gy7   gy5   gy3   gy1]
				paddw       mm2, mm5; mm2 = [rgy7  rgy5  rgy3  rgy1]
				movq        mm5, multBY; mm5 = [by    by    by    by]
				pmulhuw     mm6_mm1; mm6 = [ry6   ry4   ry2   ry0]
				pmulhuw     mm7_mm3; mm7 = [gy6   gy4   gy2   gy0]
				paddw       mm6, mm7; mm6 = [rgy6  rgy4  rgy2  rgy0]
				movq        mm7, offBY; mm7 = [offby offby offby offby]
				pmulhuw     mm4_mm5; mm4 = [by7   by5   by3   by1]
				paddw       mm2, mm7; mm2 = [RGY7  RGY5  RGY3  RGY1], RGY7 = rgy7 + offBY
				pmulhuw     mm5_mm0; mm5 = [by6   by4   by2   by0]
				paddw       mm6, mm7; mm6 = [RGY6  RGY4  RGY2  RGY0]

				movq        mm7, xFF00FF00FF00FF00; mm7 = [FF00  FF00  FF00  FF00]
				paddw       mm2, mm4; mm2 = [Y7xx  Y5xx  Y3xx  Y1xx]
				paddw       mm5, mm6; mm5 = [Y6xx  Y4xx  Y2xx  Y0xx]
				psrlw       mm5, 8; mm5 = [00Y6  00Y4  00Y2  00Y0]
				pand        mm2, mm7; mm2 = [Y700  Y500  Y300  Y100]
				por         mm2, mm5; mm2 = [Y7Y6  Y5Y4  Y3Y2  Y1Y0]
				movq[edi], mm2; store[Y7Y6  Y5Y4  Y3Y2  Y1Y0]

				///////////////////////////////////////////////////////////////////////////////////
				; Get first 8 U pixels

				movq        mm6, multRU; mm6 = [ru    ru    ru    ru]
				movq        mm7, xFF00FF00FF00FF00; mm7 = [FF 00 FF 00 FF 00 FF 00]

				pinsrw      mm2_esi(5, 0); mm2 = [xx xx xx xx xx xx R1 G1]
				pinsrw      mm2_esi(13, 1); mm2 = [xx xx xx xx R3 G3 R1 G1]
				pinsrw      mm2_esi(21, 2); mm2 = [xx xx R5 G5 R3 G3 R1 G1]
				pinsrw      mm2_esi(29, 3); mm2 = [R7 G7 R5 G5 R3 G3 R1 G1]
				movq        mm5, mm2; mm5 = [R7 G7 R5 G5 R3 G3 R1 G1]
				psllw       mm2, 8; mm2 = [G7 00 G5 00 G3 00 G1 00]
				pand        mm5, mm7; mm5 = [R7 00 R5 00 R3 00 R1 00]

				//pinsrw      mm0_esi(0,0)                ; mm0 = [xx xx xx xx xx xx G0 B0]
				//pinsrw      mm0_esi(8,1)                ; mm0 = [xx xx xx xx G2 B2 G0 B0]
				//pinsrw      mm0_esi(16,2)               ; mm0 = [xx xx G4 B4 G2 B2 G0 B0]
				//pinsrw      mm0_esi(24,3)               ; mm0 = [G6 B6 G4 B4 G2 B2 G0 B0]
				//movq        mm3, mm0                    ; mm3 = [G6 B6 G4 B4 G2 B2 G0 B0]
				//psllw       mm0, 8                      ; mm0 = [B6 00 B4 00 B2 00 B0 00]
				//pand        mm3, mm7                    ; mm3 = [G6 00 G4 00 G2 00 G0 00]

				pinsrw      mm1_esi(3, 0); mm1 = [xx xx xx xx xx xx B1 A0]
				pinsrw      mm1_esi(11, 1); mm1 = [xx xx xx xx B3 A2 B1 A0]
				pinsrw      mm1_esi(19, 2); mm1 = [xx xx B5 A4 B3 A2 B1 A0]
				pinsrw      mm1_esi(27, 3); mm1 = [B7 A6 B5 A4 B3 A2 B1 A0]
				movq        mm4, mm1; mm4 = [B7 A6 B5 A4 B3 A2 B1 A0]
				pand        mm4, mm7; mm4 = [B7 00 B5 00 B3 00 B1 00]

				pinsrw      mm1_esi(2, 0); mm1 = [xx xx xx xx xx xx A0 R0]
				pinsrw      mm1_esi(10, 1); mm1 = [xx xx xx xx A2 R2 A0 R0]
				pinsrw      mm1_esi(18, 2); mm1 = [xx xx A4 R4 A2 R2 A0 R0]
				pinsrw      mm1_esi(26, 3); mm1 = [A6 R6 A4 R4 A2 R2 A0 R0]
				psllw       mm1, 8; mm1 = [R6 00 R4 00 R2 00 R0 00]

				movq        mm7, multGU; mm7 = [gu    gu    gu    gu]

				pmulhuw     mm5_mm6; mm5 = [ru7   ru5   ru3   ru1]
				pmulhuw     mm2_mm7; mm2 = [gu7   gu5   gu3   gu1]
				paddw       mm2, mm5; mm2 = [rgu7  rgu5  rgu3  rgu1]
				movq        mm5, multBU; mm5 = [bu    bu    bu    bu]
				pmulhuw     mm6_mm1; mm6 = [ru6   ru4   ru2   ru0]
				pmulhuw     mm7_mm3; mm7 = [gu6   gu4   gu2   gu0]
				paddw       mm6, mm7; mm6 = [rgu6  rgu4  rgu2  rgu0]
				pmulhuw     mm4_mm5; mm4 = [bu7   bu5   bu3   bu1]
				pmulhuw     mm5_mm0; mm5 = [bu6   bu4   bu2   bu0]

				movq        mm7, xFF00FF00FF00FF00; mm7 = [FF00  FF00  FF00  FF00]
				psubw       mm4, mm2; mm4 = [U7xx  U5xx  U3xx  U1xx]
				psubw       mm5, mm6; mm5 = [U6xx  U4xx  U2xx  U0xx]
				psrlw       mm5, 8; mm5 = [00U6  00U4  00U2  00U0]
				pand        mm4, mm7; mm4 = [U700  U500  U300  U100]
				por         mm4, mm5; mm4 = [U7U6  U5U4  U3U2  U1U0]
				movq        mm7, x8080808080808080; mm7 = [8080  8080  8080  8080]
				paddb       mm4, mm7; add 128 to each U pixel
				movq[eax], mm4; store[U7U6  U5U4  U3U2  U1U0]

				///////////////////////////////////////////////////////////////////////////////////
				; Get first 8 V pixels

				movq        mm6, multRV; mm6 = [rV    rv    rv    rv]
				movq        mm7, xFF00FF00FF00FF00; mm7 = [FF 00 FF 00 FF 00 FF 00]

				pinsrw      mm2_esi(5, 0); mm2 = [xx xx xx xx xx xx R1 G1]
				pinsrw      mm2_esi(13, 1); mm2 = [xx xx xx xx R3 G3 R1 G1]
				pinsrw      mm2_esi(21, 2); mm2 = [xx xx R5 G5 R3 G3 R1 G1]
				pinsrw      mm2_esi(29, 3); mm2 = [R7 G7 R5 G5 R3 G3 R1 G1]
				movq        mm5, mm2; mm5 = [R7 G7 R5 G5 R3 G3 R1 G1]
				psllw       mm2, 8; mm2 = [G7 00 G5 00 G3 00 G1 00]
				pand        mm5, mm7; mm5 = [R7 00 R5 00 R3 00 R1 00]

				//pinsrw      mm0_esi(0,0)                ; mm0 = [xx xx xx xx xx xx G0 B0]
				//pinsrw      mm0_esi(8,1)                ; mm0 = [xx xx xx xx G2 B2 G0 B0]
				//pinsrw      mm0_esi(16,2)               ; mm0 = [xx xx G4 B4 G2 B2 G0 B0]
				//pinsrw      mm0_esi(24,3)               ; mm0 = [G6 B6 G4 B4 G2 B2 G0 B0]
				//movq        mm3, mm0                    ; mm3 = [G6 B6 G4 B4 G2 B2 G0 B0]
				//psllw       mm0, 8                      ; mm0 = [B6 00 B4 00 B2 00 B0 00]
				//pand        mm3, mm7                    ; mm3 = [G6 00 G4 00 G2 00 G0 00]

				pinsrw      mm1_esi(3, 0); mm1 = [xx xx xx xx xx xx B1 A0]
				pinsrw      mm1_esi(11, 1); mm1 = [xx xx xx xx B3 A2 B1 A0]
				pinsrw      mm1_esi(19, 2); mm1 = [xx xx B5 A4 B3 A2 B1 A0]
				pinsrw      mm1_esi(27, 3); mm1 = [B7 A6 B5 A4 B3 A2 B1 A0]
				movq        mm4, mm1; mm4 = [B7 A6 B5 A4 B3 A2 B1 A0]
				pand        mm4, mm7; mm4 = [B7 00 B5 00 B3 00 B1 00]

				pinsrw      mm1_esi(2, 0); mm1 = [xx xx xx xx xx xx A0 R0]
				pinsrw      mm1_esi(10, 1); mm1 = [xx xx xx xx A2 R2 A0 R0]
				pinsrw      mm1_esi(18, 2); mm1 = [xx xx A4 R4 A2 R2 A0 R0]
				pinsrw      mm1_esi(26, 3); mm1 = [A6 R6 A4 R4 A2 R2 A0 R0]
				psllw       mm1, 8; mm1 = [R6 00 R4 00 R2 00 R0 00]

				movq        mm7, multGV; mm7 = [gv    gv    gv    gv]

				pmulhuw     mm5_mm6; mm5 = [rv7   rv5   rv3   rv1]
				pmulhuw     mm2_mm7; mm2 = [gv7   gv5   gv3   gv1]
				psubw       mm5, mm2; mm5 = [rgv7  rgv5  rgv3  rgv1]
				movq        mm2, multBV; mm2 = [bv    bv    bv    bv]
				pmulhuw     mm6_mm1; mm6 = [rv6   rv4   rv2   rv0]
				pmulhuw     mm7_mm3; mm7 = [gv6   gv4   gv2   gv0]
				psubw       mm6, mm7; mm6 = [rgv6  rgv4  rgv2  rgv0]
				pmulhuw     mm4_mm2; mm4 = [bv7   bv5   bv3   bv1]
				pmulhuw     mm2_mm0; mm2 = [bv6   bv4   bv2   bv0]

				movq        mm7, xFF00FF00FF00FF00; mm7 = [FF00  FF00  FF00  FF00]
				psubw       mm5, mm4; mm5 = [V7xx  V5xx  V3xx  V1xx]
				psubw       mm6, mm2; mm6 = [V6xx  V4xx  V2xx  V0xx]
				psrlw       mm6, 8; mm6 = [00V6  00V4  00V2  00V0]
				pand        mm5, mm7; mm5 = [V700  V500  V300  V100]
				por         mm6, mm5; mm6 = [V7V6  V5V4  V3V2  V1V0]
				movq        mm7, x8080808080808080; mm7 = [8080  8080  8080  8080]
				paddb       mm6, mm7; add 128 to each V pixel
				movq[ebx], mm6; store[V7V6  V5V4  V3V2  V1V0]

				add         esi, 32; esi = pRgb + 32
				add         edi, 8; edi = pY + 8;
					   add         eax, 8; eax = pU + 8
						   add         ebx, 8; ebx = pV + 8

						   ///////////////////////////////////////////////////////////////////////////////////
						   ///////////////////////////////////////////////////////////////////////////////////
						   ; Get second 8 Y pixels

						   movq        mm6, multRY; mm6 = [ry       ry    ry    ry]
						   movq        mm7, xFF00FF00FF00FF00; mm7 = [FF 00 FF 00 FF 00 FF 00]

						   pinsrw      mm2_esi(5, 0); mm2 = [xx xx xx xx xx xx R1 G1]
						   pinsrw      mm2_esi(13, 1); mm2 = [xx xx xx xx R3 G3 R1 G1]
						   pinsrw      mm2_esi(21, 2); mm2 = [xx xx R5 G5 R3 G3 R1 G1]
						   pinsrw      mm2_esi(29, 3); mm2 = [R7 G7 R5 G5 R3 G3 R1 G1]
						   movq        mm5, mm2; mm5 = [R7 G7 R5 G5 R3 G3 R1 G1]
						   psllw       mm2, 8; mm2 = [G7 00 G5 00 G3 00 G1 00]
						   pand        mm5, mm7; mm5 = [R7 00 R5 00 R3 00 R1 00]

						   pinsrw      mm0_esi(0, 0); mm0 = [xx xx xx xx xx xx G0 B0]
						   pinsrw      mm0_esi(8, 1); mm0 = [xx xx xx xx G2 B2 G0 B0]
						   pinsrw      mm0_esi(16, 2); mm0 = [xx xx G4 B4 G2 B2 G0 B0]
						   pinsrw      mm0_esi(24, 3); mm0 = [G6 B6 G4 B4 G2 B2 G0 B0]
						   movq        mm3, mm0; mm3 = [G6 B6 G4 B4 G2 B2 G0 B0]
						   psllw       mm0, 8; mm0 = [B6 00 B4 00 B2 00 B0 00]
						   pand        mm3, mm7; mm3 = [G6 00 G4 00 G2 00 G0 00]

						   pinsrw      mm1_esi(3, 0); mm1 = [xx xx xx xx xx xx B1 A0]
						   pinsrw      mm1_esi(11, 1); mm1 = [xx xx xx xx B3 A2 B1 A0]
						   pinsrw      mm1_esi(19, 2); mm1 = [xx xx B5 A4 B3 A2 B1 A0]
						   pinsrw      mm1_esi(27, 3); mm1 = [B7 A6 B5 A4 B3 A2 B1 A0]
						   movq        mm4, mm1; mm4 = [B7 A6 B5 A4 B3 A2 B1 A0]
						   pand        mm4, mm7; mm4 = [B7 00 B5 00 B3 00 B1 00]

						   pinsrw      mm1_esi(2, 0); mm1 = [xx xx xx xx xx xx A0 R0]
						   pinsrw      mm1_esi(10, 1); mm1 = [xx xx xx xx A2 R2 A0 R0]
						   pinsrw      mm1_esi(18, 2); mm1 = [xx xx A4 R4 A2 R2 A0 R0]
						   pinsrw      mm1_esi(26, 3); mm1 = [A6 R6 A4 R4 A2 R2 A0 R0]
						   psllw       mm1, 8; mm1 = [R6 00 R4 00 R2 00 R0 00]


						   movq        mm7, multGY; mm7 = [gy    gy    gy    gy]

						   pmulhuw     mm5_mm6; mm5 = [ry7   ry5   ry3   ry1]
						   pmulhuw     mm2_mm7; mm2 = [gy7   gy5   gy3   gy1]
						   paddw       mm2, mm5; mm2 = [rgy7  rgy5  rgy3  rgy1]
						   movq        mm5, multBY; mm5 = [by    by    by    by]
						   pmulhuw     mm6_mm1; mm6 = [ry6   ry4   ry2   ry0]
						   pmulhuw     mm7_mm3; mm7 = [gy6   gy4   gy2   gy0]
						   paddw       mm6, mm7; mm6 = [rgy6  rgy4  rgy2  rgy0]
						   movq        mm7, offBY; mm7 = [offby offby offby offby]
						   pmulhuw     mm4_mm5; mm4 = [by7   by5   by3   by1]
						   paddw       mm2, mm7; mm2 = [RGY7  RGY5  RGY3  RGY1], RGY7 = rgy7 + offBY
						   pmulhuw     mm5_mm0; mm5 = [by6   by4   by2   by0]
						   paddw       mm6, mm7; mm6 = [RGY6  RGY4  RGY2  RGY0]

						   movq        mm7, xFF00FF00FF00FF00; mm7 = [FF00  FF00  FF00  FF00]
						   paddw       mm2, mm4; mm2 = [Y7xx  Y5xx  Y3xx  Y1xx]
						   paddw       mm5, mm6; mm5 = [Y6xx  Y4xx  Y2xx  Y0xx]
						   psrlw       mm5, 8; mm5 = [00Y6  00Y4  00Y2  00Y0]
						   pand        mm2, mm7; mm2 = [Y700  Y500  Y300  Y100]
						   por         mm2, mm5; mm2 = [Y7Y6  Y5Y4  Y3Y2  Y1Y0]
						   movq[edi], mm2; store[Y7Y6  Y5Y4  Y3Y2  Y1Y0]

						   ///////////////////////////////////////////////////////////////////////////////////
						   ; Get second 8 U pixels

						   movq        mm6, multRU; mm6 = [ru    ru    ru    ru]
						   movq        mm7, xFF00FF00FF00FF00; mm7 = [FF 00 FF 00 FF 00 FF 00]

						   pinsrw      mm2_esi(5, 0); mm2 = [xx xx xx xx xx xx R1 G1]
						   pinsrw      mm2_esi(13, 1); mm2 = [xx xx xx xx R3 G3 R1 G1]
						   pinsrw      mm2_esi(21, 2); mm2 = [xx xx R5 G5 R3 G3 R1 G1]
						   pinsrw      mm2_esi(29, 3); mm2 = [R7 G7 R5 G5 R3 G3 R1 G1]
						   movq        mm5, mm2; mm5 = [R7 G7 R5 G5 R3 G3 R1 G1]
						   psllw       mm2, 8; mm2 = [G7 00 G5 00 G3 00 G1 00]
						   pand        mm5, mm7; mm5 = [R7 00 R5 00 R3 00 R1 00]

						   //pinsrw      mm0_esi(0,0)                ; mm0 = [xx xx xx xx xx xx G0 B0]
						   //pinsrw      mm0_esi(8,1)                ; mm0 = [xx xx xx xx G2 B2 G0 B0]
						   //pinsrw      mm0_esi(16,2)               ; mm0 = [xx xx G4 B4 G2 B2 G0 B0]
						   //pinsrw      mm0_esi(24,3)               ; mm0 = [G6 B6 G4 B4 G2 B2 G0 B0]
						   //movq        mm3, mm0                    ; mm3 = [G6 B6 G4 B4 G2 B2 G0 B0]
						   //psllw       mm0, 8                      ; mm0 = [B6 00 B4 00 B2 00 B0 00]
						   //pand        mm3, mm7                    ; mm3 = [G6 00 G4 00 G2 00 G0 00]

						   pinsrw      mm1_esi(3, 0); mm1 = [xx xx xx xx xx xx B1 A0]
						   pinsrw      mm1_esi(11, 1); mm1 = [xx xx xx xx B3 A2 B1 A0]
						   pinsrw      mm1_esi(19, 2); mm1 = [xx xx B5 A4 B3 A2 B1 A0]
						   pinsrw      mm1_esi(27, 3); mm1 = [B7 A6 B5 A4 B3 A2 B1 A0]
						   movq        mm4, mm1; mm4 = [B7 A6 B5 A4 B3 A2 B1 A0]
						   pand        mm4, mm7; mm4 = [B7 00 B5 00 B3 00 B1 00]

						   pinsrw      mm1_esi(2, 0); mm1 = [xx xx xx xx xx xx A0 R0]
						   pinsrw      mm1_esi(10, 1); mm1 = [xx xx xx xx A2 R2 A0 R0]
						   pinsrw      mm1_esi(18, 2); mm1 = [xx xx A4 R4 A2 R2 A0 R0]
						   pinsrw      mm1_esi(26, 3); mm1 = [A6 R6 A4 R4 A2 R2 A0 R0]
						   psllw       mm1, 8; mm1 = [R6 00 R4 00 R2 00 R0 00]

						   movq        mm7, multGU; mm7 = [gu    gu    gu    gu]

						   pmulhuw     mm5_mm6; mm5 = [ru7   ru5   ru3   ru1]
						   pmulhuw     mm2_mm7; mm2 = [gu7   gu5   gu3   gu1]
						   paddw       mm2, mm5; mm2 = [rgu7  rgu5  rgu3  rgu1]
						   movq        mm5, multBU; mm5 = [bu    bu    bu    bu]
						   pmulhuw     mm6_mm1; mm6 = [ru6   ru4   ru2   ru0]
						   pmulhuw     mm7_mm3; mm7 = [gu6   gu4   gu2   gu0]
						   paddw       mm6, mm7; mm6 = [rgu6  rgu4  rgu2  rgu0]
						   pmulhuw     mm4_mm5; mm4 = [bu7   bu5   bu3   bu1]
						   pmulhuw     mm5_mm0; mm5 = [bu6   bu4   bu2   bu0]

						   movq        mm7, xFF00FF00FF00FF00; mm7 = [FF00  FF00  FF00  FF00]
						   psubw       mm4, mm2; mm4 = [U7xx  U5xx  U3xx  U1xx]
						   psubw       mm5, mm6; mm5 = [U6xx  U4xx  U2xx  U0xx]
						   psrlw       mm5, 8; mm5 = [00U6  00U4  00U2  00U0]
						   pand        mm4, mm7; mm4 = [U700  U500  U300  U100]
						   por         mm4, mm5; mm4 = [U7U6  U5U4  U3U2  U1U0]
						   movq        mm7, x8080808080808080; mm7 = [8080  8080  8080  8080]
						   paddb       mm4, mm7; add 128 to each U pixel
						   movq[eax], mm4; store[U7U6  U5U4  U3U2  U1U0]

						   ///////////////////////////////////////////////////////////////////////////////////
						   ; Get second 8 V pixels

						   movq        mm6, multRV; mm6 = [rV    rv    rv    rv]
						   movq        mm7, xFF00FF00FF00FF00; mm7 = [FF 00 FF 00 FF 00 FF 00]

						   pinsrw      mm2_esi(5, 0); mm2 = [xx xx xx xx xx xx R1 G1]
						   pinsrw      mm2_esi(13, 1); mm2 = [xx xx xx xx R3 G3 R1 G1]
						   pinsrw      mm2_esi(21, 2); mm2 = [xx xx R5 G5 R3 G3 R1 G1]
						   pinsrw      mm2_esi(29, 3); mm2 = [R7 G7 R5 G5 R3 G3 R1 G1]
						   movq        mm5, mm2; mm5 = [R7 G7 R5 G5 R3 G3 R1 G1]
						   psllw       mm2, 8; mm2 = [G7 00 G5 00 G3 00 G1 00]
						   pand        mm5, mm7; mm5 = [R7 00 R5 00 R3 00 R1 00]

						   //pinsrw      mm0_esi(0,0)                ; mm0 = [xx xx xx xx xx xx G0 B0]
						   //pinsrw      mm0_esi(8,1)                ; mm0 = [xx xx xx xx G2 B2 G0 B0]
						   //pinsrw      mm0_esi(16,2)               ; mm0 = [xx xx G4 B4 G2 B2 G0 B0]
						   //pinsrw      mm0_esi(24,3)               ; mm0 = [G6 B6 G4 B4 G2 B2 G0 B0]
						   //movq        mm3, mm0                    ; mm3 = [G6 B6 G4 B4 G2 B2 G0 B0]
						   //psllw       mm0, 8                      ; mm0 = [B6 00 B4 00 B2 00 B0 00]
						   //pand        mm3, mm7                    ; mm3 = [G6 00 G4 00 G2 00 G0 00]

						   pinsrw      mm1_esi(3, 0); mm1 = [xx xx xx xx xx xx B1 A0]
						   pinsrw      mm1_esi(11, 1); mm1 = [xx xx xx xx B3 A2 B1 A0]
						   pinsrw      mm1_esi(19, 2); mm1 = [xx xx B5 A4 B3 A2 B1 A0]
						   pinsrw      mm1_esi(27, 3); mm1 = [B7 A6 B5 A4 B3 A2 B1 A0]
						   movq        mm4, mm1; mm4 = [B7 A6 B5 A4 B3 A2 B1 A0]
						   pand        mm4, mm7; mm4 = [B7 00 B5 00 B3 00 B1 00]

						   pinsrw      mm1_esi(2, 0); mm1 = [xx xx xx xx xx xx A0 R0]
						   pinsrw      mm1_esi(10, 1); mm1 = [xx xx xx xx A2 R2 A0 R0]
						   pinsrw      mm1_esi(18, 2); mm1 = [xx xx A4 R4 A2 R2 A0 R0]
						   pinsrw      mm1_esi(26, 3); mm1 = [A6 R6 A4 R4 A2 R2 A0 R0]
						   psllw       mm1, 8; mm1 = [R6 00 R4 00 R2 00 R0 00]

						   movq        mm7, multGV; mm7 = [gv    gv    gv    gv]

						   pmulhuw     mm5_mm6; mm5 = [rv7   rv5   rv3   rv1]
						   pmulhuw     mm2_mm7; mm2 = [gv7   gv5   gv3   gv1]
						   psubw       mm5, mm2; mm5 = [rgv7  rgv5  rgv3  rgv1]
						   movq        mm2, multBV; mm2 = [bv    bv    bv    bv]
						   pmulhuw     mm6_mm1; mm6 = [rv6   rv4   rv2   rv0]
						   pmulhuw     mm7_mm3; mm7 = [gv6   gv4   gv2   gv0]
						   psubw       mm6, mm7; mm6 = [rgv6  rgv4  rgv2  rgv0]
						   pmulhuw     mm4_mm2; mm4 = [bv7   bv5   bv3   bv1]
						   pmulhuw     mm2_mm0; mm2 = [bv6   bv4   bv2   bv0]

						   movq        mm7, xFF00FF00FF00FF00; mm7 = [FF00  FF00  FF00  FF00]
						   psubw       mm5, mm4; mm5 = [V7xx  V5xx  V3xx  V1xx]
						   psubw       mm6, mm2; mm6 = [V6xx  V4xx  V2xx  V0xx]
						   psrlw       mm6, 8; mm6 = [00V6  00V4  00V2  00V0]
						   pand        mm5, mm7; mm5 = [V700  V500  V300  V100]
						   por         mm6, mm5; mm6 = [V7V6  V5V4  V3V2  V1V0]
						   movq        mm7, x8080808080808080; mm7 = [8080  8080  8080  8080]
						   paddb       mm6, mm7; add 128 to each V pixel
						   movq[ebx], mm6; store[V7V6  V5V4  V3V2  V1V0]

						   add         esi, 32; esi = pRgb + 64
						   add         edi, 8; edi = pY + 8;
					   add         eax, 8; eax = pU + 8
						   add         ebx, 8; ebx = pV + 8

						   dec			edx;
					   jnz			LoopInLine;

					   add         esi, rgbOffset; esi = esi + rgbOffset;
					   add			edi, yuvOffset;
					   add			eax, yuvOffset;
					   add			ebx, yuvOffset;
					   mov			edx, mbCountInLine;

					   dec         ecx;
					   jnz         LoopRefresh;


					   emms
			}
#endif
		}

		//ftBilinearTable_SSE2(out [edi+ebx*4]; xmm5=v,xmm6=vr,xmm7=0,[ebp]=(u,ur),[edx]=srx_x,esi=PSrcLineColor,ecx=PSrcLineColorNext)
		void __declspec(naked) ftBilinearTable_SSE2(){
			//#define  ftBilinearTable_SSE2()                     /
			asm mov         eax, [edx + ebx]
				asm movq        xmm0, qword ptr[esi + eax * 4]
				asm movq        xmm1, qword ptr[ecx + eax * 4]
				asm punpcklbw   xmm0, xmm7
				asm punpcklbw   xmm1, xmm7
				asm pmullw      xmm0, xmm5
				asm pmullw      xmm1, xmm6
				asm paddw       xmm0, xmm1
				asm pmulhw      xmm0, xmmword ptr[ebp + ebx * 4]
				asm movdqa      xmm1, xmm0
				asm punpckhqdq  xmm0, xmm0
				asm paddw       xmm0, xmm1
				asm packuswb    xmm0, xmm7
				asm movd  dword ptr[edi + ebx], xmm0
				asm ret //for  __declspec(naked)
		}
		//}
		void __declspec(naked) ftBilinearTable_SSE2_expand2(){
			//#define  ftBilinearTable_SSE2_expand2()             /
			asm mov         eax, [edx + ebx]
				asm movq        xmm0, qword ptr[esi + eax * 4]
				asm movq        xmm1, qword ptr[ecx + eax * 4]
				asm mov         eax, [edx + ebx + 4]
				asm movq        xmm2, qword ptr[esi + eax * 4]
				asm movq        xmm3, qword ptr[ecx + eax * 4]
				asm punpcklbw   xmm0, xmm7
				asm punpcklbw   xmm1, xmm7
				asm punpcklbw   xmm2, xmm7
				asm punpcklbw   xmm3, xmm7
				asm pmullw      xmm0, xmm5
				asm pmullw      xmm1, xmm6
				asm pmullw      xmm2, xmm5
				asm pmullw      xmm3, xmm6
				asm paddw       xmm0, xmm1
				asm paddw       xmm2, xmm3
				asm pmulhw      xmm0, xmmword ptr[ebp + ebx * 4]
				asm pmulhw      xmm2, xmmword ptr[ebp + ebx * 4 + 16]
				asm movdqa      xmm1, xmm0
				asm punpcklqdq  xmm0, xmm2
				asm punpckhqdq  xmm1, xmm2
				asm paddw       xmm0, xmm1
				asm packuswb    xmm0, xmm7
				asm movq  qword ptr[edi + ebx], xmm0
				asm ret
				//for  __declspec(naked)
		}
		//}	

		void ImageProcess::ImageScaling(const Bgra32Rect& Dst, const Bgra32Rect& Src, ImageScalingMethod method)
		{
			switch (method)
			{
			case Align_SSE:
				ImageScaling_Align_SSE(Dst, Src);
				break;
			case Align_SSE_mmh:
				ImageScaling_Align_SSE_mmh(Dst, Src);
				break;
			case Bilinear_MMX_Ex:
				ImageScaling_Bilinear_MMX_Ex(Dst, Src);
				break;
			case Bilinear_SSE2:
				ImageScaling_Bilinear_SSE2(Dst, Src);
				break;
			case BilinearTable_SSE2:
				ImageScaling_BilinearTable_SSE2(Dst, Src);
				break;
			case ThreeOrder_MMX:
				ImageScaling_ThreeOrder_MMX(Dst, Src);
				break;
			case ThreeOrderTable_SSE2:
				ImageScaling_ThreeOrderTable_SSE2(Dst, Src);
				break;
			default:
				ImageScaling_Bilinear_MMX_Ex(Dst, Src);
				break;
			}
		}
		void  __declspec(naked) Bilinear_Fast_MMX_expand1()
		{
			asm
			{
				mov       eax, edx
				shl       eax, 16
				shr       eax, 24
				//== movzx       eax,dh  //eax=u_8 
				MOVD      XMM5, eax
				mov       eax, edx
				shr       eax, 16     //srcx_16>>16 


				MOVD         XMM2, dword ptr[ecx + eax * 4]  //MM2=Color0 
				MOVD         XMM0, dword ptr[ecx + eax * 4 + 4]//MM0=Color2 
				PUNPCKLWD    XMM5, XMM5
				MOVD         XMM3, dword ptr[esi + eax * 4]  //MM3=Color1 
				MOVD         XMM1, dword ptr[esi + eax * 4 + 4]//MM1=Color3 
				PUNPCKLDQ    XMM5, XMM5 //mm5=u_8 
				PUNPCKLBW    XMM0, XMM7
				PUNPCKLBW    XMM1, XMM7
				PUNPCKLBW    XMM2, XMM7
				PUNPCKLBW    XMM3, XMM7
				PSUBw        XMM0, XMM2
				PSUBw        XMM1, XMM3
				PSLLw        XMM2, 8
				PSLLw        XMM3, 8
				PMULlw       XMM0, XMM5
				PMULlw       XMM1, XMM5
				PADDw        XMM0, XMM2
				PADDw        XMM1, XMM3


				PSRLw        XMM0, 8
				PSRLw        XMM1, 8
				PSUBw        XMM0, XMM1
				PSLLw        XMM1, 8
				PMULlw       XMM0, XMM6
				PADDw        XMM0, XMM1


				PSRLw     XMM0, 8
				PACKUSwb  XMM0, XMM7
				MOVd   dword ptr[edi + ebx * 4], XMM0 //write DstColor 


				ret
			}
		}
		void __declspec(naked) ftBilinear_SSE2_expand2()
		{
			asm
			{
				lea       eax, [edx + ebp]
				MOVD      XMM5, edx
				MOVD      XMM4, eax
				PUNPCKLWD XMM5, XMM4
				PSRLW     XMM5, 8


				mov       eax, edx
				shr       eax, 16     //srcx_16>>16 
				PUNPCKLWD    XMM5, XMM5
				MOVQ         XMM2, qword ptr[ecx + eax * 4]//XMM2=0  0  Color0 Color2 
				MOVQ         XMM3, qword ptr[esi + eax * 4]//XMM3=0  0  Color1 Color3 
				lea       eax, [edx + ebp]
				shr       eax, 16     //srcx_16>>16 
				PUNPCKLDQ    XMM5, XMM5 //mm5=u_8' u_8' u_8' u_8' u_8 u_8 u_8 u_8  
				movq   xmm4, qword ptr[ecx + eax * 4]
				PUNPCKLDQ    XMM2, xmm4//XMM2=Color0' Color0  Color2' Color2 
				movq   xmm4, qword ptr[esi + eax * 4]
				PUNPCKLDQ    XMM3, xmm4//XMM3=Color1' Color1  Color3' Color3 
				MOVHLPS      XMM0, XMM2 //XMM0= X  X  Color0' Color0 
				MOVHLPS      XMM1, XMM3 //XMM1= X  X  Color1' Color1 


				PUNPCKLBW    XMM0, XMM7
				PUNPCKLBW    XMM1, XMM7
				PUNPCKLBW    XMM2, XMM7
				PUNPCKLBW    XMM3, XMM7
				PSUBw        XMM0, XMM2
				PSUBw        XMM1, XMM3
				PSLLw        XMM2, 8
				PSLLw        XMM3, 8
				PMULlw       XMM0, XMM5
				PMULlw       XMM1, XMM5
				PADDw        XMM0, XMM2
				PADDw        XMM1, XMM3


				PSRLw        XMM0, 8
				PSRLw        XMM1, 8
				PSUBw        XMM0, XMM1
				PSLLw        XMM1, 8
				PMULlw       XMM0, XMM6
				PADDw        XMM0, XMM1


				PSRLw     XMM0, 8
				PACKUSwb  XMM0, XMM7


				//MOVQ qword ptr [edi+ebx*4], xmm0//write two DstColor 
				MOVDQ2Q   xmm4, xmm0
				movntq  qword ptr[edi + ebx * 4], xmm4


				ret
			}
		}
		void  ImageProcess::ImageScaling_Bilinear_SSE2(const Bgra32Rect& Dst, const Bgra32Rect& Src)
		{
			if ((0 == Dst.width) || (0 == Dst.height)
				|| (2 > Src.width) || (2 > Src.height)) return;


			long xrIntFloat_16 = ((Src.width - 1) << 16) / Dst.width;
			long yrIntFloat_16 = ((Src.height - 1) << 16) / Dst.height;


			long dst_width = Dst.width;
			long Src_byte_width = Src.byte_width;
			Bgra32* pDstLine = Dst.pdata;
			long srcy_16 = 0;
			asm pxor  xmm7, xmm7 //xmm7=0 
				for (long y = 0; y < Dst.height; ++y)
				{
					unsigned long v_8 = (srcy_16 & 0xFFFF) >> 8;
					Bgra32* PSrcLineColor = (Bgra32*)((TUInt8*)(Src.pdata) + Src_byte_width*(srcy_16 >> 16));
					Bgra32* PSrcLineColorNext = (Bgra32*)((TUInt8*)(PSrcLineColor)+Src_byte_width);
					asm
					{
						movd        xmm6, v_8
						PUNPCKLWD   xmm6, xmm6
						PUNPCKLDQ   xmm6, xmm6
						PUNPCKLQDQ  xmm6, xmm6//xmm6=v_8 

						mov       esi, PSrcLineColor
						mov       ecx, PSrcLineColorNext
						xor       edx, edx   //srcx_16=0 
						mov       ebx, dst_width
						mov       edi, pDstLine
						push      ebp
						mov       ebp, xrIntFloat_16
						push      ebx
						and       ebx, (not 1)
						test      ebx, ebx   //nop 
						jle     end_loop2


						lea       edi, [edi + ebx * 4]
						neg       ebx


					loop2_start :
						call ftBilinear_SSE2_expand2
							lea       edx, [edx + ebp * 2]
							add       ebx, 2


							jnz       loop2_start


						end_loop2 :
						pop    ebx
							and    ebx, 1
							test   ebx, ebx
							jle    end_write


							lea       edi, [edi + ebx * 4]
							neg       ebx
							call Bilinear_Fast_MMX_expand1
						end_write :


						pop       ebp
					}
					srcy_16 += yrIntFloat_16;
					((TUInt8*&)pDstLine) += Dst.byte_width;
				}
			asm emms
		}


		void ImageProcess::ImageScaling_BilinearTable_SSE2(const Bgra32Rect& Dst, const Bgra32Rect& Src)
		{
			if ((0 == Dst.width) || (0 == Dst.height)
				|| (2 > Src.width) || (2 > Src.height)) return;
			long xrIntFloat_16 = ((Src.width - 1) << 16) / Dst.width;
			long yrIntFloat_16 = ((Src.height - 1) << 16) / Dst.height;
			long dst_width = Dst.width;
			TUInt8* _bufMem = new TUInt8[(dst_width * 2 * sizeof(TMMXData64)+15) + dst_width*sizeof(Int32)];
			TMMXData64* uList = (TMMXData64*)((((ptrdiff_t)_bufMem) + 15) >> 4 << 4); //16byte align
			Int32* xList = (Int32*)(uList + dst_width * 2);
			{//init u table
				long srcx_16 = 0;
				for (long x = 0; x < dst_width * 2; x += 2){
					xList[x >> 1] = (srcx_16 >> 16);
					unsigned long u = (srcx_16 >> 8) & 0xFF;
					unsigned long ur = (256 - u) << 1;
					u = u << 1;
					uList[x + 0] = (ur | (ur << 16));
					uList[x + 0] |= uList[x + 0] << 32;
					uList[x + 1] = u | (u << 16);
					uList[x + 1] |= uList[x + 1] << 32;
					srcx_16 += xrIntFloat_16;
				}
			}
			Bgra32* pDstLine = Dst.pdata;
			long srcy_16 = 0;
			asm pxor  xmm7, xmm7 //xmm7=0
			for (long y = 0; y < Dst.height; ++y){
				unsigned long v = (srcy_16 >> 8) & 0xFF;
				unsigned long vr = (256 - v) >> 1;
				v >>= 1;
				Bgra32* PSrcLineColor = (Bgra32*)((TUInt8*)(Src.pdata) + Src.byte_width*(srcy_16 >> 16));
				Bgra32* PSrcLineColorNext = (Bgra32*)((TUInt8*)(PSrcLineColor)+Src.byte_width);
				asm{
					movd        xmm5, vr
					movd        xmm6, v
					punpcklwd   xmm5, xmm5
					punpcklwd   xmm6, xmm6
					punpckldq   xmm5, xmm5
					punpckldq   xmm6, xmm6
					punpcklqdq  xmm5, xmm5
					punpcklqdq  xmm6, xmm6

					mov       esi, PSrcLineColor
					mov       ecx, PSrcLineColorNext
					mov       edx, xList //x
					mov       ebx, dst_width
					mov       edi, pDstLine
					push      ebp
					mov       ebp, uList
					push      ebx

					and       ebx, (not 1)
					test      ebx, ebx
					jle     end_loop2

					lea       ebx, [ebx * 4]
					lea       edi, [edi + ebx]
					lea       edx, [edx + ebx]
					lea       ebp, [ebp + ebx * 4]
					neg       ebx
				loop2_start :
					call ftBilinearTable_SSE2_expand2
						//ftBilinearTable_SSE2_expand2()
						add       ebx, 8
						jnz       loop2_start
					end_loop2 :
					pop    ebx
						and    ebx, 1
						test   ebx, ebx
						jle    end_write
						lea       ebx, [ebx * 4]
						lea       edi, [edi + ebx]
						lea       edx, [edx + ebx]
						lea       ebp, [ebp + ebx * 4]
						neg       ebx
					loop1_start :
					call ftBilinearTable_SSE2
						//ftBilinearTable_SSE2()
						add       ebx, 4
						jnz       loop1_start
					end_write :
					pop       ebp
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
			delete[]_bufMem;
		}

		void ImageProcess::ImageScaling_Align_SSE(const Bgra32Rect& Dst, const Bgra32Rect& Src)
		{
			if ((0 == Dst.width) || (0 == Dst.height)
				|| (0 == Src.width) || (0 == Src.height)) return;
			unsigned long xrIntFloat_16 = (Src.width << 16) / Dst.width + 1;
			unsigned long yrIntFloat_16 = (Src.height << 16) / Dst.height + 1;

			unsigned long dst_width = Dst.width;
			Bgra32* pDstLine = Dst.pdata;
			unsigned long srcy_16 = 0;
			for (unsigned long y = 0; y < Dst.height; ++y)
			{
				Bgra32* pSrcLine = ((Bgra32*)((TUInt8*)Src.pdata + Src.byte_width*(srcy_16 >> 16)));

				asm
				{
					push      ebp
					mov       esi, pSrcLine
					mov       edi, pDstLine
					mov       edx, xrIntFloat_16
					mov       ecx, dst_width
					xor       ebp, ebp           //srcx_16=0

					and    ecx, (not 3)
					TEST   ECX, ECX   //nop
					jle    EndWriteLoop

					lea       edi, [edi + ecx * 4]
					neg       ecx

					//todo: ??

				WriteLoop :
					mov       eax, ebp
						shr       eax, 16            //srcx_16>>16
						lea       ebx, [ebp + edx]
						movd      mm0, [esi + eax * 4]
						shr       ebx, 16            //srcx_16>>16
						PUNPCKlDQ mm0, [esi + ebx * 4]
						lea       ebp, [ebp + edx * 2]

						// movntq qword ptr [edi+ecx*4], mm0  
						asm _emit 0x0F asm _emit 0xE7 asm _emit 0x04 asm _emit 0x8F

						mov       eax, ebp
						shr       eax, 16            //srcx_16>>16
						lea       ebx, [ebp + edx]
						movd      mm1, [esi + eax * 4]
						shr       ebx, 16            //srcx_16>>16
						PUNPCKlDQ mm1, [esi + ebx * 4]
						lea       ebp, [ebp + edx * 2]

						// movntq qword ptr [edi+ecx*4+8], mm1 
						asm _emit 0x0F asm _emit 0xE7 asm _emit 0x4C asm _emit 0x8F asm _emit 0x08

						add ecx, 4
						jnz WriteLoop

						asm _emit 0x0F asm _emit 0xAE asm _emit 0xF8
						emms
					EndWriteLoop :

					mov    ebx, ebp
						pop    ebp

						mov    ecx, dst_width
						and    ecx, 3
						TEST   ECX, ECX
						jle    EndLineZoom

						lea       edi, [edi + ecx * 4]
						neg       ecx
					StartBorder :
					mov       eax, ebx
						shr       eax, 16            //srcx_16>>16
						mov       eax, [esi + eax * 4]
						mov[edi + ecx * 4], eax
						add       ebx, edx

						inc       ECX
						JNZ       StartBorder
					EndLineZoom :
				}

				//
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
		}

		//=====================================================================

		void ImageProcess::ImageScaling_Align_SSE_mmh(const Bgra32Rect& Dst, const Bgra32Rect& Src)
		{

			if ((0 == Dst.width) || (0 == Dst.height)
				|| (0 == Src.width) || (0 == Src.height)) return;

			unsigned long xrIntFloat_16 = (Src.width << 16) / Dst.width + 1;
			unsigned long yrIntFloat_16 = (Src.height << 16) / Dst.height + 1;

			unsigned long dst_width = Dst.width;
			Bgra32* pDstLine = Dst.pdata;
			unsigned long srcy_16 = 0;
			unsigned long for4count = dst_width / 4 * 4;
			for (unsigned long y = 0; y < Dst.height; ++y)
			{
				Bgra32* pSrcLine = ((Bgra32*)((TUInt8*)Src.pdata + Src.byte_width*(srcy_16 >> 16)));
				unsigned long srcx_16 = 0;
				unsigned long x;
				for (x = 0; x < for4count; x += 4)
				{
					__m64 m0 = _m_from_int(*(int*)(&pSrcLine[srcx_16 >> 16]));
					srcx_16 += xrIntFloat_16;
					m0 = _m_punpckldq(m0, _m_from_int(*(int*)(&pSrcLine[srcx_16 >> 16])));
					srcx_16 += xrIntFloat_16;
					__m64 m1 = _m_from_int(*(int*)(&pSrcLine[srcx_16 >> 16]));
					srcx_16 += xrIntFloat_16;
					m1 = _m_punpckldq(m1, _m_from_int(*(int*)(&pSrcLine[srcx_16 >> 16])));
					srcx_16 += xrIntFloat_16;
					_mm_stream_pi((__m64 *)&pDstLine[x], m0);
					_mm_stream_pi((__m64 *)&pDstLine[x + 2], m1);
				}
				for (x = for4count; x < dst_width; ++x)
				{
					pDstLine[x] = pSrcLine[srcx_16 >> 16];
					srcx_16 += xrIntFloat_16;
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
			_m_empty();
		}

		void  ImageProcess::Bilinear_Fast_MMX(Bgra32* PColor0, Bgra32* PColor1, unsigned long u_8, unsigned long v_8, Bgra32* result)
		{
			asm
			{
			MOVD      MM6, v_8
			MOVD      MM5, u_8
			mov       edx, PColor0
			mov       eax, PColor1
			PXOR      mm7, mm7

			MOVD         MM2, dword ptr[eax]
			MOVD         MM0, dword ptr[eax + 4]
			PUNPCKLWD    MM5, MM5
			PUNPCKLWD    MM6, MM6
			MOVD         MM3, dword ptr[edx]
			MOVD         MM1, dword ptr[edx + 4]
			PUNPCKLDQ    MM5, MM5
			PUNPCKLBW    MM0, MM7
			PUNPCKLBW    MM1, MM7
			PUNPCKLBW    MM2, MM7
			PUNPCKLBW    MM3, MM7
			PSUBw        MM0, MM2
			PSUBw        MM1, MM3
			PSLLw        MM2, 8
			PSLLw        MM3, 8
			PMULlw       MM0, MM5
			PMULlw       MM1, MM5
			PUNPCKLDQ    MM6, MM6
			PADDw        MM0, MM2
			PADDw        MM1, MM3

			PSRLw        MM0, 8
			PSRLw        MM1, 8
			PSUBw        MM0, MM1
			PSLLw        MM1, 8
			PMULlw       MM0, MM6
			mov       eax, result
			PADDw        MM0, MM1

			PSRLw        MM0, 8
			PACKUSwb     MM0, MM7
			movd[eax], MM0
			//emms
		}
		}

		void ImageProcess::Bilinear_Border_MMX(const Bgra32Rect& pic, const long x_16, const long y_16, Bgra32* result)
		{
			long x = (x_16 >> 16);
			long y = (y_16 >> 16);
			unsigned long u_16 = ((unsigned short)(x_16));
			unsigned long v_16 = ((unsigned short)(y_16));

			Bgra32 pixel[4];
			pixel[0] = Pixels_Bound(pic, x, y);
			pixel[1] = Pixels_Bound(pic, x + 1, y);
			pixel[2] = Pixels_Bound(pic, x, y + 1);
			pixel[3] = Pixels_Bound(pic, x + 1, y + 1);

			Bilinear_Fast_MMX(&pixel[0], &pixel[2], u_16 >> 8, v_16 >> 8, result);
		}

		void ImageProcess::ImageScaling_Bilinear_MMX_Ex(const Bgra32Rect& Dst, const Bgra32Rect& Src)
		{
			if ((0 == Dst.width) || (0 == Dst.height)
				|| (0 == Src.width) || (0 == Src.height)) return;

			long xrIntFloat_16 = ((Src.width) << 16) / Dst.width + 1;
			long yrIntFloat_16 = ((Src.height) << 16) / Dst.height + 1;
			const long csDErrorX = -(1 << 15) + (xrIntFloat_16 >> 1);
			const long csDErrorY = -(1 << 15) + (yrIntFloat_16 >> 1);

			unsigned long dst_width = Dst.width;

			long border_y0 = -csDErrorY / yrIntFloat_16 + 1;              //y0+y*yr>=0; y0=csDErrorY => y>=-csDErrorY/yr
			if (border_y0 >= Dst.height) border_y0 = Dst.height;
			long border_x0 = -csDErrorX / xrIntFloat_16 + 1;
			if (border_x0 >= Dst.width) border_x0 = Dst.width;
			long border_y1 = (((Src.height - 2) << 16) - csDErrorY) / yrIntFloat_16 + 1; //y0+y*yr<=(height-2) => y<=(height-2-csDErrorY)/yr
			if (border_y1 < border_y0) border_y1 = border_y0;
			long border_x1 = (((Src.width - 2) << 16) - csDErrorX) / xrIntFloat_16 + 1;
			if (border_x1 < border_x0) border_x1 = border_x0;

			Bgra32* pDstLine = Dst.pdata;
			long Src_byte_width = Src.byte_width;
			long srcy_16 = csDErrorY;
			long y;
			for (y = 0; y < border_y0; ++y)
			{
				long srcx_16 = csDErrorX;
				for (unsigned long x = 0; x < dst_width; ++x)
				{
					Bilinear_Border_MMX(Src, srcx_16, srcy_16, &pDstLine[x]); //border
					srcx_16 += xrIntFloat_16;
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}

			for (y = border_y0; y < border_y1; ++y)
			{
				long srcx_16 = csDErrorX;
				long x;
				for (x = 0; x<border_x0; ++x)
				{
					Bilinear_Border_MMX(Src, srcx_16, srcy_16, &pDstLine[x]);//border
					srcx_16 += xrIntFloat_16;
				}

				{
					long dst_width_fast = border_x1 - border_x0;
					if (dst_width_fast>0)
					{
						unsigned long v_8 = (srcy_16 & 0xFFFF) >> 8;
						Bgra32* PSrcLineColor = (Bgra32*)((TUInt8*)(Src.pdata) + Src_byte_width*(srcy_16 >> 16));
						Bgra32* PSrcLineColorNext = (Bgra32*)((TUInt8*)(PSrcLineColor)+Src_byte_width);
						Bgra32* pDstLine_Fast = &pDstLine[border_x0];
						asm
						{
							movd         mm6, v_8
							pxor         mm7, mm7 //mm7=0
							PUNPCKLWD    MM6, MM6
							PUNPCKLDQ    MM6, MM6//mm6=v_8

							mov       esi, PSrcLineColor
							mov       ecx, PSrcLineColorNext
							mov       edx, srcx_16
							mov       ebx, dst_width_fast
							mov       edi, pDstLine_Fast
							lea       edi, [edi + ebx * 4]
							push      ebp
							mov       ebp, xrIntFloat_16
							neg       ebx

						loop_start :

							mov       eax, edx
								shl       eax, 16
								shr       eax, 24
								//== movzx       eax,dh  //eax=u_8
								MOVD      MM5, eax
								mov       eax, edx
								shr       eax, 16     //srcx_16>>16

								MOVD         MM2, dword ptr[ecx + eax * 4]
								MOVD         MM0, dword ptr[ecx + eax * 4 + 4]
								PUNPCKLWD    MM5, MM5
								MOVD         MM3, dword ptr[esi + eax * 4]
								MOVD         MM1, dword ptr[esi + eax * 4 + 4]
								PUNPCKLDQ    MM5, MM5 //mm5=u_8
								PUNPCKLBW    MM0, MM7
								PUNPCKLBW    MM1, MM7
								PUNPCKLBW    MM2, MM7
								PUNPCKLBW    MM3, MM7
								PSUBw        MM0, MM2
								PSUBw        MM1, MM3
								PSLLw        MM2, 8
								PSLLw        MM3, 8
								PMULlw       MM0, MM5
								PMULlw       MM1, MM5
								PADDw        MM0, MM2
								PADDw        MM1, MM3

								PSRLw        MM0, 8
								PSRLw        MM1, 8
								PSUBw        MM0, MM1
								PSLLw        MM1, 8
								PMULlw       MM0, MM6
								PADDw        MM0, MM1

								PSRLw     MM0, 8
								PACKUSwb  MM0, MM7
								MOVd   dword ptr[edi + ebx * 4], MM0 //write DstColor

								add       edx, ebp //srcx_16+=xrIntFloat_16
								inc       ebx
								jnz       loop_start

								pop       ebp
								mov       srcx_16, edx
						}
					}
				}

				for (x = border_x1; x < dst_width; ++x)
				{
					Bilinear_Border_MMX(Src, srcx_16, srcy_16, &pDstLine[x]);//border
					srcx_16 += xrIntFloat_16;
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
			for (y = border_y1; y < Dst.height; ++y)
			{
				long srcx_16 = csDErrorX;
				for (unsigned long x = 0; x < dst_width; ++x)
				{
					Bilinear_Border_MMX(Src, srcx_16, srcy_16, &pDstLine[x]); //border
					srcx_16 += xrIntFloat_16;
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
			asm emms
		}
		
		inline double SinXDivX(double x)
		{
			const float a = -1; //a???? a=-2,-1,-0.75,-0.5??,??????????????

			if (x < 0) x = -x; //x=abs(x);
			double x2 = x*x;
			double x3 = x2*x;
			if (x <= 1)
				return (a + 2)*x3 - (a + 3)*x2 + 1;
			else if (x <= 2)
				return a*x3 - (5 * a)*x2 + (8 * a)*x - (4 * a);
			else
				return 0;
		}
		inline TUInt8 border_color(long Color)
		{
			if (Color <= 0)
				return 0;
			else if (Color >= 255)
				return 255;
			else
				return Color;
		}
		static TMMXData32 SinXDivX_Table_MMX[(2 << 8) + 1];
		class _CAutoInti_SinXDivX_Table_MMX {
		private:
			void _Inti_SinXDivX_Table_MMX()
			{
				for (long i = 0; i <= (2 << 8); ++i)
				{
					unsigned short t = long(0.5 + (1 << 14)*SinXDivX(i*(1.0 / (256))));
					unsigned long tl = t | (((unsigned long)t) << 16);
					SinXDivX_Table_MMX[i] = tl;
				}
			};
		public:
			_CAutoInti_SinXDivX_Table_MMX() { _Inti_SinXDivX_Table_MMX(); }
		};
		static _CAutoInti_SinXDivX_Table_MMX __tmp_CAutoInti_SinXDivX_Table_MMX;

		static TMMXData64 SinXDivX_Table64_MMX[(2 << 8) + 1];
		class _CAutoInti_SinXDivX_Table64_MMX {
		private:
			void _Inti_SinXDivX_Table64_MMX()
			{
				for (long i = 0; i <= (2 << 8); ++i)
				{
					unsigned short t = (unsigned short)(0.5 + (1 << 14)*SinXDivX(i*(1.0 / (256))));
					unsigned long tl = t | (((unsigned long)t) << 16);
					TMMXData64 tll = tl | (((TMMXData64)tl) << 32);
					SinXDivX_Table64_MMX[i] = tll;
				}
			};
		public:
			_CAutoInti_SinXDivX_Table64_MMX() { _Inti_SinXDivX_Table64_MMX(); }
		};
		static _CAutoInti_SinXDivX_Table64_MMX __tmp_CAutoInti_SinXDivX_Table64_MMX;
		
		void __declspec(naked)  _private_ThreeOrderTable_Fast_SSE2_2(){
			// #define  _private_ThreeOrderTable_Fast_SSE2_2() /
			asm movq        xmm0, qword ptr[eax]
				asm movq        xmm1, qword ptr[eax + 8]
				asm movq        xmm2, qword ptr[eax + edx]
				asm movq        xmm3, qword ptr[eax + edx + 8]
				asm punpcklbw   xmm0, xmm7
				asm punpcklbw   xmm1, xmm7
				asm punpcklbw   xmm2, xmm7
				asm punpcklbw   xmm3, xmm7
				asm psllw       xmm0, 7
				asm psllw       xmm1, 7
				asm psllw       xmm2, 7
				asm psllw       xmm3, 7
				asm pmulhw      xmm0, xmmword ptr[ecx]
				asm pmulhw      xmm1, xmmword ptr[ecx + 16]
				asm pmulhw      xmm2, xmmword ptr[ecx]
				asm pmulhw      xmm3, xmmword ptr[ecx + 16]
				asm paddsw      xmm0, xmm1
				asm paddsw      xmm2, xmm3
				asm pmulhw      xmm0, xmmword ptr[ebx]
				asm pmulhw      xmm2, xmmword ptr[ebx + 16]
				asm paddsw      xmm0, xmm2
				asm ret //for __declspec(naked)
		}

		void __declspec(naked) _private_ThreeOrder_Fast_MMX()
		{
			asm
			{
			movd        mm1, dword ptr[edx]
			movd        mm2, dword ptr[edx + 4]
			movd        mm3, dword ptr[edx + 8]
			movd        mm4, dword ptr[edx + 12]
			movd        mm5, dword ptr[(offset SinXDivX_Table_MMX) + 256 * 4 + eax * 4]
			movd        mm6, dword ptr[(offset SinXDivX_Table_MMX) + eax * 4]
			punpcklbw   mm1, mm7
			punpcklbw   mm2, mm7
			punpcklwd   mm5, mm5
			punpcklwd   mm6, mm6
			psllw       mm1, 7
			psllw       mm2, 7
			pmulhw      mm1, mm5
			pmulhw      mm2, mm6
			punpcklbw   mm3, mm7
			punpcklbw   mm4, mm7
			movd        mm5, dword ptr[(offset SinXDivX_Table_MMX) + 256 * 4 + ecx * 4]
			movd        mm6, dword ptr[(offset SinXDivX_Table_MMX) + 512 * 4 + ecx * 4]
			punpcklwd   mm5, mm5
			punpcklwd   mm6, mm6
			psllw       mm3, 7
			psllw       mm4, 7
			pmulhw      mm3, mm5
			pmulhw      mm4, mm6
			paddsw      mm1, mm2
			paddsw      mm3, mm4
			movd        mm6, dword ptr[ebx] //v
			paddsw      mm1, mm3
			punpcklwd   mm6, mm6

			pmulhw      mm1, mm6
			add     edx, esi  //+pic.byte_width
			paddsw      mm0, mm1

			ret
		}
		}

		inline uint32 ThreeOrderTable_Fast_SSE2(const Bgra32* pixel, long byte_width, const TMMXData64* v4, const TMMXData64* u4){
			asm mov     eax, pixel
				asm mov     edx, byte_width
				asm mov     ebx, v4
				asm mov     ecx, u4
				asm call _private_ThreeOrderTable_Fast_SSE2_2
				//_private_ThreeOrderTable_Fast_SSE2_2()
				asm movdqa  xmm6, xmm0
				asm lea     eax, [eax + edx * 2]  //+pic.byte_width
				asm lea     ebx, [ebx + 32]
				asm call _private_ThreeOrderTable_Fast_SSE2_2
				// _private_ThreeOrderTable_Fast_SSE2_2()
				asm paddsw      xmm6, xmm0
				asm movdqa      xmm5, xmm6
				asm psrldq      xmm6, 8   //srl 8*8 bit!
				asm paddsw      xmm5, xmm6
				asm psraw       xmm5, 3
				asm packuswb    xmm5, xmm7
				asm movd        eax, xmm5
		}
		inline long getSizeBorder(long x, long maxx){
			if (x <= 0)
				return 0;
			else if (x >= maxx)
				return maxx;
			else
				return x;
		}
		inline uint32 ThreeOrderTable_Border_SSE2(const Bgra32Rect& pic, const long x0_sub1, const long y0_sub1, const TMMXData64* v4, const TMMXData64* u4){
			Bgra32 pixel[16];
			long height_sub_1 = pic.height - 1;
			long width_sub_1 = pic.width - 1;
			Bgra32* pbuf = pixel;
			for (long i = 0; i < 4; ++i, pbuf += 4){
				long y = getSizeBorder(y0_sub1 + i, height_sub_1);
				Bgra32* pLine = pic.getLinePixels(y);
				pbuf[0] = pLine[getSizeBorder(x0_sub1 + 0, width_sub_1)];
				pbuf[1] = pLine[getSizeBorder(x0_sub1 + 1, width_sub_1)];
				pbuf[2] = pLine[getSizeBorder(x0_sub1 + 2, width_sub_1)];
				pbuf[3] = pLine[getSizeBorder(x0_sub1 + 3, width_sub_1)];
			}
			return ThreeOrderTable_Fast_SSE2(pixel, 4 * sizeof(Bgra32), v4, u4);
		}
		inline void ThreeOrder_Fast_MMX(const Bgra32Rect& pic, const long x_16, const long y_16, Bgra32* result)
		{
			asm
			{
			mov     ecx, pic
			mov     eax, y_16
			mov     ebx, x_16
			movzx   edi, ah //v_8
			mov     edx, [ecx + Bgra32Rect::pdata]
			shr     eax, 16
			mov     esi, [ecx + Bgra32Rect::byte_width]
			dec     eax
			movzx   ecx, bh //u_8
			shr     ebx, 16
			imul    eax, esi
			lea     edx, [edx + ebx * 4 - 4]
			add     edx, eax //pixel

			mov     eax, ecx
			neg     ecx

			pxor    mm7, mm7  //0
			//mov     edx,pixel
			pxor    mm0, mm0  //result=0
			//lea     eax,auv_7

			lea    ebx, [(offset SinXDivX_Table_MMX) + 256 * 4 + edi * 4]
			call  _private_ThreeOrder_Fast_MMX
			lea    ebx, [(offset SinXDivX_Table_MMX) + edi * 4]
			call  _private_ThreeOrder_Fast_MMX
			neg    edi
			lea    ebx, [(offset SinXDivX_Table_MMX) + 256 * 4 + edi * 4]
			call  _private_ThreeOrder_Fast_MMX
			lea    ebx, [(offset SinXDivX_Table_MMX) + 512 * 4 + edi * 4]
			call  _private_ThreeOrder_Fast_MMX

			psraw     mm0, 3
			mov       eax, result
			packuswb  mm0, mm7
			movd[eax], mm0
			//emms
		}
		}

		void ImageProcess::ImageScaling_ThreeOrderTable_SSE2(const Bgra32Rect& Dst, const Bgra32Rect& Src)
		{
			if ((0 == Dst.width) || (0 == Dst.height)
				|| (0 == Src.width) || (0 == Src.height)) return;
			long dst_width = Dst.width;
			long dst_height = Dst.height;
			long xrIntFloat_16 = ((Src.width) << 16) / dst_width + 1;
			long yrIntFloat_16 = ((Src.height) << 16) / dst_height + 1;
			const long csDErrorX = -(1 << 15) + (xrIntFloat_16 >> 1);
			const long csDErrorY = -(1 << 15) + (yrIntFloat_16 >> 1);
			long border_y0 = ((1 << 16) - csDErrorY) / yrIntFloat_16 + 1;//y0+y*yr>=1; y0=csDErrorY => y>=(1-csDErrorY)/yr
			if (border_y0 >= dst_height) border_y0 = dst_height;
			long border_x0 = ((1 << 16) - csDErrorX) / xrIntFloat_16 + 1;
			if (border_x0 >= dst_width) border_x0 = dst_width;
			long border_y1 = (((Src.height - 3) << 16) - csDErrorY) / yrIntFloat_16 + 1; //y0+y*yr<=(height-3) => y<=(height-3-csDErrorY)/yr
			if (border_y1 < border_y0) border_y1 = border_y0;
			long border_x1 = (((Src.width - 3) << 16) - csDErrorX) / xrIntFloat_16 + 1;;
			if (border_x1 < border_x0) border_x1 = border_x0;
			TUInt8* _bufMem = new TUInt8[(dst_width * 4 * sizeof(TMMXData64)+15) + dst_width*sizeof(Int32)];
			TMMXData64* uList = (TMMXData64*)((((ptrdiff_t)_bufMem) + 15) >> 4 << 4);
			Int32* xList = (Int32*)(uList + dst_width * 4);
			{//init u table
				long srcx_16 = csDErrorX;
				for (long x = 0; x < dst_width * 4; x += 4){
					xList[x >> 2] = (srcx_16 >> 16) - 1;
					long u = (srcx_16 >> 8) & 0xFF;
					uList[x + 0] = SinXDivX_Table64_MMX[256 + u];
					uList[x + 1] = SinXDivX_Table64_MMX[u];
					uList[x + 2] = SinXDivX_Table64_MMX[256 - u];
					uList[x + 3] = SinXDivX_Table64_MMX[512 - u];
					srcx_16 += xrIntFloat_16;
				}
			}
			TMMXData64 _v4[8 + 2];
			TMMXData64* v4 = (&_v4[0]); v4 = (TMMXData64*)((((ptrdiff_t)v4) + 15) >> 4 << 4);
			asm pxor    xmm7, xmm7
				Bgra32* pDstLine = Dst.pdata;
			long srcy_16 = csDErrorY;
			for (long y = 0; y < dst_height; ++y){
				//v table
				const long srcy_sub1 = (srcy_16 >> 16) - 1;
				const long v = (srcy_16 >> 8) & 0xFF;
				v4[0] = SinXDivX_Table64_MMX[256 + v];
				v4[1] = v4[0];
				v4[2] = SinXDivX_Table64_MMX[v];
				v4[3] = v4[2];
				v4[4] = SinXDivX_Table64_MMX[256 - v];
				v4[5] = v4[4];
				v4[6] = SinXDivX_Table64_MMX[512 - v];
				v4[7] = v4[6];
				if ((y < border_y0) || (y >= border_y1)){
					for (long x = 0; x < dst_width; ++x)
						pDstLine[x].argb = ThreeOrderTable_Border_SSE2(Src, xList[x], srcy_sub1, v4, &uList[x * 4]); //border
				}
				else{
					for (long x = 0; x < border_x0; ++x)
						pDstLine[x].argb = ThreeOrderTable_Border_SSE2(Src, xList[x], srcy_sub1, v4, &uList[x * 4]);//border
					const Bgra32* pixelLine = Src.getLinePixels(srcy_sub1);
					long byte_width = Src.byte_width;
					for (long x = border_x0; x < border_x1; ++x)
						pDstLine[x].argb = ThreeOrderTable_Fast_SSE2(&pixelLine[xList[x]], byte_width, v4, &uList[x * 4]);//fast MMX !
					for (long x = border_x1; x < dst_width; ++x)
						pDstLine[x].argb = ThreeOrderTable_Border_SSE2(Src, xList[x], srcy_sub1, v4, &uList[x * 4]);//border
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
			delete[]_bufMem;
		}

		
		void ImageProcess::ThreeOrder_Border_MMX(const Bgra32Rect& pic, const long x_16, const long y_16, Bgra32* result)
		{
			unsigned long x0_sub1 = (x_16 >> 16) - 1;
			unsigned long y0_sub1 = (y_16 >> 16) - 1;
			long u_16_add1 = ((unsigned short)(x_16)) + (1 << 16);
			long v_16_add1 = ((unsigned short)(y_16)) + (1 << 16);

			Bgra32 pixel[16];

			for (long i = 0; i < 4; ++i)
			{
				long y = y0_sub1 + i;
				pixel[i * 4 + 0] = Pixels_Bound(pic, x0_sub1, y);
				pixel[i * 4 + 1] = Pixels_Bound(pic, x0_sub1 + 1, y);
				pixel[i * 4 + 2] = Pixels_Bound(pic, x0_sub1 + 2, y);
				pixel[i * 4 + 3] = Pixels_Bound(pic, x0_sub1 + 3, y);
			}

			Bgra32Rect npic;
			npic.pdata = &pixel[0];
			npic.byte_width = 4 * sizeof(Bgra32);
			//npic.width     =4;
			//npic.height    =4;
			ThreeOrder_Fast_MMX(npic, u_16_add1, v_16_add1, result);
		}

		void ImageProcess::ImageScaling_ThreeOrder_MMX(const Bgra32Rect& Dst, const Bgra32Rect& Src)
		{
			if ((0 == Dst.width) || (0 == Dst.height)
				|| (0 == Src.width) || (0 == Src.height)) return;

			long xrIntFloat_16 = ((Src.width) << 16) / Dst.width + 1;
			long yrIntFloat_16 = ((Src.height) << 16) / Dst.height + 1;
			const long csDErrorX = -(1 << 15) + (xrIntFloat_16 >> 1);
			const long csDErrorY = -(1 << 15) + (yrIntFloat_16 >> 1);

			unsigned long dst_width = Dst.width;

			long border_y0 = ((1 << 16) - csDErrorY) / yrIntFloat_16 + 1;//y0+y*yr>=1; y0=csDErrorY => y>=(1-csDErrorY)/yr
			if (border_y0 >= Dst.height) border_y0 = Dst.height;
			long border_x0 = ((1 << 16) - csDErrorX) / xrIntFloat_16 + 1;
			if (border_x0 >= Dst.width) border_x0 = Dst.width;
			long border_y1 = (((Src.height - 3) << 16) - csDErrorY) / yrIntFloat_16 + 1; //y0+y*yr<=(height-3) => y<=(height-3-csDErrorY)/yr
			if (border_y1 < border_y0) border_y1 = border_y0;
			long border_x1 = (((Src.width - 3) << 16) - csDErrorX) / xrIntFloat_16 + 1;;
			if (border_x1 < border_x0) border_x1 = border_x0;

			Bgra32* pDstLine = Dst.pdata;
			long srcy_16 = csDErrorY;
			long y;
			for (y = 0; y < border_y0; ++y)
			{
				long srcx_16 = csDErrorX;
				for (unsigned long x = 0; x < dst_width; ++x)
				{
					ThreeOrder_Border_MMX(Src, srcx_16, srcy_16, &pDstLine[x]); //border
					srcx_16 += xrIntFloat_16;
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
			for (y = border_y0; y < border_y1; ++y)
			{
				long srcx_16 = csDErrorX;
				long x;
				for (x = 0; x < border_x0; ++x)
				{
					ThreeOrder_Border_MMX(Src, srcx_16, srcy_16, &pDstLine[x]);//border
					srcx_16 += xrIntFloat_16;
				}
				for (x = border_x0; x < border_x1; ++x)
				{
					ThreeOrder_Fast_MMX(Src, srcx_16, srcy_16, &pDstLine[x]);//fast MMX !
					srcx_16 += xrIntFloat_16;
				}
				for (x = border_x1; x < dst_width; ++x)
				{
					ThreeOrder_Border_MMX(Src, srcx_16, srcy_16, &pDstLine[x]);//border
					srcx_16 += xrIntFloat_16;
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
			for (y = border_y1; y < Dst.height; ++y)
			{
				long srcx_16 = csDErrorX;
				for (unsigned long x = 0; x < dst_width; ++x)
				{
					ThreeOrder_Border_MMX(Src, srcx_16, srcy_16, &pDstLine[x]); //border
					srcx_16 += xrIntFloat_16;
				}
				srcy_16 += yrIntFloat_16;
				((TUInt8*&)pDstLine) += Dst.byte_width;
			}
			asm emms
		}
		
		static unsigned char aucClipTable[2048 * 2];
		static unsigned char * paucClip = aucClipTable + 2048;

		class _aucClip_Table {
		private:
			void _Inti_aucClip_Table()
			{
				for (int i = -2048; i < 2048; i++)
				{
					paucClip[i] = i < 0 ? 0 : (unsigned char)((i > 255) ? 255 : i);
				}
			};
		public:
			_aucClip_Table() { _Inti_aucClip_Table(); }
		};
		static _aucClip_Table __tmp__aucClip_Table;

		static inline unsigned char ToUChar(int iValue) { return paucClip[iValue]; }

		void ImageProcess::YUV420ToBGRA32_SSE(
			unsigned char * pRgb, unsigned int uRgbStride,
			unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int uYStride, unsigned int uUStride, unsigned int uVStride,
			unsigned int uWidth, unsigned int uHeight,
			bool bSmooth)
		{
			__m128i		unpack_out[4];
			__m128i		convert_out[6];
			__m128i*    y_line1 = (__m128i *) pY;
			__m128i*    y_line2 = (__m128i*)((uint8_t *)y_line1 + uYStride);
			__m128i*    u_plane = (__m128i*)pU;
			__m128i*    v_plane = (__m128i*)pV;
			__m128i*	rgb_out_line1 = (__m128i *) pRgb;
			__m128i*	rgb_out_line2 = (__m128i *) ((uint8_t *)rgb_out_line1 + uRgbStride);
			uint32_t	pixels_remaining_on_line = uWidth;
			uint32_t	lines_remaining = uHeight;

			if (pixels_remaining_on_line % 16 != 0)
			{
				YUV420ToBGRA32(pRgb, uRgbStride, pY, pU, pV, uYStride, uUStride, uVStride, uWidth, uHeight, bSmooth);
				return;
			}

			while (lines_remaining > 0)
			{
				__m128i* y1 = y_line1;
				__m128i* y2 = y_line2;
				__m128i* u = u_plane;
				__m128i* v = v_plane;
				__m128i* rgb1 = rgb_out_line1;
				__m128i* rgb2 = rgb_out_line2;

				while (pixels_remaining_on_line > 0)
				{
					unpack_yuv42Xp_to_2_y_vectors_sse2(y_line1, unpack_out, &unpack_out[2]);
					y_line1++;

					unpack_low_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);

					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(unpack_out, convert_out);

					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(&unpack_out[2], &convert_out[3]);

					pack_6_rgb_vectors_in_4_bgra_vectors_sse2(convert_out, rgb_out_line1);

					rgb_out_line1 += 4;
					unpack_yuv42Xp_to_2_y_vectors_sse2(y_line2, unpack_out, &unpack_out[2]);

					y_line2++;
					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(unpack_out, convert_out);

					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(&unpack_out[2], &convert_out[3]);

					pack_6_rgb_vectors_in_4_bgra_vectors_sse2(convert_out, rgb_out_line2);

					rgb_out_line2 += 4;

					unpack_yuv42Xp_to_2_y_vectors_sse2(y_line1, unpack_out, &unpack_out[2]);
					y_line1++;

					unpack_high_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);

					u_plane++;
					v_plane++;
					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(unpack_out, convert_out);

					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(&unpack_out[2], &convert_out[3]);

					pack_6_rgb_vectors_in_4_bgra_vectors_sse2(convert_out, rgb_out_line1);

					rgb_out_line1 += 4;
					unpack_yuv42Xp_to_2_y_vectors_sse2(y_line2, unpack_out, &unpack_out[2]);

					y_line2++;
					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(unpack_out, convert_out);

					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(&unpack_out[2], &convert_out[3]);

					pack_6_rgb_vectors_in_4_bgra_vectors_sse2(convert_out, rgb_out_line2);

					rgb_out_line2 += 4;
					pixels_remaining_on_line -= 32;

					if (pixels_remaining_on_line == 16)
					{
						y_line1--;
						y_line2--;
						rgb_out_line1 -= 4;
						rgb_out_line2 -= 4;
						u_plane = (__m128i*)((uint8_t *)u_plane - 8);
						v_plane = (__m128i*)((uint8_t *)v_plane - 8);
						pixels_remaining_on_line += 16;
					}
				}
				y_line1 = (__m128i*)((uint8_t *)y1 + 2 * uYStride);
				y_line2 = (__m128i*)((uint8_t *)y2 + 2 * uYStride);
				rgb_out_line1 = (__m128i *) ((uint8_t *)rgb1 + 2 * uRgbStride);
				rgb_out_line2 = (__m128i *) ((uint8_t *)rgb2 + 2 * uRgbStride);
				u_plane = (__m128i*)((uint8_t *)u + uUStride);
				v_plane = (__m128i*)((uint8_t *)v + uVStride);
				lines_remaining -= 2;
				pixels_remaining_on_line = uWidth;
			}
		}

		void ImageProcess::NV12ToBGRA32_SSE(
			unsigned char * pRgb, unsigned int uRgbStride,
			unsigned char* pY, unsigned char* pUV, unsigned int uYStride, unsigned int uUVStride,
			unsigned int uWidth, unsigned int uHeight)
		{
			__m128i		unpack_out[4];
			__m128i		convert_out[6];
			__m128i*    y_line1 = (__m128i *) pY;
			__m128i*    y_line2 = (__m128i*)((uint8_t *)y_line1 + uYStride);
			__m128i*    uv_plane = (__m128i*)pUV;
			__m128i*	rgb_out_line1 = (__m128i *) pRgb;
			__m128i*	rgb_out_line2 = (__m128i *) ((uint8_t *)rgb_out_line1 + uRgbStride);
			uint32_t	pixels_remaining_on_line = uWidth;
			uint32_t	lines_remaining = uHeight;

			if (pixels_remaining_on_line % 16 != 0)
			{
				NV12ToBGRA32(pRgb, uRgbStride, pY, pUV, uYStride, uUVStride, uWidth, uHeight);
				return;
			}

			while (lines_remaining > 0)
			{
				__m128i* y1 = y_line1;
				__m128i* y2 = y_line2;
				__m128i* uv = uv_plane;
				__m128i* rgb1 = rgb_out_line1;
				__m128i* rgb2 = rgb_out_line2;

				while (pixels_remaining_on_line > 0)
				{
					unpack_yuv42Xp_to_2_y_vectors_sse2(y_line1, unpack_out, &unpack_out[2]);
					y_line1++;

					unpack_nv12_to_uv_vector_sse2(uv_plane, &unpack_out[1], &unpack_out[3]);
					uv_plane++;

					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(unpack_out, convert_out);

					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(&unpack_out[2], &convert_out[3]);

					pack_6_rgb_vectors_in_4_bgra_vectors_sse2(convert_out, rgb_out_line1);

					rgb_out_line1 += 4;
					unpack_yuv42Xp_to_2_y_vectors_sse2(y_line2, unpack_out, &unpack_out[2]);

					y_line2++;
					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(unpack_out, convert_out);

					nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(&unpack_out[2], &convert_out[3]);

					pack_6_rgb_vectors_in_4_bgra_vectors_sse2(convert_out, rgb_out_line2);

					rgb_out_line2 += 4;

					pixels_remaining_on_line -= 16;
				}
				y_line1 = (__m128i*)((uint8_t *)y1 + 2 * uYStride);
				y_line2 = (__m128i*)((uint8_t *)y2 + 2 * uYStride);
				rgb_out_line1 = (__m128i *) ((uint8_t *)rgb1 + 2 * uRgbStride);
				rgb_out_line2 = (__m128i *) ((uint8_t *)rgb2 + 2 * uRgbStride);
				uv_plane = (__m128i*)((uint8_t *)uv + uUVStride);
				lines_remaining -= 2;
				pixels_remaining_on_line = uWidth;
			}
		}

		void ImageProcess::NV12ToRGB(
			unsigned char * pRgb, unsigned int uRgbStride, int iRgbComponent,
			unsigned char* pY, unsigned char* pUV, unsigned int uYStride, unsigned int uUVStride,
			unsigned int uWidth, unsigned int uHeight, RgbExtraOperation rgbOpr)
		{
			if ((uWidth & 1) == 0 && (uHeight & 1) == 0)
			{
				int iRCommon, iGCommon, iBCommon;
				int iY, iU, iV;
				int offsetRGB = uRgbStride * 2 - uWidth * iRgbComponent;
				int offsetY = uYStride * 2 - uWidth;
				int offsetUV = uUVStride - uWidth;
				unsigned char* pRGB1 = pRgb;
				unsigned char* pRGB2 = pRgb + uRgbStride;
				unsigned char* pYY = pY + uYStride;

				int width = uWidth / 2;
				int height = uHeight / 2;

				for (unsigned int i = 0; i < height; i++)
				{
					for (unsigned int j = 0; j < width; j++)
					{
						iU = (*pUV++);
						iV = (*pUV++);
						iU -= 128;
						iV -= 128;

						iBCommon = (116130 * iU + 32768) >> 16;
						iGCommon = (-22554 * iU - 46802 * iV + 32768) >> 16;
						iRCommon = (91881 * iV + 32768) >> 16;

						//PIXEL  top left
						iY = *pY++;
						*pRGB1++ = ToUChar(iY + iBCommon);
						*pRGB1++ = ToUChar(iY + iGCommon);
						*pRGB1++ = ToUChar(iY + iRCommon);
						rgbOpr(pRGB1);
						//PIXEL top right
						iY = *pY++;
						*pRGB1++ = ToUChar(iY + iBCommon);
						*pRGB1++ = ToUChar(iY + iGCommon);
						*pRGB1++ = ToUChar(iY + iRCommon);
						rgbOpr(pRGB1);

						//PIXEL bottom left
						iY = *pYY++;
						*pRGB2++ = ToUChar(iY + iBCommon);
						*pRGB2++ = ToUChar(iY + iGCommon);
						*pRGB2++ = ToUChar(iY + iRCommon);
						rgbOpr(pRGB2);

						//PIXEL  bottom right
						iY = *pYY++;
						*pRGB2++ = ToUChar(iY + iBCommon);
						*pRGB2++ = ToUChar(iY + iGCommon);
						*pRGB2++ = ToUChar(iY + iRCommon);
						rgbOpr(pRGB2);
					}

					pY += offsetY;
					pYY += offsetY;
					pUV += offsetUV;
					pRGB1 += offsetRGB;
					pRGB2 += offsetRGB;
				}
			}
			else
			{
				int iY, iU, iV;
				int iRgbOffset = uRgbStride - iRgbComponent * uWidth;
				int iYOffset = uYStride - uWidth;
				int iUVOffset1 = uUVStride - uWidth;
				int iUVOffset2 = -(int)uWidth;
				for (unsigned int i = 0; i < uHeight; i++)
				{
					for (unsigned int j = 0; j < uWidth; j++)
					{
						iY = *pY++;
						iU = pUV[0];
						iV = pUV[1];
						iU -= 128;
						iV -= 128;

						*pRgb++ = ToUChar(iY + ((116130 * iU + 32768) >> 16));
						*pRgb++ = ToUChar(iY + ((-22554 * iU - 46802 * iV + 32768) >> 16));
						*pRgb++ = ToUChar(iY + ((91881 * iV + 32768) >> 16));
						rgbOpr(pRgb);

						pUV += (j & 1) << 1;
					}
					pY += iYOffset;
					pRgb += iRgbOffset;

					if (i & 1)
					{
						pUV += iUVOffset1;
					}
					else
					{
						pUV += iUVOffset2;
					}
				}
			}
		}

		void ImageProcess::NV12ToBGRA32(
			unsigned char * pRgb, unsigned int uRgbStride,
			unsigned char* pY, unsigned char* pUV, unsigned int uYStride, unsigned int uUVStride,
			unsigned int uWidth, unsigned int uHeight)
		{
			NV12ToRGB(pRgb, uRgbStride, 4, pY, pUV, uYStride, uUVStride, uWidth, uHeight, RgbExtraOperationAsign);
		}

		void ImageProcess::YUV420ToBGRA32(
			unsigned char * pRgb, unsigned int uRgbStride,
			unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int uYStride, unsigned int uUStride, unsigned int uVStride,
			unsigned int uWidth, unsigned int uHeight,
			bool bSmooth)
		{
			if (!bSmooth)
			{
				{
					unsigned int uHalfWidth = (uWidth >> 1);
					int iY, iU, iV;
					int iRgbOffset = uRgbStride - 4 * uWidth;
					int iYOffset = uYStride - uWidth;
					int iUOffset1 = uUStride - uHalfWidth;
					int iUOffset2 = -(int)uHalfWidth;
					int iVOffset1 = uVStride - uHalfWidth;
					int iVOffset2 = iUOffset2;

					for (unsigned int i = 0; i < uHeight; i++)
					{
						for (unsigned int j = 0; j < uWidth; j++)
						{
							iY = *pY++;
							iU = (*pU);
							iV = (*pV);
							iU -= 128;
							iV -= 128;

							*pRgb++ = ToUChar(iY + ((116130 * iU + 32768) >> 16));
							*pRgb++ = ToUChar(iY + ((-22554 * iU - 46802 * iV + 32768) >> 16));
							*pRgb++ = ToUChar(iY + ((91881 * iV + 32768) >> 16));
							*pRgb++ = 0;

							pU += (j & 1);
							pV += (j & 1);
						}
						pY += iYOffset;
						pRgb += iRgbOffset;

						if (i & 1)
						{
							pU += iUOffset1;
							pV += iVOffset1;
						}
						else
						{
							pU += iUOffset2;
							pV += iVOffset2;
						}
					}
				}
			}
			else
			{
				int iY = *pY;
				int iU = (*pU);
				int iV = (*pV);
				iU -= 128;
				iV -= 128;

				unsigned char aRgb[4];
				aRgb[0] = ToUChar(iY + ((116130 * iU + 32768) >> 16));
				aRgb[1] = ToUChar(iY + ((-22554 * iU - 46802 * iV + 32768) >> 16));
				aRgb[2] = ToUChar(iY + ((91881 * iV + 32768) >> 16));
				aRgb[3] = 0;

				unsigned char * pRgbOld = pRgb;
				// set the first line
				for (unsigned int j = 0; j < uWidth; j++)
				{
					memcpy(pRgb, &aRgb[0], 4 * sizeof(unsigned char));
					pRgb += 4;
				}
				//set the other uHeight-1 line
				pRgb = pRgbOld + uRgbStride;

				for (unsigned int j = 1; j < uHeight; j++)
				{
					memcpy(pRgb, pRgbOld, uWidth * 4 * sizeof(unsigned char));
					pRgb += uRgbStride;
				}
			}
		}

		void ImageProcess::CopyYUV420ToBGRA32(
			unsigned char* pY, unsigned char* pU, unsigned char* pV,
			unsigned int uYStride, unsigned int uUStride, unsigned int uVStride, unsigned int uSrcWidth, unsigned int uSrcHeight,
			unsigned char* pRgb, unsigned int uRgbStride, unsigned int uDstWidth, unsigned int uDstHeight, ImageScalingMethod method)
		{
			bool bScaled = (uSrcWidth != uDstWidth) || (uSrcHeight != uDstHeight);

			if (bScaled)
			{
				unsigned char* pScaledBGRA = new unsigned char[uSrcWidth*uSrcHeight * 4];

				YUV420ToBGRA32_SSE(pScaledBGRA, uSrcWidth * 4, pY, pU, pV, uYStride, uUStride, uVStride, uSrcWidth, uSrcHeight, false);

				Bgra32Rect src, dst;
				src.pdata = (Bgra32*)pScaledBGRA;
				src.byte_width = uSrcWidth * 4;
				src.width = uSrcWidth;
				src.height = uSrcHeight;
				dst.pdata = (Bgra32*)pRgb;
				dst.byte_width = uRgbStride;
				dst.width = uDstWidth;
				dst.height = uDstHeight;

				ImageScaling(dst, src, method);

				delete[] pScaledBGRA;
			}
			else
			{
				YUV420ToBGRA32_SSE(pRgb, uRgbStride, pY, pU, pV, uYStride, uUStride, uVStride, uDstWidth, uDstHeight, false);
			}
		}

		void ImageProcess::CopyNV12ToBGRA32(
			unsigned char* pY, unsigned char* pUV, unsigned int uYStride, unsigned int uUVStride, unsigned int uSrcWidth, unsigned int uSrcHeight,
			unsigned char* pRgb, unsigned int uRgbStride, unsigned int uDstWidth, unsigned int uDstHeight, ImageScalingMethod method)
		{
			bool bScaled = (uSrcWidth != uDstWidth) || (uSrcHeight != uDstHeight);

			if (bScaled)
			{
				unsigned char* pScaledBGRA = new unsigned char[uSrcWidth*uSrcHeight * 4];

				NV12ToBGRA32_SSE(pScaledBGRA, uSrcWidth * 4, pY, pUV, uYStride, uUVStride, uSrcWidth, uSrcHeight);

				Bgra32Rect src, dst;
				src.pdata = (Bgra32*)pScaledBGRA;
				src.byte_width = uSrcWidth * 4;
				src.width = uSrcWidth;
				src.height = uSrcHeight;
				dst.pdata = (Bgra32*)pRgb;
				dst.byte_width = uRgbStride;
				dst.width = uDstWidth;
				dst.height = uDstHeight;

				ImageScaling(dst, src, method);

				delete[] pScaledBGRA;
			}
			else
			{
				NV12ToBGRA32_SSE(pRgb, uRgbStride, pY, pUV, uYStride, uUVStride, uDstWidth, uDstHeight);
			}
		}

	}
}
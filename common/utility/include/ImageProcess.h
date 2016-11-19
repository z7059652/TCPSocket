#pragma once 
#include "target_os.h"
#include <emmintrin.h>
#include <tmmintrin.h>
#include <stdint.h>
#include <memory.h>

namespace Titanium {
	namespace TIRA
	{
		typedef void(*RgbExtraOperation) (unsigned char*& ptr);

		__inline void RgbExtraOperationAsign(unsigned char*& ptr)
		{
			*ptr++ = 0;
		}
		__inline void RgbExtraOperationNop(unsigned char*& ptr)
		{}

#define CONST_M128I_ARRAY(var, num_elem)				__declspec(align(16)) const __int64 var[(num_elem)][2]
#define M128I_ARRAY_VAL(var, num_elem, ...)				__declspec(align(16)) __int64 var[(num_elem)][2]={__VA_ARGS__}
#define CONST_M128I(var, val64_1, val64_2)				__declspec(align(16)) static const __int64 var[] = { (val64_1), (val64_2) }
#define M128I(var, val64_1, val64_2)					__declspec(align(16)) __int64 var[] = { (val64_1), (val64_2)}
		// Use the macros below to access a variable declared with the above macros.
#define	_M(var)											(*((__m128i *)(var)))	// dereference one element
#define _AM(var)										((__m128i *)(&(var)))		// Address of one element

		// Declare a __m128i variable, and load one unaligned __m128i vector from the unaligned buffer
#define	DECLARE_VECT_N_UNALIGN_LOAD(var, unaligned_buffer_ptr)\
	__m128i var = _mm_loadu_si128(unaligned_buffer_ptr)

		// Planar YUV
#define UNALIGNED_YUVP_Y_INPUT_PREAMBLE			DECLARE_VECT_N_UNALIGN_LOAD(aligned_y_vector, y_input)
#define UNALIGNED_YUVP_UV_INPUT_PREAMBLE		DECLARE_VECT_N_UNALIGN_LOAD(aligned_u_vector, u_input); DECLARE_VECT_N_UNALIGN_LOAD(aligned_v_vector, v_input)
#define YUVP_Y_INPUT_VECT						(&aligned_y_vector)
#define YUVP_U_INPUT_VECT						(&aligned_u_vector)
#define YUVP_V_INPUT_VECT						(&aligned_v_vector)

#define M128_STORE(src, dst)					_mm_storeu_si128(&(dst), (src))

#define asm __asm
		typedef unsigned char TUInt8; // [0..255]
		typedef unsigned int uint32; // [0..255]
		typedef int Int32; // [0..255]
		typedef unsigned long long  TMMXData64;
		typedef unsigned long TMMXData32;

		enum ImageScalingMethod
		{
			Align_SSE = 0,
			Align_SSE_mmh = 1,
			Bilinear_MMX_Ex = 2,
			Bilinear_SSE2 = 3,
			BilinearTable_SSE2 = 4,
			ThreeOrder_MMX = 5,
			ThreeOrderTable_SSE2 = 6
		};

		struct Bgra32 {
			inline static uint32 packBgra32Data(unsigned long r8, unsigned long g8, unsigned long b8, unsigned long a8 = 255) { return b8 | (g8 << 8) | (r8 << 16) | (a8 << 24); }
			union {
				uint32   argb;
				struct
				{
					TUInt8  b;
					TUInt8  g;
					TUInt8  r;
					TUInt8  a;
				};

			};
			inline Bgra32(){}
			inline Bgra32(const Bgra32& Bgra32) :argb(Bgra32.argb){}
			inline explicit Bgra32(const uint32 Bgra32) :argb(Bgra32){}
			inline explicit Bgra32(unsigned long r8, unsigned long g8, unsigned long b8, unsigned long a8 = 255) :argb(packBgra32Data(r8, g8, b8, a8)){}
			inline bool operator ==(const Bgra32& Bgra32) const { return argb == Bgra32.argb; }
			inline bool operator !=(const Bgra32& Bgra32) const{ return !((*this) == Bgra32); }
		};

		struct Bgra32Rect{
		public:
			Bgra32*    pdata;
			long        byte_width;
			long        width;
			long        height;
			inline Bgra32Rect() :pdata(0), byte_width(0), width(0), height(0){}
			inline Bgra32Rect(const Bgra32Rect& ref) : pdata(ref.pdata), byte_width(ref.byte_width), width(ref.width), height(ref.height){}

			//get (x,y)  bgra 
			inline Bgra32& pixels(const long x, const long y) const { return getLinePixels(y)[x]; }
			//get the address of y th line 
			inline Bgra32* getLinePixels(const long y) const { return (Bgra32*)(((TUInt8*)pdata) + byte_width*y); }
			// whether is an empty rect
			inline bool getIsEmpty()const { return ((width <= 0) || (height <= 0)); }
			//point to the next line 
			inline void nextLine(Bgra32*& pline)const { ((TUInt8*&)pline) += byte_width; }


			//Set false when (x,y)  out of border 
			inline bool clipToBorder(long& x, long& y)const{
				bool isIn = true;
				if (x < 0) {
					isIn = false; x = 0;

				}
				else if (x >= width) {
					isIn = false; x = width - 1;

				}
				if (y < 0) {
					isIn = false; y = 0;

				}
				else if (y >= height) {
					isIn = false; y = height - 1;

				}
				return isIn;

			}
			inline Bgra32 getPixelsBorder(long x, long y) const {
				bool isInPic = clipToBorder(x, y);
				Bgra32 result = pixels(x, y);
				if (!isInPic)
					result.a = 0;
				return result;
			}
		};

		inline Bgra32& Pixels(const Bgra32Rect& pic, const long x, const long y)
		{
			return ((Bgra32*)((TUInt8*)pic.pdata + pic.byte_width*y))[x];
		}

		class ImageProcess
		{
		public:
			// rgb to yuv
			static void BGRA32ToYUV444(
				unsigned char * pRgb, unsigned int uRgbStride,
				unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int uYuvStride,
				unsigned int uWidth, unsigned int uHeight);
			static void BGRA32ToYUV444_SSE(
				unsigned char * pRgb, unsigned int uRgbStride,
				unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int uYuvStride,
				unsigned int uWidth, unsigned int uHeight);

			// yuv to rgb
			static void YUV420ToBGRA32_SSE(
				unsigned char * pRgb, unsigned int uRgbStride,
				unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int uYStride, unsigned int uUStride, unsigned int uVStride,
				unsigned int uWidth, unsigned int uHeight,
				bool bSmooth);

			static void YUV420ToBGRA32(
				unsigned char * pRgb, unsigned int uRgbStride,
				unsigned char * pY, unsigned char * pU, unsigned char * pV, unsigned int uYStride, unsigned int uUStride, unsigned int uVStride,
				unsigned int uWidth, unsigned int uHeight,
				bool bSmooth);

			static void NV12ToBGRA32_SSE(
				unsigned char * pRgb, unsigned int uRgbStride,
				unsigned char* pY, unsigned char* pUV, unsigned int uYStride, unsigned int uUVStride,
				unsigned int uWidth, unsigned int uHeight);

			static void NV12ToBGRA32(
				unsigned char * pRgb, unsigned int uRgbStride,
				unsigned char* pY, unsigned char* pUV, unsigned int uYStride, unsigned int uUVStride,
				unsigned int uWidth, unsigned int uHeight);
			static void NV12ToRGB(
				unsigned char * pRgb, unsigned int uRgbStride, int iRgbComponent,
				unsigned char* pY, unsigned char* pUV, unsigned int uYStride, unsigned int uUVStride,
				unsigned int uWidth, unsigned int uHeight, RgbExtraOperation rgbOpr);

			static void CopyYUV420ToBGRA32(
				unsigned char* pY, unsigned char* pU, unsigned char* pV,
				unsigned int uYStride, unsigned int uUStride, unsigned int uVStride, unsigned int uSrcWidth, unsigned int uSrcHeight,
				unsigned char* pRgb, unsigned int uRgbStride, unsigned int uDstWidth, unsigned int uDstHeight, ImageScalingMethod method);

			static void CopyNV12ToBGRA32(
				unsigned char* pY, unsigned char* pUV, unsigned int uYStride, unsigned int uUVStride, unsigned int uSrcWidth, unsigned int uSrcHeight,
				unsigned char* pRgb, unsigned int uRgbStride, unsigned int uDstWidth, unsigned int uDstHeight, ImageScalingMethod method);

			// scaling
			static void ImageScaling(const Bgra32Rect& Dst, const Bgra32Rect& Src, ImageScalingMethod method);

		private:
			static inline void unpack_yuv42Xp_to_2_y_vectors_sse2(__m128i* y_input, __m128i* out_1_v16i_y_vector1, __m128i* out_1_v16i_y_vector2)
			{
				CONST_M128I(zero_vect, 0x0LL, 0x0LL);
				UNALIGNED_YUVP_Y_INPUT_PREAMBLE;

				// Y1 unpacking
				*out_1_v16i_y_vector1 = _mm_unpacklo_epi8(*YUVP_Y_INPUT_VECT, _M(zero_vect));
				// Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PUNPCKLBW       1	0.5

				//
				// Y2 unpacking
				*out_1_v16i_y_vector2 = _mm_unpackhi_epi8(*YUVP_Y_INPUT_VECT, _M(zero_vect));
				// Y9  0	Y10  0	Y11  0	Y12  0	Y13  0	Y14  0	Y15  0	Y16  0		// PUNPCKHBW       1	0.5
			}

			static inline void unpack_low_yuv42Xp_to_uv_vector_sse2(__m128i* u_input, __m128i* v_input, __m128i* out_1_v16i_uv_vector1, __m128i* out_1_v16i_uv_vector2)
			{
				CONST_M128I(zero_vect, 0x0LL, 0x0LL);
				M128I(scratch, 0x0LL, 0x0LL);
				UNALIGNED_YUVP_UV_INPUT_PREAMBLE;

				_M(scratch) = _mm_unpacklo_epi8(*YUVP_U_INPUT_VECT, *YUVP_V_INPUT_VECT);
				// U1 V1	U2 V2	U3 V3	U4 V4	U5 V5	U6 V6	U7 V7	U8 V8		// PUNPCKLBW       1	0.5

				*out_1_v16i_uv_vector1 = _mm_unpacklo_epi8(_M(scratch), _M(zero_vect));
				// U1 0		V1 0	U2 0	V2 0	U3 0	V3 0	U4 0	V4	0		// PUNPCKLBW       1	0.5

				*out_1_v16i_uv_vector2 = _mm_unpackhi_epi8(_M(scratch), _M(zero_vect));
				// U5 0		V5 0	U6 0	V6 0	U7 0	V7 0	U8 0	V8	0		// PUNPCKHBW       1	0.5
			}

			static inline void unpack_nv12_to_uv_vector_sse2(__m128i* uv_input, __m128i* out_1_v16i_uv_vector1, __m128i* out_1_v16i_uv_vector2)
			{
				//_mm_unpacklo_epi8
				CONST_M128I(zero_vect, 0x0LL, 0x0LL);
				__m128i aligned_uv = _mm_loadu_si128(uv_input);

				*out_1_v16i_uv_vector1 = _mm_unpacklo_epi8(aligned_uv, _M(zero_vect));
				// U1 0		V1 0	U2 0	V2 0	U3 0	V3 0	U4 0	V4	0		// PUNPCKLBW       1	0.5

				*out_1_v16i_uv_vector2 = _mm_unpackhi_epi8(aligned_uv, _M(zero_vect));
				// U5 0		V5 0	U6 0	V6 0	U7 0	V7 0	U8 0	V8	0		// PUNPCKHBW       1	0.5
			}

			static inline void nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)
			{
				CONST_M128I(shuffMask, 0x0504050401000100LL, 0x0D0C0D0C09080908LL);
				M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
				M128I(sub16, 0x0000000000000000LL, 0x0000000000000000LL);

				M128I(uvRCoeffs, 0x00B3000000B30000LL, 0x00B3000000B30000LL);
				M128I(uvGCoeffs, 0xFFA5FFD4FFA5FFD4LL, 0xFFA5FFD4FFA5FFD4LL);
				M128I(uvBCoeffs, 0x000000E3000000E3LL, 0x000000E3000000E3LL);

				_M(sub128) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(sub128));

				_M(sub16) = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(sub16));

				_M(uvRCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvRCoeffs));
				_M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), 7);

				_M(uvRCoeffs) = _mm_shuffle_epi8(_M(uvRCoeffs), _M(shuffMask));

				out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(sub16), _M(uvRCoeffs));

				_M(uvGCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvGCoeffs));
				_M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), 7);
				_M(uvGCoeffs) = _mm_shuffle_epi8(_M(uvGCoeffs), _M(shuffMask));

				out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(sub16), _M(uvGCoeffs));

				_M(uvBCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvBCoeffs));
				_M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), 7);
				_M(uvBCoeffs) = _mm_shuffle_epi8(_M(uvBCoeffs), _M(shuffMask));

				out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(sub16), _M(uvBCoeffs));
			};

			static inline void pack_6_rgb_vectors_in_4_bgra_vectors_sse2(__m128i* in_6_r_g_b_vectors, __m128i* out_4_bgra_vectors)
			{
				__m128i* r1 = &in_6_r_g_b_vectors[0];
				__m128i* g1 = &in_6_r_g_b_vectors[1];
				__m128i* b1 = &in_6_r_g_b_vectors[2];
				__m128i* r2 = &in_6_r_g_b_vectors[3];
				__m128i* g2 = &in_6_r_g_b_vectors[4];
				__m128i* b2 = &in_6_r_g_b_vectors[5];
				__m128i zero = _mm_setzero_si128();

				__m128i rPacked = _mm_packus_epi16(*r1, *r2);						// PACKUSWB		1	0.5
				// R1 R2	R3 R4	R5 R6	R7 R8	R9 R10	R11 R12	R13 R14	R15 R16

				__m128i gPacked = _mm_packus_epi16(*g1, *g2);						// PACKUSWB		1	0.5
				// G1 G2	G3 G4	G5 G6	G7 G8	G9 G10	G11 G12	G13 G14	G15 G16

				__m128i bPacked = _mm_packus_epi16(*b1, *b2);						// PACKUSWB		1	0.5
				// B1 B2	B3 B4	B5 B6	B7 B8	B9 B10	B11 B12	B13 B14	B15 B16


				// unpacking lows
				__m128i bg = _mm_unpacklo_epi8(bPacked, gPacked);					// PUNPCKLBW	1	0.5
				// B1 G1	B2 G2	B3 G3	B4 G4	B5 G5	B6 G6	B7 G7	B8 G8

				__m128i ra = _mm_unpacklo_epi8(rPacked, zero);						// PUNPCKLBW	1	0.5
				// R1 A1	R2 A2	R3 A3	R4 A4	R5 A5	R6 A6	R7 A7	R8 A8

				M128_STORE(_mm_unpacklo_epi16(bg, ra), out_4_bgra_vectors[0]);		// PUNPCKLWD	1	0.5
				// B1 G1	R1 A1	B2 G2	R2 A2	B3 G3	R3 A3	B4 G4	R4 A4

				M128_STORE(_mm_unpackhi_epi16(bg, ra), out_4_bgra_vectors[1]);		// PUNPCKHWD	1	0.5
				// B5 G5	R5 A5	B6 G6	R6 A6	B7 G7	R7 A7	B8 G8	R8 A8

				// unpacking highs
				bg = _mm_unpackhi_epi8(bPacked, gPacked);							// PUNPCKHBW	1	0.5
				// B9 G9	B10 G10	B11 G11	B12 G12 B13 G13	B14 G14	B15 G15	B16 G16

				ra = _mm_unpackhi_epi8(rPacked, zero);								// PUNPCKHBW	1	0.5
				// R9 A9	R10 A10	R11 A11	R12 A12	R13 A13	R14 A14	R15 A15	R16 A16

				M128_STORE(_mm_unpacklo_epi16(bg, ra), out_4_bgra_vectors[2]);		// PUNPCKLWD	1	0.5
				// B9 G9	R9 A9	B10 G10	R10 A10	B11 G11	R11 A11	B12 G12	R12 A12

				M128_STORE(_mm_unpackhi_epi16(bg, ra), out_4_bgra_vectors[3]);		// PUNPCKHWD	1	0.5
				// B13 G13	R13 A13	B14 G14	R14 A14	B15 G15	R15 A15	B16 G16	R16 A16
			}

			static inline void unpack_high_yuv42Xp_to_uv_vector_sse2(__m128i* u_input, __m128i* v_input, __m128i* out_1_v16i_uv_vector1, __m128i* out_1_v16i_uv_vector2)
			{
				CONST_M128I(zero_vect, 0x0LL, 0x0LL);
				M128I(scratch, 0x0LL, 0x0LL);
				UNALIGNED_YUVP_UV_INPUT_PREAMBLE;

				_M(scratch) = _mm_unpackhi_epi8(*YUVP_U_INPUT_VECT, *YUVP_V_INPUT_VECT);
				// U9 V9	U10 V10	U11 V11	U12 V12	U13 V13	U14 V14	U15 V15	U16 V16		// PUNPCKHBW       1	0.5

				*out_1_v16i_uv_vector1 = _mm_unpacklo_epi8(_M(scratch), _M(zero_vect));
				// U9 0		V9 0	U10 0 	V10 0	U11 0 	V11 0	U12 0 	V12 0		// PUNPCKLBW       1	0.5

				*out_1_v16i_uv_vector2 = _mm_unpackhi_epi8(_M(scratch), _M(zero_vect));
				// U13 0	V13 0	U14 0 	V14 0	U15 0 	V15 0	U16 0	V16 0		// PUNPCKHBW       1	0.5
			}

			static inline void nnb_upsample_n_convert_yuv444_vectors_to_rgb_vectors_bt601_sse2_ssse3(__m128i* in_3_v16i_yuv_vectors, __m128i* out_3_v16i_rgb_vectors)
			{
				M128I(uvRCoeffs, 0x00B3000000B30000LL, 0x00B3000000B30000LL);
				M128I(uvGCoeffs, 0xFFA5FFD4FFA5FFD4LL, 0xFFA5FFD4FFA5FFD4LL);
				M128I(uvBCoeffs, 0x000000E3000000E3LL, 0x000000E3000000E3LL);
				M128I(ysub16, 0x0000000000000000LL, 0x0000000000000000LL); // we do not sub in Titanium

				M128I(usub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
				M128I(vsub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);

				M128I(tmp128x, 0x0000000000000000LL, 0x0000000000000000LL);
				M128I(tmp128y, 0x0000000000000000LL, 0x0000000000000000LL);

				M128I(tmp128r, 0x0000000000000000LL, 0x0000000000000000LL);
				M128I(tmp128g, 0x0000000000000000LL, 0x0000000000000000LL);
				M128I(tmp128b, 0x0000000000000000LL, 0x0000000000000000LL);

				// Y - 16
				_M(ysub16) = _mm_add_epi16(in_3_v16i_yuv_vectors[0], _M(ysub16));		// PADDW		1	0.5

				// U - 128	V - 128
				_M(usub128) = _mm_add_epi16(in_3_v16i_yuv_vectors[1], _M(usub128));		// PADDW		1	0.5
				_M(vsub128) = _mm_add_epi16(in_3_v16i_yuv_vectors[2], _M(vsub128));		// PADDW		1	0.5

				// UV values
				// u1~u8 v1~v8 -> u1 v1 u2 v2 u3 v3 u4 v4 ...
				_M(tmp128x) = _mm_unpacklo_epi16(_M(usub128), _M(vsub128));
				_M(tmp128y) = _mm_unpackhi_epi16(_M(usub128), _M(vsub128));

				// uvR
				_M(tmp128r) = _mm_madd_epi16(_M(tmp128x), _M(uvRCoeffs));
				_M(tmp128r) = _mm_srai_epi32(_M(tmp128r), 7);
				_M(uvRCoeffs) = _mm_madd_epi16(_M(tmp128y), _M(uvRCoeffs));
				_M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), 7);
				_M(tmp128r) = _mm_packs_epi32(_M(tmp128r), _M(uvRCoeffs));

				// uvG
				_M(tmp128g) = _mm_madd_epi16(_M(tmp128x), _M(uvGCoeffs));
				_M(tmp128g) = _mm_srai_epi32(_M(tmp128g), 7);
				_M(uvGCoeffs) = _mm_madd_epi16(_M(tmp128y), _M(uvGCoeffs));
				_M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), 7);
				_M(tmp128g) = _mm_packs_epi32(_M(tmp128g), _M(uvGCoeffs));

				// uvB
				_M(tmp128b) = _mm_madd_epi16(_M(tmp128x), _M(uvBCoeffs));
				_M(tmp128b) = _mm_srai_epi32(_M(tmp128b), 7);
				_M(uvBCoeffs) = _mm_madd_epi16(_M(tmp128y), _M(uvBCoeffs));
				_M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), 7);
				_M(tmp128b) = _mm_packs_epi32(_M(tmp128b), _M(uvBCoeffs));

				// R
				out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(ysub16), _M(tmp128r));

				// G
				out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(ysub16), _M(tmp128g));

				// B
				out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(ysub16), _M(tmp128b));
			}

			static inline Bgra32 Pixels_Bound(const Bgra32Rect& pic, long x, long y)
			{
				//assert((pic.width>0)&&(pic.height>0));
				bool IsInPic = true;
				if (x < 0) { x = 0; IsInPic = false; }
				else if (x >= pic.width) { x = pic.width - 1; IsInPic = false; }
				if (y < 0) { y = 0; IsInPic = false; }
				else if (y >= pic.height) { y = pic.height - 1; IsInPic = false; }
				Bgra32 result = Pixels(pic, x, y);
				if (!IsInPic) result.a = 0;
				return result;
			}

			static void Bilinear_Fast_MMX(Bgra32* PColor0, Bgra32* PColor1, unsigned long u_8, unsigned long v_8, Bgra32* result);
			static void Bilinear_Border_MMX(const Bgra32Rect& pic, const long x_16, const long y_16, Bgra32* result);
			static void ThreeOrder_Border_MMX(const Bgra32Rect& pic, const long x_16, const long y_16, Bgra32* result);

			static void ImageScaling_Align_SSE(const Bgra32Rect& Dst, const Bgra32Rect& Src);
			static void ImageScaling_Align_SSE_mmh(const Bgra32Rect& Dst, const Bgra32Rect& Src);
			static void ImageScaling_ThreeOrder_MMX(const Bgra32Rect& Dst, const Bgra32Rect& Src);
			static void ImageScaling_Bilinear_MMX_Ex(const Bgra32Rect& Dst, const Bgra32Rect& Src);
			static void ImageScaling_Bilinear_SSE2(const Bgra32Rect& Dst, const Bgra32Rect& Src);
			static void ImageScaling_BilinearTable_SSE2(const Bgra32Rect& Dst, const Bgra32Rect& Src);
			static void ImageScaling_ThreeOrderTable_SSE2(const Bgra32Rect& Dst, const Bgra32Rect& Src);
		};
	}
}
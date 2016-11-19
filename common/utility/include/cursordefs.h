#pragma once
#include <memory>
#include "imagedefs.h"
#include "rectdefs.h"

namespace Titanium { namespace TIRA
{
	typedef struct __CursorPosition
	{
		bool m_Visible;
		int x;
		int y;
	}CursorPosition;

	static const int CURSOR_MAX_SIZE = 256 * 256 * 4;
	static const int CURSOR_MASK_MAX_SIZE = CURSOR_MAX_SIZE / 8;
	enum TI_CURSOR_TYPE
	{
		TI_CURSOR_TYPE_MONOCHROME,
		TI_CURSOR_TYPE_COLOR_MASK,
		TI_CURSOR_TYPE_COLOR
	};

	struct ti_cursor
	{
		bool isUpdatePos;
		short posX;
		short posY;
		short width;
		short height;
		short hotspotX;
		short hotspotY;
		TI_CURSOR_TYPE type;
		unsigned char shape[CURSOR_MAX_SIZE];

		bool SamePosWith(const ti_cursor &c) const
		{
			return posX == c.posX && posY == c.posY;
		}
		bool SameImageWith(const ti_cursor &c) const
		{
			return
				width == c.width &&
				height == c.height &&
				hotspotX == c.hotspotX &&
				hotspotY == c.hotspotY &&
				type == c.type &&
				memcmp(shape, c.shape, CURSOR_MAX_SIZE) == 0;
		}
		bool SameWith(const ti_cursor &c) const
		{
			return SamePosWith(c) && SameImageWith(c);
		}
	};
}
}

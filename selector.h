#pragma once

#ifndef DTA100_SELECTOR_DEF
#define DTA100_SELECTOR_DEF
namespace dta100editor
{
	struct Selector
	{
		void Create(int32_t w, int32_t h, int32_t bw, int32_t bh)
		{
			x = 0;
			y = 0;
			width = w;
			height = h;
			bound_width = bw;
			bound_height = bh;
		}

		void Set(int32_t nx, int32_t ny)
		{
			x = nx;
			y = ny;

			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x > bound_width - width) x = bound_width - width;
			if (y > bound_height - height) y = bound_height - height;
		}

		int32_t x = 0;
		int32_t y = 0;
		int32_t width = 0;
		int32_t height = 0;
		int32_t bound_width = 0;
		int32_t bound_height = 0;
	};
}
#endif // DTA100_SELECTOR_DEF

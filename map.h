#pragma once

#ifndef DTA100_MAP_DEF
#define DTA100_MAP_DEF
namespace dta100editor
{
	template <typename T>
	struct Map
	{
		void Create(int32_t w, int32_t h)
		{
			width = w;
			height = h;
			pixels = w * h;
			data.reset(new T[pixels]{});
		}

		Map& operator = (const Map<T>& other)
		{
			if (&other == this)
				return *this;

			if (this->pixels != other.pixels)
			{
				this->data.reset(new T[other.pixels]{});
				this->width = other.width;
				this->height = other.height;
				this->pixels = other.pixels;
			}

			std::copy(&other.data[0], &other.data[0] + this->pixels, &this->data[0]);
		}

		T get(const int32_t x, const int32_t y) const
		{
			if (x < width && y < height)
				return data[y * width + x];
			else
				return 0;
		}

		void set(const int32_t x, const int32_t y, const T value)
		{
			if (x < width && y < height)
				data[y * width + x] = value;
		}

		int32_t width = 0;
		int32_t height = 0;
		int32_t pixels = 0;
		std::unique_ptr<T[]> data = nullptr;
	};
}
#endif // DTA100_MAP_DEF

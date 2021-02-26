#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "imgui.h"
#include "map.h"
#include "selector.h"

namespace dta100editor 
{

	class dta100editor : public olc::PixelGameEngine
	{

	public:
		dta100editor()
		{
			sAppName = "DTA100 Editor";
		}

	public:
		const int32_t scope_size = 32;
		imgui::ImmediateModeGUI* UI;
		olc::Pixel testColor = olc::WHITE;
		std::string testText = "Its white!";
		Map<uint32_t> dta100map;
		Selector selector;
		int red;
		int green;
		int blue;

		olc::Sprite* sprMap = nullptr;
		olc::Decal* decMap = nullptr;

		olc::Sprite* sprScope = nullptr;
		olc::Decal* decScope = nullptr;


	public:
		bool OnUserCreate() override
		{
			dta100map.Create(100, 100);
			sprMap = new olc::Sprite(100, 100);
			sprScope = new olc::Sprite(scope_size * 20, scope_size * 20);
			for (int32_t i = 0; i < dta100map.width; i++)
				for (int32_t j = 0; j < dta100map.height; j++)
				{
					dta100map.set(i, j, olc::Pixel(rand() % 256, rand() % 256, rand() % 256).n);
					sprMap->SetPixel(i, j, olc::Pixel(rand() % 256, rand() % 256, rand() % 256).n);
				}
			decMap = new olc::Decal(sprMap);
			decScope = new olc::Decal(sprScope);

			selector.Create(20, 20, dta100map.width, dta100map.height);

			updateSelector();

			UI = new imgui::ImmediateModeGUI(this);
			red = 0;
			green = 0;
			blue = 0;

			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override
		{
			UI->state.mouseX = GetMouseX();
			UI->state.mouseY = GetMouseY();
			UI->state.mouseDown = GetMouse(0).bHeld;

			Clear(olc::BLACK);
			checkSelector();
			drawMap();
			drawSelector();
			drawScope();
			checkDraw();
			checkPick();


			UI->Prepare();
			UI->Slider(GEN_ID, 5, 550, 255, red);
			UI->Slider(GEN_ID, 45, 550, 255, green);
			UI->Slider(GEN_ID, 85, 550, 255, blue);
			UI->Finish();

			DrawString(olc::vi2d{ 10, 500 }, std::to_string(red), olc::RED);
			DrawString(olc::vi2d{ 50, 500 }, std::to_string(green), olc::GREEN);
			DrawString(olc::vi2d{ 90, 500 }, std::to_string(blue), olc::BLUE);

			FillRect(olc::vi2d{ 20, 400 }, olc::vi2d{ 80, 80 }, olc::Pixel(red, green, blue));

			return true;
		}

		void drawMap()
		{
			DrawDecal(olc::vi2d{ 50, 50 }, decMap);
		}

		void drawSelector()
		{
			DrawRect(olc::vi2d{ selector.x+50, selector.y+50 }, olc::vi2d{ selector.width, selector.height }, olc::YELLOW);
		}

		void checkSelector()
		{
			int32_t x = GetMouseX()-50;
			int32_t y = GetMouseY()-50;
			int32_t w = dta100map.width + 50;
			int32_t h = dta100map.height + 100;
			if (x < w && y < h)
			{
				if (GetMouse(0).bHeld)
				{
					selector.Set(x - selector.width / 2, y - selector.height / 2);
					updateSelector();
				}
			}	
		}

		void updateSelector()
		{
			olc::vi2d a = olc::vi2d(scope_size, scope_size);
			for (int32_t x = selector.x; x < selector.x + selector.width; x++)
				for (int32_t y = selector.y; y < selector.y + selector.height; y++)
				{
					int32_t sx = (x - selector.x) * scope_size;
					int32_t sy = (y - selector.y) * scope_size;
					scopeTile(sx, sy, dta100map.get(x, y), decScope->sprite);
				}
			decScope->Update();
		}

		void updateSingleSelector(int32_t x, int32_t y, uint32_t c)
		{
			scopeTile(x * scope_size, y * scope_size, c, decScope->sprite);
			decScope->Update();
		}

		void scopeTile(int32_t x, int32_t y, uint32_t c, olc::Sprite* spr)
		{
			for (int i = x+1; i < x + scope_size-1; i++)
				for (int j = y + 1; j < y + scope_size - 1; j++)
					spr->SetPixel(i, j, c);
		}

		void drawScope()
		{
			DrawDecal(olc::vi2d{ 200, 1 }, decScope);
		}

		void checkDraw()
		{
			int32_t mx = GetMouseX();
			int32_t my = GetMouseY();

			if (mx > 200 && GetMouse(0).bHeld)
			{
				int32_t offx = (mx - 200) / scope_size;
				int32_t offy = my / scope_size;
				int32_t sx = selector.x + offx;
				int32_t sy = selector.y + offy;

				dta100map.set(sx, sy, olc::Pixel(red, green, blue).n);
				decMap->sprite->SetPixel(sx, sy, olc::Pixel(red, green, blue).n);
				decMap->Update();

				updateSingleSelector(offx, offy, olc::Pixel(red, green, blue).n);
			}
		}

		void checkPick()
		{
			int32_t mx = GetMouseX();
			int32_t my = GetMouseY();

			if (mx > 200 && GetMouse(1).bHeld)
			{
				int32_t sx = selector.x + (mx - 200) / scope_size;
				int32_t sy = selector.y + my / scope_size;

				uint32_t c = dta100map.get(sx, sy);

				olc::Pixel p = olc::Pixel(c);

				red = p.r;
				green = p.g;
				blue = p.b;
			}
		}
	};
}

int main()
{
	dta100editor::dta100editor main;
	if (main.Construct(1024, 768, 1, 1, false, true))
	{
		main.SetPixelMode(olc::Pixel::Mode::NORMAL);
		main.Start();
	}
	return 0;
}


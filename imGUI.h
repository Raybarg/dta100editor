#pragma once

#ifndef IM_GUI_DEF
#define IM_GUI_DEF

#include "olcPixelGameEngine.h"

namespace imgui
{
	class ImmediateModeGUI;

	struct strMyUIState {
		int mouseX;
		int mouseY;
		bool mouseDown;
		int hotItem;
		int activeItem;

		int kbditem;
		int keyentered;
		int keymod;

		int lastWidget;
	};

	class ImmediateModeGUI
	{
	public:
		ImmediateModeGUI(olc::PixelGameEngine* _pge);
		virtual ~ImmediateModeGUI();

	public:
		strMyUIState state;

	public:
		void Prepare();
		void Finish();

		bool Button(int id, int x, int y, std::string sText);
		int Slider(int id, int x, int y, int max, int& value);
		int textfield(int id, int x, int y, char* buffer);

	private:
		olc::PixelGameEngine* pge = nullptr;

		int RegionHit(int x, int y, int w, int h);
	};

	/******************/
	/* Implementation */
	/******************/

	ImmediateModeGUI::ImmediateModeGUI(olc::PixelGameEngine* _pge)
	{
		pge = _pge;
		state = { 0, 0, false, 0, 0, 0, 0, 0, 0 };
	}
	ImmediateModeGUI::~ImmediateModeGUI() { }

	/// <summary>
	/// Draw immediate mode UI button and return button pressed state
	/// </summary>
	/// <param name="id">Unique identifier for button</param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="sText">Button text</param>
	/// <returns>Button pressed True/False</returns>
	bool ImmediateModeGUI::Button(int id, int x, int y, std::string sText) {
		olc::vi2d pos = { x, y };
		olc::vi2d size = { 64, 48 };

		size_t textlength = sText.length();
		if (textlength > INT32_MAX) return false; // Failsafe for overflow exception
		int32_t length = static_cast<int32_t>(textlength);
		
		olc::vi2d txtPos = { x + size.x / 2 - length * 4, y + size.y / 2 - 4 };

		pge->FillRect(pos + olc::vi2d{ 5, 5 }, size, olc::DARK_GREY); // Shadow

		if (RegionHit(x, y, size.x, size.y))
		{
			state.hotItem = id;
			if (state.activeItem == 0 && state.mouseDown)
			{
				state.activeItem = id;
			}
		}

		if (state.hotItem == id)
		{
			if (state.activeItem == id)
			{
				// Its hot and active
				pge->FillRect(pos + olc::vi2d{ 2, 2 }, size, olc::WHITE);
				pge->DrawString(txtPos + olc::vi2d{ 2, 2 }, sText, olc::BLACK);
			}
			else
			{
				// Just hot
				pge->FillRect(pos, size, olc::WHITE);
				pge->DrawString(txtPos, sText, olc::BLACK);
			}
		}
		else
		{
			// Not hot
			pge->FillRect(pos, size, olc::GREY);
			pge->DrawString(txtPos, sText, olc::BLACK);
		}

		if (!state.mouseDown && state.hotItem == id && state.activeItem == id)
		{
			// Released mouse button on top of hot button -> button has been "pressed" properly
			return true;
		}

		return false;
	}

	int ImmediateModeGUI::Slider(int id, int x, int y, int max, int& value)
	{
		olc::vi2d pos = { x, y };
		olc::vi2d size = { 32, 116 };

		// Calculate mouse cursor's relative y offset
		int ypos = ((100 - 16) * value) / max;

		// Check for hotness
		if (RegionHit(x + 8, y + 8, 16, 100))
		{
			state.hotItem = id;
			if (state.activeItem == 0 && state.mouseDown)
				state.activeItem = id;
		}

		// Render the scrollbar
		pge->FillRect(pos, size, olc::GREY);

		if (state.activeItem == id || state.hotItem == id)
		{
			pge->FillRect(pos + olc::vi2d{ 8, 8 + ypos }, olc::vi2d{ 16, 16 }, olc::VERY_DARK_GREY);
		}
		else
		{
			pge->FillRect(pos + olc::vi2d{ 8, 8 + ypos }, olc::vi2d{ 16, 16 }, olc::DARK_GREY);
		}

		// Update widget value
		if (state.activeItem == id)
		{
			int mousepos = state.mouseY - (y + 8);
			if (mousepos < 0) mousepos = 0;
			if (mousepos > 100) mousepos = 100;
			int v = (mousepos * max) / 100;
			if (v != value)
			{
				value = v;
				return 1;
			}
		}

		return 0;
	}

	int ImmediateModeGUI::textfield(int id, int x, int y, char* buffer)
	{
		olc::vi2d pos = { x, y };

		size_t len = strlen(buffer);
		int changed = 0;

		// Check for hotness
		if (RegionHit(x - 4, y - 4, 30 * 14 + 8, 24 + 8))
		{
			state.hotItem = id;
			if (state.activeItem == 0 && state.mouseDown)
				state.activeItem = id;
		}

		// If no widget has keyboard focus, take it
		if (state.kbditem == 0)
			state.kbditem = id;

		// If we have keyboard focus, show it
		if (state.kbditem == id)
			pge->FillRect(pos + olc::vi2d{ -6, -6 }, olc::vi2d{ 30 * 14 + 12, 24 + 12 }, olc::DARK_GREY);

		// Render the text field
		if (state.activeItem == id || state.hotItem == id)
		{
			pge->FillRect(pos + olc::vi2d{ -4, -4 }, olc::vi2d{ 30 * 14 + 8, 24 + 8 }, olc::GREY);
		}
		else
		{
			pge->FillRect(pos + olc::vi2d{ -4, -4 }, olc::vi2d{ 30 * 14 + 8, 24 + 8 }, olc::VERY_DARK_GREY);
		}

		pge->DrawString(olc::vi2d{ x, y }, std::string(buffer), olc::WHITE);

		// Render cursor if we have keyboard focus
		/*
		if (state.kbditem == id)
			drawstring("_", x + len * 14, y);
		*/

		// If we have keyboard focus, we'll need to process the keys
		/*
		if (uistate.kbditem == id)
		{
			switch (uistate.keyentered)
			{
			case SDLK_TAB:
				// If tab is pressed, lose keyboard focus.
				// Next widget will grab the focus.
				uistate.kbditem = 0;
				// If shift was also pressed, we want to move focus
				// to the previous widget instead.
				if (uistate.keymod & KMOD_SHIFT)
					uistate.kbditem = uistate.lastwidget;
				// Also clear the key so that next widget
				// won't process it
				uistate.keyentered = 0;
				break;
			case SDLK_BACKSPACE:
				if (len > 0)
				{
					len--;
					buffer[len] = 0;
					changed = 1;
				}
				break;
			}
			if (uistate.keychar >= 32 && uistate.keychar < 127 && len < 30)
			{
				buffer[len] = uistate.keychar;
				len++;
				buffer[len] = 0;
				changed = 1;
			}
		}
		*/

		// If button is hot and active, but mouse button is not
		// down, the user must have clicked the widget; give it 
		// keyboard focus.
		if (state.mouseDown == 0 &&
			state.hotItem == id &&
			state.activeItem == id)
			state.kbditem = id;

		state.lastWidget = id;

		return changed;
	}

	/// <summary>
	/// Prepare imGUI. Must be called each frame before calling imGUI conditions.
	/// </summary>
	void ImmediateModeGUI::Prepare()
	{
		state.hotItem = 0;
	}

	/// <summary>
	/// Finish imGUI. Must be called each frame after all imGUI conditions have been called.
	/// </summary>
	void ImmediateModeGUI::Finish()
	{
		if (!state.mouseDown)
		{
			state.activeItem = 0;
		}
		else
		{
			if (state.activeItem == 0)
			{
				state.activeItem = -1;
			}
		}
	}

	/// <summary>
	/// Check if state mouse coordinates are inside given region
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="w"></param>
	/// <param name="h"></param>
	/// <returns>Mouse coordinates are inside given region true/false</returns>
	int ImmediateModeGUI::RegionHit(int x, int y, int w, int h)
	{
		if (state.mouseX < x || state.mouseY < y || state.mouseX >= x + w || state.mouseY > y + h)
		{
			return 0;
		}
		return 1;
	}


}
#define GEN_ID (__LINE__)
#endif // IM_GUI_DEF


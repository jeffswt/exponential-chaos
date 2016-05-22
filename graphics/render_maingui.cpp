
//	"render_maingui.cpp" -*- C++ -*-
//	Created on: 16 April 2016
//	Main menu interface manager.
//
//	Copyright (C) 2016  Geoffrey Tang.
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "graphics/render_private.h"

#include <GL/freeglut_ext.h>

namespace graphicsRenderMainGuiInternals
{
	void	beginSinglePlayer(void*	Param)
	{
		GuiCtrl::GuiState = GuiCtrl::LocalSaveGui;
		return ;
	}
	void	beginMultiPlayer(void*	Param)
	{
		GuiCtrl::GuiState = GuiCtrl::ServerGui;
		return ;
	}
	void	beginOptions(void*	Param)
	{
		GuiCtrl::GuiState = GuiCtrl::OptionsGui;
		Sleep(50);
		return ;
	}
	void	beginExitGame(void*	Param)
	{
		glutLeaveMainLoop();
		return ; // Which is not possible...
	}
}

bool	graphicsRenderMainGui(
		void)
{
	using namespace graphicsRenderMainGuiInternals;
//	Buttons definitions
	GuiDeclareObject(GuiObject, pictBackground,
			- GameConfig.WindowWidth / 2, GameConfig.WindowWidth / 2,
			GameConfig.WindowHeight / 2, - GameConfig.WindowHeight / 2,
			"gui/main/bg.png");
	GuiDeclareObject(GuiObject, pictLogo,
			-270, 270, 280, 10,
			"gui/main/logo.png");
	GuiDeclareObject(GuiButton, buttonSinglePlayer,
			-320, 320, -58, -122,
			"gui/button_vanilla/normal.png",
			"gui/button_vanilla/hovering.png",
			"gui/button_vanilla/pressed.png",
			NULL, beginSinglePlayer);
	GuiDeclareObject(GuiButton, buttonMultiPlayer,
			-320, 320, -134, -198,
			"gui/button_vanilla/normal.png",
			"gui/button_vanilla/hovering.png",
			"gui/button_vanilla/pressed.png",
			NULL, beginMultiPlayer);
	GuiDeclareObject(GuiButton, buttonOptions,
			-320, -6, -210, -274,
			"gui/button_vanilla/normal.png",
			"gui/button_vanilla/hovering.png",
			"gui/button_vanilla/pressed.png",
			NULL, beginOptions);
	GuiDeclareObject(GuiButton, buttonExitGame,
			6, 320, -210, -274,
			"gui/button_vanilla/normal.png",
			"gui/button_vanilla/hovering.png",
			"gui/button_vanilla/pressed.png",
			NULL, beginExitGame);
//	Font and strings definitions
	GuiDeclareObject(GuiFont, fontSinglePlayer,
			- 138, -102, 1.0, 1.0, 1.0,
			32, ANSI_CHARSET, "OCR A Std",
			"Singleplayer");
	GuiDeclareObject(GuiFont, fontMultiPlayer,
			- 126, -178, 1.0, 1.0, 1.0,
			32, ANSI_CHARSET, "OCR A Std",
			"Multiplayer");
	GuiDeclareObject(GuiFont, fontOptions,
			- 240, -254, 1.0, 1.0, 1.0,
			32, ANSI_CHARSET, "OCR A Std",
			"Options");
	GuiDeclareObject(GuiFont, fontExitGame,
			66, -254, 1.0, 1.0, 1.0,
			32, ANSI_CHARSET, "OCR A Std",
			"Exit Game");
	pictBackground.RenderObject();
	pictLogo.RenderObject();
	buttonSinglePlayer.RenderObject();
	buttonMultiPlayer.RenderObject();
	buttonOptions.RenderObject();
	buttonExitGame.RenderObject();
	fontSinglePlayer.RenderObject();
	fontMultiPlayer.RenderObject();
	fontOptions.RenderObject();
	fontExitGame.RenderObject();
	return true;
}

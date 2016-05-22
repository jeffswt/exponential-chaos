
//	"render_gmpsd.cpp" -*- C++ -*-
//	Created on: 17 April 2016
//	Game paused menu renderer.
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

#include "native/keyboard.h"

namespace	graphicsRenderGamePausedGuiInternals
{
	bool	thisIsHost = false;
	void	beginReturn(
			void*	Param)
	{
		PhEngine::PhEngineState = PhEngine::Running;
		GuiCtrl::GuiState = GuiCtrl::Game;
		return ;
	}
	void	beginSaveExit(
			void*	Param)
	{
		GuiDrawLoadingDialog("Saving world, please wait...");
		PhEngine::PhEngineState = PhEngine::SaveExit;
		GuiCtrl::GuiState = GuiCtrl::MainGui;
		WaitForEngineEvent();
		return ;
	}
	void	beginDiscardExit(
			void*	Param)
	{
		if (thisIsHost)
			GuiDrawLoadingDialog("Please wait...");
		else if (!thisIsHost)
			GuiDrawLoadingDialog("Disconnecting, please wait...");
		PhEngine::PhEngineState = PhEngine::Stopped;
		GuiCtrl::GuiState = GuiCtrl::MainGui;
		WaitForEngineEvent();
		return ;
	}
}

bool	graphicsRenderGamePausedGui(
		void)
{
	if (PhEngine::PhEngineState == PhEngine::Stopped) {
		GuiCtrl::GuiState = GuiCtrl::MainGui;
		return false;
	}
	using namespace graphicsRenderGamePausedGuiInternals;
	GuiDeclareObject(GuiButton, buttonReturn,
			-320, 320, 32, -32,
			"gui/button_vanilla/normal.png",
			"gui/button_vanilla/hovering.png",
			"gui/button_vanilla/pressed.png",
			NULL, beginReturn);
	GuiDeclareObject(GuiButton, buttonSaveExit,
			-320, -6, -44, -108,
			"gui/button_vanilla/normal.png",
			"gui/button_vanilla/hovering.png",
			"gui/button_vanilla/pressed.png",
			NULL, beginSaveExit);
	GuiDeclareObject(GuiButton, buttonDiscardExit,
			6, 320, -44, -108,
			"gui/button_vanilla/normal.png",
			"gui/button_vanilla/hovering.png",
			"gui/button_vanilla/pressed.png",
			NULL, beginDiscardExit);
	GuiDeclareObject(GuiButton, buttonDisconnect,
			-320, 320, -44, -108,
			"gui/button_vanilla/normal.png",
			"gui/button_vanilla/hovering.png",
			"gui/button_vanilla/pressed.png",
			NULL, beginDiscardExit);
	GuiDeclareObject(GuiFont, fontTitle,
			- 180, GameConfig.WindowHeight / 2 - 60, 0.0, 0.0, 0.0,
			50, ANSI_CHARSET, "OCR A Std",
			"Game Paused");
	GuiDeclareObject(GuiFont, fontReturn,
			- 158, -12, 1.0, 1.0, 1.0,
			32, ANSI_CHARSET, "OCR A Std",
			"Return to game");
	GuiDeclareObject(GuiFont, fontSaveExit,
			- 280, -88, 1.0, 1.0, 1.0,
			32, ANSI_CHARSET, "OCR A Std",
			"Save & Exit");
	GuiDeclareObject(GuiFont, fontDiscardExit,
			110, -88, 1.0, 1.0, 1.0,
			32, ANSI_CHARSET, "OCR A Std",
			"Exit");
	GuiDeclareObject(GuiFont, fontDisconnect,
			- 280, -88, 1.0, 1.0, 1.0,
			32, ANSI_CHARSET, "OCR A Std",
			"Disconnect from server");
//	Something else...
	if (kGetKeyOnpress(VK_ESCAPE))
		beginReturn(NULL);
//	Render button objects
	buttonReturn.RenderObject();
	if (GuiCtrl::workMainMap->IsHost) {
		buttonSaveExit.RenderObject();
		buttonDiscardExit.RenderObject();
	} else if (!GuiCtrl::workMainMap->IsHost) {
		buttonDisconnect.RenderObject();
	}
//	Render font objects
	fontTitle.RenderObject();
	fontReturn.RenderObject();
	thisIsHost = GuiCtrl::workMainMap->IsHost;
	if (thisIsHost) {
		fontSaveExit.RenderObject();
		fontDiscardExit.RenderObject();
	} else if (!thisIsHost) {
		fontDisconnect.RenderObject();
	}
	return true;
}

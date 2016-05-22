
//	"render_private.h" -*- C++ -*-
//	Created on: 28 April 2016
//	Private header for graphics renderer routines.
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

#ifndef GRAPHICS_RENDER_PRIVATE_H_
#define GRAPHICS_RENDER_PRIVATE_H_

#include "graphics/render.h"

#include <sstream>

#include "graphics/gui.h"
#include "native/inputctrl.h"
#include "physics/chunk.h"
#include "physics/config.h"
#include "physics/engine.h"

#include <windows.h>

extern	InputCtrlType	InputControl;
extern	ConfigType		GameConfig;
extern	std::string		CurrentDirectory;

namespace	GuiCtrl
{
	enum GuiStateType {
		LoadGui,
		MainGui,
		OptionsGui,
		LocalSaveGui,
		ServerGui,
		Game,
		GamePaused
	};
	extern	GuiStateType	GuiState;
	extern	GameMap*		workMainMap;
}

bool	graphicsRenderGamePausedGui(
		void);

bool	graphicsRenderGameOverlay(
		GameMap*	MainMap);

bool	graphicsRenderGame(
		GameMap*	MainMap);

bool	graphicsRenderLoadGui(
		void);

bool	graphicsRenderLocalSaveGui(
		void);

bool	graphicsRenderServerGui(
		void);

bool	graphicsRenderMainGui(
		void);

bool	graphicsRenderOptionsGui(
		void);

#endif /* GRAPHICS_RENDER_PRIVATE_H_ */

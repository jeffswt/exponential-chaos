
//	"render_ldgui.cpp" -*- C++ -*-
//	Created on: 17 April 2016
//	Program initialisation interface renderer.
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

#include <mutex>
#include <thread>

//#define RENDER_LDGUI_NO_GUI_RENDERING_

namespace	graphicsLoadGuiInternals{
	double		LoadPercentage;
}

bool	graphicsRenderLoadGui(
		void)
{
	using namespace graphicsLoadGuiInternals;
//	The above is used for smaller gap between UI changing
#ifndef RENDER_LDGUI_NO_GUI_RENDERING_
	GuiDeclareObject(GuiObject, pictBg,
			-GameConfig.WindowWidth / 2, GameConfig.WindowWidth / 2,
			GameConfig.WindowHeight / 2, -GameConfig.WindowHeight / 2,
			"gui/misc/bg.png");
	GuiDeclareObject(GuiObject, pictLogo,
			-320, 320, GameConfig.WindowHeight / 2 - 60, GameConfig.WindowHeight / 2 - 540,
			"gui/load/logo.png");
	GuiDeclareObject(GuiObject, objPercentInactive,
			-GameConfig.WindowWidth / 2 + 128, GameConfig.WindowWidth / 2 - 128,
			-GameConfig.WindowHeight / 2 + 120, -GameConfig.WindowHeight / 2 + 80,
			"gui/load/bar_inactive.png");
	GuiObject	objPercentActive;
#endif
	if (LoadPercentage < 0.995) {
#ifndef RENDER_LDGUI_NO_GUI_RENDERING_
		objPercentActive.SetProperties(
				-GameConfig.WindowWidth / 2 + 128,
				(GameConfig.WindowWidth - 256) * LoadPercentage - GameConfig.WindowWidth / 2 + 128,
				-GameConfig.WindowHeight / 2 + 120, -GameConfig.WindowHeight / 2 + 80,
				"gui/load/bar_active.png");
//		Rendering objects
		pictBg.RenderObject();
		pictLogo.RenderObject();
		objPercentInactive.RenderObject();
		objPercentActive.RenderObject();
#else
		std::cout << LoadPercentage << "\n" << std::flush;
#endif
//		Checking whether there are any PNG textures to be dumped

	} else {
		if (LoadPercentage > 1.5) {
			LoadPercentage = 0.0;
			return true;
		}
//		Done initialising, which should only have happened once
		LoadPercentage = 1.0;
		endImportGameEntityTypes();
		GuiCtrl::GuiState = GuiCtrl::MainGui;
	}
	return true;
}

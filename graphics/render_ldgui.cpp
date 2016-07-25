
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

#include <iomanip>
#include <mutex>
#include <sstream>
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
	GuiDeclareObject(GuiFont, fontIndicator,
			-GameConfig.WindowWidth / 2 + 140, -GameConfig.WindowHeight / 2 + 130,
			1.0, 1.0, 1.0, 20, ANSI_CHARSET, "OCR A Std", "Loading entities...");
	GuiObject	objPercentActive;
	if (LoadPercentage < 0.995) {
		objPercentActive.SetProperties(
				-GameConfig.WindowWidth / 2 + 128,
				(GameConfig.WindowWidth - 256) * LoadPercentage - GameConfig.WindowWidth / 2 + 128,
				-GameConfig.WindowHeight / 2 + 120, -GameConfig.WindowHeight / 2 + 80,
				"gui/load/bar_active.png");
//		Process text info
		std::stringstream	SStream;
		std::string			SOut;
		SStream << "Loading entities... " << std::fixed << std::setprecision(3) << LoadPercentage * 100 << "%";
		getline(SStream, SOut);
		fontIndicator.SetContent(SOut);
//		Rendering objects
		pictBg.RenderObject();
		pictLogo.RenderObject();
		objPercentInactive.RenderObject();
		objPercentActive.RenderObject();
		fontIndicator.RenderObject();
	} else {
		if (LoadPercentage > 1.5) {
			LoadPercentage = 0.0;
			return true;
		}
//		Done initialising, which should only have happened once
		LoadPercentage = 1.0;
		endImportGameEntityTypes();
//		GuiCtrl::GuiState = GuiCtrl::LoadTexGui;
		GuiCtrl::GuiState = GuiCtrl::MainGui;
	}
	return true;
}

extern	int	CreatedTextures;
bool	graphicsRenderLoadTexGui(
		void)
{
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
	GuiDeclareObject(GuiFont, fontIndicator,
			-GameConfig.WindowWidth / 2 + 140, -GameConfig.WindowHeight / 2 + 130,
			1.0, 1.0, 1.0, 20, ANSI_CHARSET, "OCR A Std", "Loading textures...");
	GuiObject	objPercentActive;
	static	int	LoadedTextures = 1;
	double		LoadPercentage = (double)LoadedTextures / CreatedTextures;
//	Rendering the items and setting the progress bar.
	if (LoadedTextures <= CreatedTextures) {
		objPercentActive.SetProperties(
				-GameConfig.WindowWidth / 2 + 128,
				(GameConfig.WindowWidth - 256) * LoadPercentage - GameConfig.WindowWidth / 2 + 128,
				-GameConfig.WindowHeight / 2 + 120, -GameConfig.WindowHeight / 2 + 80,
				"gui/load/bar_active.png");
//		Process text info
		std::stringstream	SStream;
		std::string			SOut;
		SStream << "Loading textures... " << std::fixed << std::setprecision(3) << LoadPercentage * 100 << "%";
		getline(SStream, SOut);
		fontIndicator.SetContent(SOut);
//		Rendering objects
		pictBg.RenderObject();
		pictLogo.RenderObject();
		objPercentInactive.RenderObject();
		objPercentActive.RenderObject();
		fontIndicator.RenderObject();
//		To post this texture into the buffer, we need to render it once
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, LoadedTextures);
		glBegin(GL_POLYGON);
			double	RenderX, RenderY;
			double	TexX, TexY;
			for (double itX = 0.5, itY = 0.5; ; ) {
				RenderX = itX * 10;
				RenderY = itY * 10 - GameConfig.WindowHeight / 2 + 100;
				TexX = 0.5 + itX;
				TexY = 0.5 + itY;
				glTexCoord2d(TexX, TexY);
				glVertex2f(RenderX, RenderY);
				if (itX == 0.5 && itY == 0.5) itY = -0.5;
				else if (itX == 0.5 && itY == -0.5) itX = -0.5;
				else if (itX == -0.5 && itY == -0.5) itY = 0.5;
				else break;
			}
		glEnd();
		LoadedTextures++;
	} else {
		GuiCtrl::GuiState = GuiCtrl::MainGui;
	}
	return true;
}


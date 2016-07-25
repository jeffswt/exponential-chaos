
//	"render_gm.cpp" -*- C++ -*-
//	Created on: 11 April 2016
//	Game interface renderer.
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

#include <set>

bool graphicsRenderGame(
		GameMap*	MainMap)
{
	if (PhEngine::PhEngineState == PhEngine::Stopped) {
		GuiCtrl::GuiState = GuiCtrl::MainGui;
		return false;
	}
	GuiDeclareObject(GuiObject, pictBackground,
			- GameConfig.WindowWidth / 2, GameConfig.WindowWidth / 2,
			GameConfig.WindowHeight / 2, - GameConfig.WindowHeight / 2,
			"misc/game_bg.png");
	static	GuiFont	fontPlayerName;
	pictBackground.RenderObject();
	MainMap->LockMemory();
//	Find player chunk and locate its number
	Entity*			PlayerEnt = MainMap->EntityList[InputControl.PlayerGuid];
	std::set<Chunk*>RendChunks;
	std::set<int>	RendLayers;
	int 			MidNum = GetEntityChunkNum(InputControl.CameraX);
	if (PlayerEnt == NULL)
		return false;
	RendChunks.clear();
	RendLayers.clear();
//	Process required chunks
	for (int chnk = MidNum - GameConfig.ViewDistance; chnk <= MidNum + GameConfig.ViewDistance; chnk++)
		if (MainMap->ChunkList[chnk] != NULL)
			RendChunks.insert(MainMap->ChunkList[chnk]);
//	Process required layers
	for (auto itert : RendChunks)
		for (auto layerItrt : itert->EntityLayerList)
			if (RendLayers.find(layerItrt.first) == RendLayers.end())
				RendLayers.insert(layerItrt.first);
//	Main render system
	for (int layer : RendLayers)
	for (auto chnk : RendChunks)
	if (chnk->EntityLayerList.find(layer) != chnk->EntityLayerList.end()) {
		auto& layerList = chnk->EntityLayerList[layer];
		for (auto itrt : layerList) {
			Entity*		RenEnt = itrt.second;
			if (!RenEnt->DataIntact()) continue;
			EntityType*	RenTyp = RenEnt->Properties.Type;
			EntityType::typeGraphics	nGraphics
					= RenTyp->Graphics[RenEnt->Properties.TypeState];
//			If hidden, do not render
			if (!RenEnt->RenderEnabled()) continue;
//			If out of render area, do not render.
			if ((RenEnt->Physics.PosX - nGraphics.LengthX - InputControl.CameraX) * GameConfig.PixelRatio > 0.5 * GameConfig.WindowWidth ||
					(RenEnt->Physics.PosX + nGraphics.LengthX - InputControl.CameraX) * GameConfig.PixelRatio < -0.5 * GameConfig.WindowWidth ||
					(RenEnt->Physics.PosY - nGraphics.LengthY - InputControl.CameraY) * GameConfig.PixelRatio > 0.5 * GameConfig.WindowHeight ||
					(RenEnt->Physics.PosY + nGraphics.LengthY - InputControl.CameraY) * GameConfig.PixelRatio < -0.5 * GameConfig.WindowHeight)
				continue;
			glEnable(GL_TEXTURE_2D);
//			After doing this, textures wouldn't look blurry even if it's small
//			Thanks to http://stackoverflow.com (again)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glColor3f(1.0, 1.0, 1.0);
//			We are now processing an animation
			if (nGraphics.TextureList.size() > 1) {
				double	CurTexTime = MainMap->CurTime;
				if (RenTyp->Properties.Type == "Particle")
					CurTexTime -= RenEnt->Properties.GenTime;
				double	TexTime = CurTexTime - (int)(CurTexTime / nGraphics.AnimationInterval) *
						nGraphics.AnimationInterval;
				int		TexIndex = TexTime / nGraphics.AnimationInterval * nGraphics.TextureList.size();
				if (TexIndex < 0 || TexIndex > 1048576) TexIndex = 0; // Some precautions...
				glBindTexture(GL_TEXTURE_2D, nGraphics.TextureList[TexIndex]);
			} else if (nGraphics.TextureList.size() == 1) {
				glBindTexture(GL_TEXTURE_2D, nGraphics.TextureList[0]);
			} else {
				glBindTexture(GL_TEXTURE_2D, 0);
			}
//			Draw the polygon of the entity
			glBegin(GL_POLYGON);
			double	RenderSiX, RenderSiY;
			double	RenderX, RenderY;
			double	TexX, TexY;
			for (double itX = 0.5, itY = 0.5; ; ) {
				RenderSiX = itX * nGraphics.LengthX + RenEnt->Physics.PosX - InputControl.CameraX;
				RenderSiY = itY * nGraphics.LengthY + RenEnt->Physics.PosY - InputControl.CameraY;
				RenderX = RenderSiX * GameConfig.PixelRatio;
				RenderY = RenderSiY * GameConfig.PixelRatio;
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
//			Render player entities, this is an exception.
			if (RenTyp->Properties.Type == "Player") {
				fontPlayerName.SetProperties(
					RenderX, RenderY, 0.0, 0.0, 0.0,
					20, ANSI_CHARSET, "OCR A Std");
				fontPlayerName.SetContent(RenEnt->Properties.Name);
				fontPlayerName.RenderObject();
			}
		}
	}
//	Rendering some overlays
	graphicsRenderGameOverlay(MainMap);
//	Finalising render procedure
	MainMap->UnlockMemory();
	return true;
}

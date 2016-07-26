
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

void	my_proc_tex_rotate(
		int*	tcx,
		int*	tcy)
{
//	Unprotected function, call from graphicsFuncRotateTexCoords please.
	int	ntcx[4], ntcy[4];
	ntcx[0] = tcx[3], ntcy[0] = tcy[3];
	for (int i = 0; i < 3; i++)
		ntcx[i + 1] = tcx[i], ntcy[i + 1] = tcy[i];
	for (int i = 0; i < 4; i++)
		tcx[i] = ntcx[i], tcy[i] = ntcy[i];
	return ;
}

void	my_proc_tex_mirror(
		int*	tcx,
		int*	tcy)
{
//	Unprotected function, call from graphicsFuncRotateTexCoords please.
	std::swap(tcx[3], tcx[0]);
	std::swap(tcx[2], tcx[1]);
	std::swap(tcy[3], tcy[0]);
	std::swap(tcy[2], tcy[1]);
	return ;
}

void	graphicsFuncRotateTexCoords(
		int*	tcx,
		int*	tcy,
		int		tr)
{
	if (tr <= 0 || tr >= 8)
		return ; // There's nothing to do, as it's not possible
	if (tr >= 4) {
//		Proceed mirroring first, which is the deal
		my_proc_tex_mirror(tcx, tcy);
		tr -= 4;
	}
	for (int i = 0; i < tr; i++)
		my_proc_tex_rotate(tcx, tcy);
	return ;
}

void	graphicsFuncRenderEntity(
		GameMap*	MainMap,
		EntityType*	RenTyp,
		Entity*		RenEnt,
		EntityType::typeGraphics&
					nGraphics,
		bool		RenderItemOnHand,
		double		CenterX,
		double		CenterY,
		double		Width,
		double		Height,
		double		PixelRatio)
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0, 1.0, 1.0);
//	We are now processing an animation
	if (!RenderItemOnHand) {
		if (nGraphics.TextureList.size() > 1) {
			double	CurTexTime = MainMap->CurTime;
			if (RenTyp->Properties.Type == "Particle"
					&& RenEnt != NULL)
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
	} else {
		glBindTexture(GL_TEXTURE_2D, nGraphics.TextureOnHand);
	}
//	Draw the polygon of the entity
	glBegin(GL_POLYGON);
	int	TexCoordsX[4] = {1, 1, -1, -1},
		TexCoordsY[4] = {1, -1, -1, 1},
		RendCoordsX[4] = {1, 1, -1, -1},
		RendCoordsY[4] = {1, -1, -1, 1};
	graphicsFuncRotateTexCoords(TexCoordsX, TexCoordsY, nGraphics.TexRotation);
	for (int i = 0; i < 4; i++) {
		double	itX = 0.5 * RendCoordsX[i], itY = 0.5 * RendCoordsY[i];
		double	RenderX = (CenterX + itX * Width) * PixelRatio,
				RenderY = (CenterY + itY * Height) * PixelRatio;
		glTexCoord2d(0.5 + 0.5 * TexCoordsX[i], 0.5 + 0.5 * TexCoordsY[i]);
		glVertex2f(RenderX, RenderY);
		if (itX == 0.5 && itY == 0.5) itY = -0.5;
		else if (itX == 0.5 && itY == -0.5) itX = -0.5;
		else if (itX == -0.5 && itY == -0.5) itY = 0.5;
		else break;
	}
	glEnd();
	return ;
}

bool	graphicsRenderGame(
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
			graphicsFuncRenderEntity(MainMap, RenTyp, RenEnt, nGraphics, false,
					RenEnt->Physics.PosX - InputControl.CameraX,
					RenEnt->Physics.PosY - InputControl.CameraY,
					nGraphics.LengthX,
					nGraphics.LengthY,
					GameConfig.PixelRatio);
//			Render player entities, this is an exception.
			if (RenTyp->Properties.Type == "Player") {
//				Render things on the player's hand
				PlayerEntity*	PlyEnt = (PlayerEntity*)RenEnt->Physics.ExtendedTags;
				if (PlyEnt->InventoryFocus > 0 &&
						PlyEnt->InventoryFocus <= (int)PlyEnt->Inventory.size() &&
						PlyEnt->InventoryFocus <= 9) {
//					That means there's something he is holding
					auto		workPair = &PlyEnt->Inventory[PlyEnt->InventoryFocus - 1];
					EntityType*	HldEnt = workPair->first;
					EntityType::typeGraphics
								hnGraphics = HldEnt->Graphics[workPair->second];
//					To render its held item or not, this is a question.
					bool	renderHeldItem = true;
					if (RenEnt->Properties.Name == GameConfig.PlayerName
							&& HldEnt->Properties.Type == "Block")
						renderHeldItem = false;
					double	scaleFactor = 0.6 / std::min(hnGraphics.LengthX, hnGraphics.LengthY);
					graphicsFuncRenderEntity(MainMap, HldEnt, NULL, hnGraphics, renderHeldItem,
							RenEnt->Physics.PosX + nGraphics.LengthX * 0.5 - InputControl.CameraX,
							RenEnt->Physics.PosY - InputControl.CameraY,
							hnGraphics.LengthX * scaleFactor,
							hnGraphics.LengthY * scaleFactor,
							GameConfig.PixelRatio);
				}
//				Render player name
				double	RenderX = (- nGraphics.LengthX * 0.5 + RenEnt->Physics.PosX -
						InputControl.CameraX) * GameConfig.PixelRatio;
				double	RenderY = (nGraphics.LengthY * 0.5 + RenEnt->Physics.PosY -
						InputControl.CameraY) * GameConfig.PixelRatio;
				fontPlayerName.SetProperties(
					RenderX, RenderY, 0.0, 0.0, 0.0,
					20, ANSI_CHARSET, "OCR A Std");
				fontPlayerName.SetContent(RenEnt->Properties.Name);
				fontPlayerName.RenderObject();
			}
			continue;
		}
	}
//	Rendering some overlays
	graphicsRenderGameOverlay(MainMap);
//	Finalising render procedure
	MainMap->UnlockMemory();
	return true;
}

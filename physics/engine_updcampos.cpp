
//	"engine_updcampos.cpp" -*- C++ -*-
//	Created on: 28 April 2016
//	Physics engine / Camera position updater.
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

#include "physics/engine_private.h"

bool	PhEngine::UpdateCameraPosition(
		GameMap*	MainMap,
		double		DeltaTime,
		bool		DetectMouse)
{
	/* Main function */
	float distX = (float)(InputControl.MouseX - GameConfig.WindowWidth / 2) /
			GameConfig.PixelRatio;
	float distY = (float)(GameConfig.WindowHeight / 2 - InputControl.MouseY) /
			GameConfig.PixelRatio;
	float MouseDist = sqrt(distX * distX + distY * distY);
	float EdgeDist = GameConfig.CameraDistTolerance / GameConfig.PixelRatio;
	float curSpeed = 0.0;
	/* Update camera position according to mouse position */
	if (MouseDist > EdgeDist && DetectMouse && InputControl.MouseEntry == GLUT_ENTERED) {
		curSpeed = (MouseDist - EdgeDist) * GameConfig.CameraSpeed;
		InputControl.CameraX += curSpeed * distX / MouseDist * DeltaTime;
		InputControl.CameraY += curSpeed * distY / MouseDist * DeltaTime;
	}
	/* Update camera position according to player position */
	Entity *playerEntity = MainMap->EntityList[InputControl.PlayerGuid];
	if (!playerEntity->DataIntact()) return false;
	if (playerEntity->Physics.PosX > InputControl.CameraX + EdgeDist)
		InputControl.CameraX = playerEntity->Physics.PosX - EdgeDist;
	if (playerEntity->Physics.PosX < InputControl.CameraX - EdgeDist)
		InputControl.CameraX = playerEntity->Physics.PosX + EdgeDist;
	if (playerEntity->Physics.PosY > InputControl.CameraY + EdgeDist * GameConfig.WindowHeight / GameConfig.WindowWidth)
		InputControl.CameraY = playerEntity->Physics.PosY - EdgeDist * GameConfig.WindowHeight / GameConfig.WindowWidth;
	if (playerEntity->Physics.PosY < InputControl.CameraY - EdgeDist * GameConfig.WindowHeight / GameConfig.WindowWidth)
		InputControl.CameraY = playerEntity->Physics.PosY + EdgeDist * GameConfig.WindowHeight / GameConfig.WindowWidth;
	return true;
}

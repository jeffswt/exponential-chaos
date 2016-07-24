
//	"render.cpp" -*- C++ -*-
//	Created on: 5 March 2016
//	Definitions for OpenGL renderer dispatcher.
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
#include "graphics/entry.h"
#include "graphics/gui.h"

namespace	GuiCtrl {
	GuiStateType	GuiState(LoadGui);
	GameMap*		workMainMap;
}

void	graphicsMouseClick(
		int	button,
		int	state,
		int	x,
		int	y)
{
	switch(button) {
	case GLUT_LEFT_BUTTON:
		InputControl.LDown = state == GLUT_DOWN ? 1 : 0; break;
	case GLUT_RIGHT_BUTTON:
		InputControl.RDown = state == GLUT_DOWN ? 1 : 0; break;
	case GLUT_MIDDLE_BUTTON:
		InputControl.MDown = state == GLUT_DOWN ? 1 : 0; break;
	default:
		break;
	}
	InputControl.MouseX = x;
	InputControl.MouseY = y;
	return ;
}

void	graphicsMouseMotionActive(
		int	x,
		int	y)
{
	InputControl.MouseX = x;
	InputControl.MouseY = y;
	return ;
}

void	graphicsMouseMotionPassive(
		int	x,
		int	y)
{
	InputControl.MouseX = x;
	InputControl.MouseY = y;
	return ;
}

void	graphicsMouseEntry(
		int	state)
{
	InputControl.MouseEntry = state;
	return ;
}

bool	graphicsUpdateResolution(
		void)
{
	int	oldWidth = GameConfig.WindowWidth;
	int	oldHeight = GameConfig.WindowHeight;
	int	newWidth = glutGet(GLUT_WINDOW_WIDTH);
	int	newHeight = glutGet(GLUT_WINDOW_HEIGHT);
	if (oldWidth == newWidth && oldHeight == newHeight)
		return true;
	GameConfig.WindowWidth = newWidth;
	GameConfig.WindowHeight = newHeight;
	PostResolutionRequireUpdate();
	graphicsInit();
	return true;
}

void	graphicsRender(
		void)
{
//	Default settings
	using namespace GuiCtrl;
	if (GameConfig.PixelRatio == 0 || CurrentDirectory == "")
		throw NullPointerException();
//	Initialise renderer
	glClear(GL_COLOR_BUFFER_BIT); // Clear window
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	double	BeginTime = GetProcessTime();
	double	EndTime = BeginTime;
	double	FrameWaitTime = 0.015;
//	Finished pre-render loading, now we shall continue to load individuals
	graphicsUpdateResolution();
	switch (GuiState) {
	case LoadGui:
		graphicsRenderLoadGui();
		break;
	case MainGui:
		graphicsRenderMainGui();
		break;
	case OptionsGui:
		graphicsRenderOptionsGui();
		break;
	case LocalSaveGui:
		graphicsRenderLocalSaveGui();
		break;
	case ServerGui:
		graphicsRenderServerGui();
		break;
	case MiscLoadDialog:
		GuiDrawLoadingDialogWorker();
		break;
	case Game:
		graphicsRenderGame(workMainMap);
		if (PhEngine::PhEngineState == PhEngine::Paused)
			GuiState = GamePaused;
		break;
	case GamePaused:
		graphicsRenderGame(workMainMap);
		graphicsRenderGamePausedGui();
		break;
	default:
		break;
	}
//	Posting results
	glFlush();
//	Estimate generation time of next frame
	EndTime = GetProcessTime();
	if (EndTime - BeginTime > 0.003)
		FrameWaitTime = (EndTime - BeginTime) * 5;
	if (EndTime - BeginTime > 0.3)
		FrameWaitTime = 1.5;
	SleepFromTime(BeginTime, FrameWaitTime);
	glutPostRedisplay();
	return ;
}

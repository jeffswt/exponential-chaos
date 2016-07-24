
//	"render_optgui.cpp" -*- C++ -*-
//	Created on: 17 April 2016
//	Options menu interface renderer.
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

namespace graphicsRenderOptionsGuiInternals
{
	template <typename TYPE>
	std::string	ToString(
			TYPE	From)
	{
		std::stringstream	Stream;
		Stream << From;
		std::string Ret;
		Stream >> Ret;
		return Ret;
	}
	void	SetNameWorker(
			void*	Param)
	{
		std::string	NewName = *((std::string*)Param);
		if (NewName.length() > 0)
			GameConfig.PlayerName = NewName;
		return ;
	}
	void	SetName(
			void*	Param)
	{
		GuiInputDialog("Player name:",
				SetNameWorker);
		return ;
	}
	void	SetViewDistanceWorker(
			void*	Param)
	{
		std::string			NewRes = *((std::string*)Param);
		std::stringstream	sStream;
		int					nDist;
		sStream << NewRes;
		sStream >> nDist;
		if (nDist >= 1 && nDist <= 16)
			GameConfig.ViewDistance = nDist;
		return ;
	}
	void	SetViewDistance(
			void*	Param)
	{
		GuiInputDialog("Specify view distance in chunks (x16.0m):",
				SetViewDistanceWorker);
		return ;
	}
	void	SetPixelRatioWorker(
			void*	Param)
	{
		std::string			NewRes = *((std::string*)Param);
		std::stringstream	sStream;
		double				nRatio;
		sStream << NewRes;
		sStream >> nRatio;
		if (nRatio >= 30 && nRatio <= 200)
			GameConfig.PixelRatio = nRatio;
		return ;
	}
	void	SetPixelRatio(
			void*	Param)
	{
		GuiInputDialog("Specify pixel ratio (pixels / 1.0m):",
				SetPixelRatioWorker);
		return ;
	}
	void	SetCameraDistToleranceWorker(
			void*	Param)
	{
		std::string			NewRes = *((std::string*)Param);
		std::stringstream	sStream;
		int					nDist;
		sStream << NewRes;
		sStream >> nDist;
		if (nDist >= 50 && nDist <= std::min(GameConfig.WindowWidth, GameConfig.WindowHeight) / 2)
			GameConfig.CameraDistTolerance = nDist;
		return ;
	}
	void	SetCameraDistTolerance(
			void*	Param)
	{
		GuiInputDialog("Specify maximum distance from Camera to player (in pixels):",
				SetCameraDistToleranceWorker);
		return ;
	}
	void	SetCameraSpeedWorker(
			void*	Param)
	{
		std::string			NewRes = *((std::string*)Param);
		std::stringstream	sStream;
		double				nSpeed;
		sStream << NewRes;
		sStream >> nSpeed;
		if (nSpeed >= 0.5 && nSpeed <= 10.0)
			GameConfig.CameraSpeed = nSpeed;
		return ;
	}
	void	SetCameraSpeed(
			void*	Param)
	{
		GuiInputDialog("Set camera movement speed:",
				SetCameraSpeedWorker);
		return ;
	}
	void	SetFullscreen(
				void*	Param)
	{
		GameConfig.WindowFullScreen ^= 1;
		glutFullScreenToggle();
		return ;
	}
	void	SetOpenToLan(
			void*	Param)
	{
		GameConfig.OpenToLan ^= 1;
		return ;
	}
	void	SetReturnToMenu(
			void*	Param)
	{
		GuiCtrl::GuiState = GuiCtrl::MainGui;
		return ;
	}
};

bool	graphicsRenderOptionsGui(
		void)
{
	using namespace graphicsRenderOptionsGuiInternals;
	GuiDeclareObject(GuiObject, pictBackground,
			- GameConfig.WindowWidth / 2, GameConfig.WindowWidth / 2,
			GameConfig.WindowHeight / 2, - GameConfig.WindowHeight / 2,
			"gui/misc/bg.png");
	GuiDeclareObject(GuiButton, buttonName,
			-540, 540, 216, 152,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetName);
	GuiDeclareObject(GuiButton, buttonViewDistance,
			-540, -10, 140, 76,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetViewDistance);
	GuiDeclareObject(GuiButton, buttonPixelRatio,
			10, 540, 140, 76,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetPixelRatio);
	GuiDeclareObject(GuiButton, buttonCameraDistTolerance,
			-540, -10, 64, 0,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetCameraDistTolerance);
	GuiDeclareObject(GuiButton, buttonCameraSpeed,
			10, 540, 64, 0,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetCameraSpeed);
	GuiDeclareObject(GuiButton, buttonResolution,
			-540, -10, -12, -76,
			"gui/button_vanilla/disabled.png", "gui/button_vanilla/disabled.png", "gui/button_vanilla/disabled.png",
			NULL, NULL);
	GuiDeclareObject(GuiButton, buttonFullscreen,
			10, 540, -12, -76,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetFullscreen);
	GuiDeclareObject(GuiButton, buttonOpenToLan,
			-540, 540, -88, -152,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetOpenToLan);
	GuiDeclareObject(GuiButton, buttonReturnToMenu,
			-320, 320, - GameConfig.WindowHeight / 2 + 128, - GameConfig.WindowHeight / 2 + 64,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetReturnToMenu);
	GuiDeclareObject(GuiFont, fontTitle,
			- 115, GameConfig.WindowHeight / 2 - 60, 1.0, 1.0, 1.0,
			50, ANSI_CHARSET, "OCR A Std",
			"Options");
	GuiDeclareObject(GuiFont, fontName,
			-500, 172, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Player Name : nul");
	GuiDeclareObject(GuiFont, fontViewDistance,
			-500, 96, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"View distance : nul");
	GuiDeclareObject(GuiFont, fontPixelRatio,
			50, 96, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Pixel ratio : nul");
	GuiDeclareObject(GuiFont, fontCameraDistTolerance,
			-500, 20, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Viewport tolerance : nul");
	GuiDeclareObject(GuiFont, fontCameraSpeed,
			50, 20, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Camera speed : nul");
	GuiDeclareObject(GuiFont, fontResolution,
			-500, -56, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Resolution : nul");
	GuiDeclareObject(GuiFont, fontFullscreen,
			50, -56, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Fullscreen : nul");
	GuiDeclareObject(GuiFont, fontOpenToLan,
			-500, -132, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Open to LAN : nul");
	GuiDeclareObject(GuiFont, fontReturnToMenu,
			-170, - GameConfig.WindowHeight / 2 + 84, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Return to Main Menu");
//	Set specific data
	fontName.SetContent("Player name : " + GameConfig.PlayerName);
	fontViewDistance.SetContent("View distance : " + ToString(GameConfig.ViewDistance));
	fontPixelRatio.SetContent("Pixel ratio : " + ToString(GameConfig.PixelRatio));
	fontCameraDistTolerance.SetContent("Viewport Tolerance : " + ToString(GameConfig.CameraDistTolerance));
	fontCameraSpeed.SetContent("Camera speed : " + ToString(GameConfig.CameraSpeed));
	fontResolution.SetContent("Resolution : " + ToString(GameConfig.WindowWidth) + "x" + ToString(GameConfig.WindowHeight));
	fontFullscreen.SetContent("Fullscreen : " + (GameConfig.WindowFullScreen ? (std::string)"True" : (std::string)"False"));
	fontOpenToLan.SetContent("Open To LAN : " + (GameConfig.OpenToLan ? (std::string)"True" : (std::string)"False"));
//	Render objects and buttons
	pictBackground.RenderObject();
	buttonName.RenderObject();
	buttonViewDistance.RenderObject();
	buttonPixelRatio.RenderObject();
	buttonCameraDistTolerance.RenderObject();
	buttonCameraSpeed.RenderObject();
	buttonResolution.RenderObject();
	buttonFullscreen.RenderObject();
	buttonOpenToLan.RenderObject();
	buttonReturnToMenu.RenderObject();
//	Render font objects
	fontTitle.RenderObject();
	fontName.RenderObject();
	fontViewDistance.RenderObject();
	fontPixelRatio.RenderObject();
	fontCameraDistTolerance.RenderObject();
	fontCameraSpeed.RenderObject();
	fontResolution.RenderObject();
	fontFullscreen.RenderObject();
	fontOpenToLan.RenderObject();
	fontReturnToMenu.RenderObject();
	return true;
}

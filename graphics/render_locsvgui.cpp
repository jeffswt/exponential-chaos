
//	"render_locsvgui.cpp" -*- C++ -*-
//	Created on: 23 April 2016
//	Interface manager for looking up local saves.
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

#include <algorithm>
#include <thread>

namespace graphicsRenderLocalSaveGuiInternals
{
	GuiButton				buttonsGameMap[20];
	GuiFont					fontsGameMap[20];
	std::vector<GameMap*>	MapList;
	int						CurPage, CurSelection;
	int						PageCapacity, CurPageCapacity;
	bool					Loaded = false;
	void	SetReturnToMenu(
			void*	Param)
	{
		Loaded = false;
		GuiCtrl::GuiState = GuiCtrl::MainGui;
		return ;
	}
	void	SetMapSelection(
			void*	Param)
	{
		buttonsGameMap[CurSelection].SetProperties(
				- GameConfig.WindowWidth / 2 + 60, -24,
				GameConfig.WindowHeight / 2 - 76 * CurSelection - 128,
				GameConfig.WindowHeight / 2 - 76 * CurSelection - 192,
				"gui/button_vanilla/normal.png",
				"gui/button_vanilla/hovering.png",
				"gui/button_vanilla/pressed.png",
				NULL, SetMapSelection,
				NULL, (void*)(long long)CurSelection);
		CurSelection = (long long)Param;
		if (CurSelection < 0 || CurSelection > CurPageCapacity)
			CurSelection = 0;
		buttonsGameMap[CurSelection].SetProperties(
				- GameConfig.WindowWidth / 2 + 60, -24,
				GameConfig.WindowHeight / 2 - 76 * CurSelection - 128,
				GameConfig.WindowHeight / 2 - 76 * CurSelection - 192,
				"gui/button_vanilla/pressed.png",
				"gui/button_vanilla/pressed.png",
				"gui/button_vanilla/pressed.png",
				NULL, NULL, NULL, NULL);
		return ;
	}
	void	SetPageSelection(
			void*	Param)
	{
		if ((long long)Param == 1) CurPage--;
		else CurPage++;
//		Avoid overflow or SIGSEGV
		while (CurPage < 0) CurPage++;
		while (CurPage * PageCapacity >= (int)MapList.size() && CurPage > 0) CurPage--;
		if ((CurPage + 1) * PageCapacity >= (int)MapList.size())
			CurPageCapacity = MapList.size() % PageCapacity;
		else
			CurPageCapacity = PageCapacity;
		if (CurSelection > PageCapacity)
			SetMapSelection((void*)(long long)(CurPageCapacity - 1));
//		Updating map names
		for (int i = 0; i < CurPageCapacity; i++)
			fontsGameMap[i].SetContent(MapList[CurPage * PageCapacity + i]->Name);
		return ;
	}
	void	SetPlayMapWorker(
			GameMap*	playMap)
	{
		GuiCtrl::workMainMap = playMap;
		playMap->Clear();
		playMap->ReimportFromJson();
		BeginPhysicsEngine(playMap);
		WaitForEngineEvent();
		GuiCtrl::GuiState = GuiCtrl::Game;
		return ;
	}
	void	SetPlayMap(
			void*	Param)
	{
		Loaded = false;
		int	Idx = CurPage * PageCapacity + CurSelection;
		GameMap*	playMap = MapList[Idx];
		playMap->IsHost = true;
		if (!playMap) return ;
		GuiDrawLoadingDialog("Loading world, please wait...");
		std::thread*	hWorker = new std::thread(SetPlayMapWorker, playMap);
		if (!hWorker) throw STLException();
		return ;
	}
	bool	my_sort_map_time(GameMap* __x, GameMap* __y)
		{ return __x->ModifyTime > __y->ModifyTime; };
	void	PostUpdateMapInfo(
			bool	UpdateMapData)
	{
		if (UpdateMapData) {
//			Cleanup all previously used game maps...
			for (auto itert = MapList.begin(); itert != MapList.end(); itert++) {
				GameMap*	gmWorker = *itert;
				gmWorker->Clear();
				delete gmWorker;
			}
			MapList.clear();
//			Importing game maps and updating buttons...
			ImportGameMapHeaders(MapList);
		}
		std::sort(MapList.begin(), MapList.end(), my_sort_map_time);
		PageCapacity = (GameConfig.WindowHeight - 128 - 30) / 76;
		CurPageCapacity = std::min(PageCapacity, (int)MapList.size());
		CurPage = CurSelection = 0;
		for (int i = 0; i < PageCapacity; i++) {
			buttonsGameMap[i].SetProperties(
					- GameConfig.WindowWidth / 2 + 60, -24,
					GameConfig.WindowHeight / 2 - 76 * i - 128,
					GameConfig.WindowHeight / 2 - 76 * i - 192,
					"gui/button_vanilla/normal.png",
					"gui/button_vanilla/hovering.png",
					"gui/button_vanilla/pressed.png",
					NULL, SetMapSelection,
					NULL, (void*)(long long)i);
			if (i == CurSelection)
				buttonsGameMap[i].SetProperties(
					- GameConfig.WindowWidth / 2 + 60, -24,
					GameConfig.WindowHeight / 2 - 76 * i - 128,
					GameConfig.WindowHeight / 2 - 76 * i - 192,
					"gui/button_vanilla/pressed.png",
					"gui/button_vanilla/pressed.png",
					"gui/button_vanilla/pressed.png",
					NULL, NULL, NULL, NULL);
			fontsGameMap[i].SetProperties(
					- GameConfig.WindowWidth / 2 + 80,
					GameConfig.WindowHeight / 2 - 76 * i - 172,
					1.0, 1.0, 1.0,
					24, ANSI_CHARSET, "OCR A Std");
		}
		for (int i = 0; i < CurPageCapacity; i++)
			fontsGameMap[i].SetContent(MapList[i]->Name);
		return ;
	}
}

bool	graphicsRenderLocalSaveGui(
		void)
{
	using namespace graphicsRenderLocalSaveGuiInternals;
	if (Loaded == false) {
		PostUpdateMapInfo(true);
		Loaded = true;
	}
//	Defining some items...
	static	int	buttonUseMapLastSet = 2;
	static	int	buttonPrevPageLastSet = 2;
	static	int	buttonNextPageLastSet = 2;
	static	GuiObject	pictBackground;
	bool	RequireUpdate = false;
	if (pictBackground.RequireUpdate())
		RequireUpdate = true,
		pictBackground.SetProperties(
			- GameConfig.WindowWidth / 2, GameConfig.WindowWidth / 2,
			GameConfig.WindowHeight / 2, - GameConfig.WindowHeight / 2,
			"gui/misc/bg.png");
	GuiDeclareObject(GuiButton, buttonReturnToMenu,
			24, GameConfig.WindowWidth / 2 - 60, GameConfig.WindowHeight / 2 - 128, GameConfig.WindowHeight / 2 - 192,
			"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
			NULL, SetReturnToMenu);
	static	GuiButton	buttonPlayMap;
	static	GuiButton	buttonPrevPage;
	static	GuiButton	buttonNextPage;
	GuiDeclareObject(GuiFont, fontTitle,
			- 176, GameConfig.WindowHeight / 2 - 60, 1.0, 1.0, 1.0,
			50, ANSI_CHARSET, "OCR A Std",
			"Select map");
	GuiDeclareObject(GuiFont, fontReturnToMenu,
			44, GameConfig.WindowHeight / 2 - 172, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Return to Main Menu");
	GuiDeclareObject(GuiFont, fontPlayMap,
			44, GameConfig.WindowHeight / 2 - 248, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Play with selected save");
	GuiDeclareObject(GuiFont, fontPrevPage,
			44, GameConfig.WindowHeight / 2 - 324, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Last Page");
	GuiDeclareObject(GuiFont, fontNextPage,
			324, GameConfig.WindowHeight / 2 - 324, 1.0, 1.0, 1.0,
			28, ANSI_CHARSET, "OCR A Std",
			"Next Page");
//	Set specific data
	if (RequireUpdate)
		PostUpdateMapInfo(false);
	if (CurPageCapacity <= 0 && (buttonUseMapLastSet != 0 || RequireUpdate)) {
		buttonPlayMap.SetProperties(
				24, GameConfig.WindowWidth / 2 - 60, GameConfig.WindowHeight / 2 - 204, GameConfig.WindowHeight / 2 - 268,
				"gui/button_vanilla/disabled.png", "gui/button_vanilla/disabled.png", "gui/button_vanilla/disabled.png",
				NULL, NULL, NULL, NULL);
		buttonUseMapLastSet = 0;
	} else if (CurPageCapacity > 0 && (buttonUseMapLastSet != 1 || RequireUpdate)) {
		buttonPlayMap.SetProperties(
				24, GameConfig.WindowWidth / 2 - 60, GameConfig.WindowHeight / 2 - 204, GameConfig.WindowHeight / 2 - 268,
				"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
				NULL, SetPlayMap, NULL, NULL);
		buttonUseMapLastSet = 1;
	}
	if (CurPage <= 0 && (buttonPrevPageLastSet != 0 || RequireUpdate)) {
		buttonPrevPage.SetProperties(
				24, 280, GameConfig.WindowHeight / 2 - 280, GameConfig.WindowHeight / 2 - 324,
				"gui/button_vanilla/disabled.png", "gui/button_vanilla/disabled.png", "gui/button_vanilla/disabled.png",
				NULL, NULL, NULL, NULL);
		buttonPrevPageLastSet = 0;
	} else if (CurPage > 0 && (buttonPrevPageLastSet != 1 || RequireUpdate)) {
		buttonPrevPage.SetProperties(
				24, 280, GameConfig.WindowHeight / 2 - 280, GameConfig.WindowHeight / 2 - 324,
				"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
				NULL, SetPageSelection, NULL, (void*)1);
		buttonPrevPageLastSet = 1;
	}
	if ((CurPage + 1) * PageCapacity >= (int)MapList.size() && (buttonNextPageLastSet != 0 || RequireUpdate)) {
		buttonNextPage.SetProperties(
				304, GameConfig.WindowWidth / 2 - 60, GameConfig.WindowHeight / 2 - 280, GameConfig.WindowHeight / 2 - 324,
				"gui/button_vanilla/disabled.png", "gui/button_vanilla/disabled.png", "gui/button_vanilla/disabled.png",
				NULL, NULL, NULL, NULL);
		buttonNextPageLastSet = 0;
	} else if ((CurPage + 1) * PageCapacity < (int)MapList.size() && (buttonNextPageLastSet != 1 || RequireUpdate)){
		buttonNextPage.SetProperties(
				304, GameConfig.WindowWidth / 2 - 60, GameConfig.WindowHeight / 2 - 280, GameConfig.WindowHeight / 2 - 324,
				"gui/button_vanilla/normal.png", "gui/button_vanilla/hovering.png", "gui/button_vanilla/pressed.png",
				NULL, SetPageSelection, NULL, (void*)2);
		buttonNextPageLastSet = 1;
	}
//	Render objects and buttons
	pictBackground.RenderObject();
	buttonReturnToMenu.RenderObject();
	buttonPlayMap.RenderObject();
	buttonPrevPage.RenderObject();
	buttonNextPage.RenderObject();
	for (int i = 0; i < CurPageCapacity; i++)
		buttonsGameMap[i].RenderObject();
//	Render fonts
	fontTitle.RenderObject();
	fontReturnToMenu.RenderObject();
	fontPlayMap.RenderObject();
	fontPrevPage.RenderObject();
	fontNextPage.RenderObject();
	for (int i = 0; i < CurPageCapacity; i++)
		fontsGameMap[i].RenderObject();
//	Render map descriptions
	std::string	mapDesc = "";
	int			len = (GameConfig.WindowWidth / 2 - 84 - 12) / 18;
	GameMap*	MainMap = MapList[CurPage * PageCapacity + CurSelection];
	mapDesc = "Modified: " + GetSystemTimeString(MainMap->ModifyTime) + "\n";
	for (unsigned int i = 0; i < MainMap->Description.length(); i++) {
		if (i % len == 0) mapDesc += '\n';
		mapDesc += MainMap->Description[i];
	}
	GuiDrawTooltip(24, GameConfig.WindowHeight / 2 - 356, mapDesc);
	return true;
}

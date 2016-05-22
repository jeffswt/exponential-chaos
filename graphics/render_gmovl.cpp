
//	"render_gmovl.cpp" -*- C++ -*-
//	Created on: 1 May 2016
//	Game overlay renderer.
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

#include "network/netsync.h"

#include "graphics/render_private.h"

#include "native/keyboard.h"
#include "network/chat.h"

//#define RENDER_GMOVL_DISPLAY_ITEM_DESCRIPTION_

extern	std::map<std::string, EntityType*>	EntityTypes;

namespace graphicsRenderInventoryInternals
{
	int				CurPage, ItemCount;
	Entity*			PlayerEnt;
	PlayerEntity*	PlayerExt;
	GuiObject		ItemPics[45];
#ifdef RENDER_GMOVL_DISPLAY_ITEM_DESCRIPTION_
	GuiFont			ItemDesc[45];
#endif
	GuiButton		Activators[45];
	std::vector<std::string>	PendTooltips;
//	Declarations...
	void	UpdateToSlot(EntityType*);
	void	buttonInitSlot(void*);
	void	buttonDrawTooltip(void*);
	void	PostUpdateInventoryView(void);
//	Definitions...
	void	UpdateToSlot(
			EntityType*	EntTyp)
	{
		int	hotBarItems = std::min(9, (int)PlayerExt->Inventory.size());
		if (PlayerExt->IsCreative) {
			for (int i = 0; i < hotBarItems; i++)
				if (EntTyp == PlayerExt->Inventory[i].first) {
//					Already in the hotbar, don't need to insert.
					PlayerExt->InventoryFocus = i + 1;
					return ;
				}
//			The required entity type is not in the hotbar, now attempting
//			to insert into the inventory.
			PlayerExt->InventoryFocus = 1;
			PlayerExt->Inventory.insert(PlayerExt->Inventory.begin(),
					std::make_pair(EntTyp, 64));
			while (PlayerExt->Inventory.size() > 9)
				PlayerExt->Inventory.erase(--PlayerExt->Inventory.end());
		} else {
			auto	itert = PlayerExt->Inventory.begin();
			auto	inspair = std::make_pair(EntTyp, 1);
			for (; itert != PlayerExt->Inventory.end(); itert++)
				if (itert->first == EntTyp)
					break;
//			Now we shall create a copy and re-insert it into the hotbar.
			if (itert != PlayerExt->Inventory.end()) {
				inspair = *itert;
				PlayerExt->Inventory.erase(itert);
				PlayerExt->Inventory.insert(PlayerExt->Inventory.begin(), inspair);
			}
		}
		PostUpdateInventoryView();
		return ;
	}
	void	buttonInitSlot(
			void*	Params)
	{
		EntityType*	EntTyp;
		EntTyp = (EntityType*)Params;
		UpdateToSlot(EntTyp);
		return ;
	}
	void	buttonDrawTooltip(
			void*	Params)
	{
		EntityType*	EntTyp;
		EntTyp = (EntityType*)Params;
		PendTooltips.push_back(EntTyp->Properties.Description);
		return ;
	}
	void	PostUpdateInventoryView(
			void)
	{
		int	lBegin = -356 + 64 / 2 + 4;
		int	uBegin = 227 - 136 / 2 - 4;
		int	cntBegin = CurPage * 45;
#ifdef RENDER_GMOVL_DISPLAY_ITEM_DESCRIPTION_
		for (int i = 0; i < 45; i++)
			ItemDesc[i].SetProperties(
				lBegin + (i % 9) * 72, uBegin - (i / 9 + 1) * 72 + 8, 0.0, 0.0, 0.0,
				16, ANSI_CHARSET, "Times New Roman", "");
#endif
		if (PlayerExt->IsCreative) {
			int		actualSize = 0;
//			Some entities are not welcomed by us...
			for (auto i = EntityTypes.begin(); i != EntityTypes.end(); i++) {
				if (i->second->DataIntact()
						&& i->second->Properties.ShowInCreative)
					actualSize++;
			}
			while (actualSize <= CurPage * 45 && CurPage > 0) CurPage--, cntBegin -= 45;
			ItemCount = std::min(std::max(0, actualSize - CurPage * 45), 45);
			int		itrtCnt = 0;
			auto	itert = EntityTypes.begin();
			for (; itert != EntityTypes.end() && itrtCnt < cntBegin; itert++)
				if (itert->second->DataIntact()
						&& itert->second->Properties.ShowInCreative)
					itrtCnt++;
			for (int i = 0; i < ItemCount && itert != EntityTypes.end(); itert++)
			if (itert->second->DataIntact()
					&& itert->second->Properties.ShowInCreative) {
				std::string	fontDesc = itert->second->Properties.Name;
				ItemPics[i].SetProperties(
					lBegin + (i % 9) * 72, lBegin + (i % 9 + 1) * 72 - 8,
					uBegin - (i / 9) * 72, uBegin - (i / 9 + 1) * 72 + 8,
					itert->second->Graphics.TextureOnHand);
#ifdef RENDER_GMOVL_DISPLAY_ITEM_DESCRIPTION_
				ItemDesc[i].SetContent(fontDesc);
#endif
				Activators[i].SetProperties(
					lBegin + (i % 9) * 72, lBegin + (i % 9 + 1) * 72 - 8,
					uBegin - (i / 9) * 72, uBegin - (i / 9 + 1) * 72 + 8,
					"", "", "", buttonDrawTooltip, buttonInitSlot,
					(void*)(itert->second), (void*)(itert->second));
				i++; // Post update...
			}
		} else {
			while ((int)PlayerExt->Inventory.size() <= CurPage * 45 &&
					CurPage > 0) CurPage--, cntBegin -= 45;
			ItemCount = std::min(std::max(0,
					(int)PlayerExt->Inventory.size() -
					CurPage * 45), 45);
			for (int i = 0; i < ItemCount; i++) {
				std::string	fontDesc = PlayerExt->Inventory[cntBegin + i].first
						->Properties.Name;
				ItemPics[i].SetProperties(
					lBegin + (i % 9) * 72, lBegin + (i % 9 + 1) * 72 - 8,
					uBegin - (i / 9) * 72, uBegin - (i / 9 + 1) * 72 + 8,
					PlayerExt->Inventory[cntBegin + i].first->Graphics.TextureOnHand);
#ifdef RENDER_GMOVL_DISPLAY_ITEM_DESCRIPTION_
				ItemDesc[i].SetContent(fontDesc);
#endif
				Activators[i].SetProperties(
					lBegin + (i % 9) * 72, lBegin + (i % 9 + 1) * 72 - 8,
					uBegin - (i / 9) * 72, uBegin - (i / 9 + 1) * 72 + 8,
					"", "", "", buttonDrawTooltip, buttonInitSlot,
					(void*)(PlayerExt->Inventory[cntBegin + i].first),
					(void*)(PlayerExt->Inventory[cntBegin + i].first));
			}
		}
//		As the selection might have changed, we are informing the server of the
//		inventory change...
		NetmgrInsertEntity(PlayerEnt);
		return ;
	}
}

bool	graphicsRenderChatMsg(
		void)
{
#define	my_define_font(_x, _y)\
	GuiDeclareObject(GuiFont, _x,\
		-GameConfig.WindowWidth / 2 + 20, GameConfig.WindowHeight / 2 - _y * 28,\
		0.251, 0.349, 0.596, 20, ANSI_CHARSET, "OCR A Std");
	std::vector<std::string>	Vec;
	using namespace std;
	chatRetrieveMessageList(Vec);
	for (int i = 0; i < 5; i++) Vec.push_back("");
	my_define_font(fontLine1, 1);
	my_define_font(fontLine2, 2);
	my_define_font(fontLine3, 3);
	my_define_font(fontLine4, 4);
	my_define_font(fontLine5, 5);
//	Setting font content
	fontLine1.SetContent(Vec[0]);
	fontLine2.SetContent(Vec[1]);
	fontLine3.SetContent(Vec[2]);
	fontLine4.SetContent(Vec[3]);
	fontLine5.SetContent(Vec[4]);
//	Rendering objects...
	fontLine1.RenderObject();
	fontLine2.RenderObject();
	fontLine3.RenderObject();
	fontLine4.RenderObject();
	fontLine5.RenderObject();
	return true;
}

bool	graphicsRenderInventory(
		GameMap*		MainMap,
		Entity*			nPlayerEnt,
		PlayerEntity*	nPlayerExt)
{
	using namespace graphicsRenderInventoryInternals;
	PlayerEnt = nPlayerEnt;
	PlayerExt = nPlayerExt;
//	These settings do not change with the resolution, so we don't need to
//	update these settings when required to update.
	static	GuiObject	pictInventoryBg;
	static	int			LastSize = 0;
	static	GameMap*	LastMap = NULL;
	bool	RequireUpdate = false;
	if (pictInventoryBg.RequireUpdate())
		RequireUpdate = true;
	if (LastSize != (int)PlayerExt->Inventory.size()) {
		LastSize = PlayerExt->Inventory.size();
		RequireUpdate = true;
	}
//	Okay, so if you see that when switching between maps, there seems to
//	be something strange occurs...
	if (LastMap != MainMap) {
		LastMap = MainMap;
		RequireUpdate = true;
	}
	if (RequireUpdate) {
		pictInventoryBg.SetProperties(-356, 356, 227, -227,
			"gui/inventory/menu.png");
		PostUpdateInventoryView();
	}
//	Begin rendering
	pictInventoryBg.RenderObject();
	for (int i = 0; i < ItemCount; i++)
		ItemPics[i].RenderObject();
#ifdef RENDER_GMOVL_DISPLAY_ITEM_DESCRIPTION_
	for (int i = 0; i < ItemCount; i++)
		ItemDesc[i].RenderObject();
#endif
	for (int i = 0; i < ItemCount; i++)
		Activators[i].RenderObject();
	for (std::string str : PendTooltips)
		GuiDrawTooltip(str);
	PendTooltips.clear();
//	Some other things we need to take care about
	if ((kGetKeyOnpress('A') ||
			kGetKeyOnpress('W') ||
			kGetKeyOnpress(VK_UP) ||
			kGetKeyOnpress(VK_LEFT))
			&& CurPage > 0) {
		CurPage--;
		PostUpdateInventoryView();
	} else if (kGetKeyOnpress('D') ||
			kGetKeyOnpress('S') ||
			kGetKeyOnpress(VK_DOWN) ||
			kGetKeyOnpress(VK_RIGHT)) {
		CurPage++;
		PostUpdateInventoryView();
	}
	return true;
}

bool	graphicsRenderGameOverlay(
		GameMap*	MainMap)
{
	if (PhEngine::PhEngineState == PhEngine::Stopped) {
		GuiCtrl::GuiState = GuiCtrl::MainGui;
		return false;
	}
	Entity*			PlayerEnt;
	PlayerEntity*	PlayerExt;
	PlayerEnt = MainMap->PlayerEntityList[InputControl.PlayerGuid];
	if (!PlayerEnt) throw NullPointerException();
	PlayerExt = (PlayerEntity*)PlayerEnt->Physics.ExtendedTags;
	if (!PlayerExt) throw NullPointerException();
//	Datum received, ready to draw a few overlays
	int		hotBarTop, hotBarBot;
	int		hotBarItems;
	bool	RequireUpdate = false;
	hotBarTop = - GameConfig.WindowHeight / 2 + 64;
	hotBarBot = - GameConfig.WindowHeight / 2;
	hotBarItems = std::min(9, (int)PlayerExt->Inventory.size());
//	Object declarations
	static	GuiObject	objHotbarBg;
	static	GuiObject	objHotbarItems[9];
	if (objHotbarBg.RequireUpdate()) {
		objHotbarBg.SetProperties(
			-288, 288, hotBarTop, hotBarBot,
			"gui/inventory/hotbar.png");
		RequireUpdate = true;
	}
	GuiDeclareObject(GuiObject, objHotbarFocus,
			-288, -224, hotBarTop, hotBarBot,
			"gui/inventory/hotbarfocus.png");
	for (int i = 0; i < hotBarItems; i++)
		objHotbarItems[i].SetProperties(
			-288 + i * 64 + 8, -224 + i * 64 - 8, hotBarTop - 8, hotBarBot + 8,
			PlayerExt->Inventory[i].first->Graphics.TextureOnHand);
	static	GuiFont		fontItemCount[9];
	if (RequireUpdate)
		for (int i = 0; i < 9; i++)
			fontItemCount[i].SetProperties(
				-288 + i * 64 + 8, hotBarBot + 8, 0.456, 0.984, 0.984,
				20, ANSI_CHARSET, "OCR A Std");
	GuiDeclareObject(GuiFont, fontPlayerLife,
			-288, hotBarTop, 0.456, 0.984, 0.984,
			20, ANSI_CHARSET, "OCR A Std");
	if (true) {
		std::stringstream	Stream;
		std::string			NewStr = "";
		Stream << "(" << (int)PlayerEnt->Physics.PosX
			   << ", " << (int)PlayerEnt->Physics.PosY
			   << ", " << PlayerEnt->Properties.Layer
			   << ")";
		getline(Stream, NewStr);
		if (!PlayerExt->IsCreative) {
			std::stringstream	Stream;
			std::string			WorkStr;
			Stream << PlayerExt->Life;
			Stream >> WorkStr;
			WorkStr = "Life Remaining: " + WorkStr;
			if (PlayerExt->Life < 0)
				WorkStr = "You died!";
			NewStr = WorkStr + " " + NewStr;
		}
		fontPlayerLife.SetContent(NewStr);
	}
	for (int i = 0; i < hotBarItems; i++) {
		std::stringstream	Stream;
		std::string			WorkStr;
		int					CurCount;
		CurCount = PlayerExt->Inventory[i].second;
		if (CurCount > 999 || PlayerExt->IsCreative)
			Stream << "INF";
		else
			Stream << CurCount;
		Stream >> WorkStr;
		fontItemCount[i].SetContent(WorkStr);
	}
//	Specific settings
	objHotbarFocus.SetProperties(
			-352 + PlayerExt->InventoryFocus * 64, -288 + PlayerExt->InventoryFocus * 64,
			hotBarTop, hotBarBot, "gui/inventory/hotbarfocus.png");
//	Post renderer data to classes
	objHotbarBg.RenderObject();
	for (int i = 0; i < hotBarItems; i++)
		objHotbarItems[i].RenderObject();
	objHotbarFocus.RenderObject();
	for (int i = 0; i < hotBarItems; i++)
		fontItemCount[i].RenderObject();
	fontPlayerLife.RenderObject();
	graphicsRenderChatMsg();
	if (PhEngine::PhEngineState == PhEngine::InventoryOpen) {
		graphicsRenderInventory(MainMap, PlayerEnt, PlayerExt);
		if (kGetKeyOnpress('E') || kGetKeyOnpress(VK_ESCAPE))
			PhEngine::PhEngineState = PhEngine::Running;
	}
	if (PhEngine::PhEngineState == PhEngine::Running
			&& kGetKeyOnpress('T')) {
		PhEngine::PhEngineState = PhEngine::InventoryOpen;
		std::string	UpdMsg = "";
		MainMap->UnlockMemory();
		UpdMsg = GuiInputDialog("Broadcast chat message:");
		MainMap->LockMemory();
		UpdMsg = "[" + GameConfig.PlayerName + "] " + UpdMsg;
		if (UpdMsg.length() > 0) {
			if (MainMap->IsHost)
				chatInsertMessage(UpdMsg);
			NetmgrPostMessage(UpdMsg);
		}
		PhEngine::PhEngineState = PhEngine::Running;
	}
	return true;
}

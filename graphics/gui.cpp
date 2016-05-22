
//	"gui.cpp" -*- C++ -*-
//	Created on: 16 April 2016
//	Definitions of GUI objects' rendering.
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

#include "graphics/gui.h"

#include <map>
#include <set>
#include <sstream>
#include <vector>

#include "algo/triple_pair.h"
#include "native/keyboard.h"
#include "native/keystate2ascii.h"
#include "native/inputctrl.h"
#include "native/texture_png.h"
#include "physics/config.h"

extern	std::string		CurrentDirectory;
extern	InputCtrlType	InputControl;
extern	ConfigType		GameConfig;

bool	RenderUpdater(
		int		op,
		bool*	pntr)
{
	static	std::set<bool*>	RendererUpdater;
	if (op == 1)
		RendererUpdater.insert(pntr);
	else if (op == 2)
		RendererUpdater.erase(pntr);
	else if (op == 3)
		for (auto itert : RendererUpdater)
			*(itert) = true;
	return true;
}

std::string	GuiInputDialog(
		std::string	Notice)
{
	GuiDeclareObject(GuiObject, pictBackground,
		- GameConfig.WindowWidth / 2, GameConfig.WindowWidth / 2,
		GameConfig.WindowHeight / 2, - GameConfig.WindowHeight / 2,
		"gui/misc/bg.png");
	GuiDeclareObject(GuiObject, pictBanner,
			-480, 480, 32, -32,
			"gui/misc/tooltip.png");
	GuiDeclareObject(GuiFont, fontTitle,
			- 480, 60, 1.0, 1.0, 1.0,
			30, ANSI_CHARSET, "OCR A Std",
			"null");
	GuiDeclareObject(GuiFont, fontCurEnter,
			- 480, -12, 1.0, 1.0, 1.0,
			50, ANSI_CHARSET, "OCR A Std",
			"null");
	fontTitle.SetContent(Notice);
	std::string	Ret = "";
	double	beginTime = GetProcessTime();
	double	endTime = beginTime;
	while (true) {
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		fontCurEnter.SetContent(Ret);
		int	CurKey = KeystateToAscii();
		if (CurKey == '\r') {
			std::string Newer = "";
			for (unsigned int i = 0; i < Ret.length() - 1; i++)
				Newer += Ret[i];
			Ret = Newer;
		} else if (CurKey == '\n') {
			break;
		} else if (CurKey != 0) {
			Ret += (char)CurKey;
		}
		pictBackground.RenderObject();
		pictBanner.RenderObject();
		fontTitle.RenderObject();
		fontCurEnter.RenderObject();
		glFlush();
		if (kGetKeyOnpress(VK_ESCAPE))
			return "";
//		Set timeout, that will cause unresponsiveness
		endTime = GetProcessTime();
		if (endTime - beginTime > 5.0)
			return "";
		SleepForTime(0.015);
	}
	return Ret;
}

bool	GuiDrawTooltip(
		int			begX,
		int			begY,
		std::string	Notice)
{
	static	GuiObject	pictBg;
	static	GuiFont		fontNote;
//	Processing lines...
	Notice += "\n";
	std::vector<std::string> lines;
	std::string	curproc = "";
	int	maxlen = 0;
	for (char i : Notice) {
		if (i == '\n') {
			maxlen = std::max(maxlen, (int)curproc.length());
			lines.push_back(curproc);
			curproc = "";
		} else {
			curproc += i;
		}
	}
//	Rendering the lines...
	pictBg.SetProperties(begX, begX + maxlen * 18 + 10,
			begY, begY - 40 * lines.size() - 16, "gui/misc/tooltip.png");
	pictBg.RenderObject();
	for (unsigned int i = 0; i < lines.size(); i++) {
		curproc = lines[i];
		fontNote.SetProperties(
				begX + 8, begY - 30 - 40 * i, 1.0, 1.0, 1.0,
				26, ANSI_CHARSET, "OCR A Std");
		fontNote.SetContent(curproc);
		fontNote.RenderObject();
	}
	return true;
}

bool	GuiDrawTooltip(
		std::string	Content)
{
	int	begX = InputControl.MouseX - GameConfig.WindowWidth / 2,
		begY = GameConfig.WindowHeight / 2 - InputControl.MouseY;
	return GuiDrawTooltip(begX, begY, Content);
}

bool	GuiDrawLoadingDialog(
		std::string	Notice)
{
	GuiDeclareObject(GuiObject, pictBackground,
		- GameConfig.WindowWidth / 2, GameConfig.WindowWidth / 2,
		GameConfig.WindowHeight / 2, - GameConfig.WindowHeight / 2,
		"gui/misc/bg.png");
	GuiDeclareObject(GuiFont, fontTitle,
			- 480, 60, 1.0, 1.0, 1.0,
			30, ANSI_CHARSET, "OCR A Std",
			"null");
	fontTitle.SetContent(Notice);
	glFlush();
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	pictBackground.RenderObject();
	fontTitle.RenderObject();
	glFlush();
	return true;
}

bool	GuiObject::RequireUpdate(void)
	{ return vlRequireUpdate; }

bool	GuiObject::ForceUpdate(void)
	{ vlRequireUpdate = true; return true; }

bool	GuiObject::LoadTexture(
		std::string RelativePath)
{
	if (RelativePath == "") {
		Tex = 0;
		return true;
	}
	GLuint	WorkTex = 0;
	std::string WorkPath = CurrentDirectory + "textures/" + RelativePath;
	WorkTex = LoadPNGTexture(WorkPath.c_str(), NULL, NULL);
	if (!WorkTex)
		return false;
	Tex = WorkTex;
	return true;
}

bool	GuiObject::RenderObject(
		void)
{
	if (Tex != 0) {
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, Tex);
		glBegin(GL_POLYGON);
			glTexCoord2d(0.0, 0.0);
			glVertex2f(LBound, DBound);
			glTexCoord2d(1.0, 0.0);
			glVertex2f(RBound, DBound);
			glTexCoord2d(1.0, 1.0);
			glVertex2f(RBound, UBound);
			glTexCoord2d(0.0, 1.0);
			glVertex2f(LBound, UBound);
		glEnd();
	}
	return true;
}

bool	GuiObject::SetProperties(
		int			lBnd,
		int			rBnd,
		int			uBnd,
		int			dBnd,
		std::string	TexPath)
{
	LBound = lBnd;
	RBound = rBnd;
	UBound = uBnd;
	DBound = dBnd;
	LoadTexture(TexPath);
	vlRequireUpdate = false;
	return true;
}

bool	GuiObject::SetProperties(
		int		lBnd,
		int		rBnd,
		int		uBnd,
		int		dBnd,
		GLuint	NewTex)
{
	LBound = lBnd;
	RBound = rBnd;
	UBound = uBnd;
	DBound = dBnd;
	Tex = NewTex;
	vlRequireUpdate = false;
	return true;
}

bool	GuiObject::SetProperties(
		int		lBnd,
		int		rBnd,
		int		uBnd,
		int		dBnd)
{
	LBound = lBnd;
	RBound = rBnd;
	UBound = uBnd;
	DBound = dBnd;
	vlRequireUpdate = false;
	return true;
}

GuiObject::GuiObject(
		void)
{
	LBound = RBound = UBound = DBound = 0;
	Tex = 0;
//	Updater settings
	RenderUpdater(1, &vlRequireUpdate);
	vlRequireUpdate = true;
	return ;
}

GuiObject::GuiObject(
		int			lBnd,
		int			rBnd,
		int			uBnd,
		int			dBnd,
		std::string	TexPath)
{
	SetProperties(lBnd, rBnd, uBnd, dBnd, TexPath);
//	Updater settings
	RenderUpdater(1, &vlRequireUpdate);
	vlRequireUpdate = false;
	return ;
}

GuiObject::~GuiObject(
		void)
{
	RenderUpdater(2, &vlRequireUpdate);
	return ;
}

bool	GuiButton::RequireUpdate(void)
	{ return vlRequireUpdate; }

bool	GuiButton::ForceUpdate(void)
	{ vlRequireUpdate = true; return true; }

bool	GuiButton::RenderObject(
		void)
{
	int	State = 0;
	if (InputControl.MouseX >= GameConfig.WindowWidth / 2 + LBound &&
			InputControl.MouseX <= GameConfig.WindowWidth / 2 + RBound &&
			InputControl.MouseY >= GameConfig.WindowHeight / 2 - UBound &&
			InputControl.MouseY <= GameConfig.WindowHeight / 2 - DBound)
		State = 1;
	if (State == 1 &&
			InputControl.LDown)
		State = 2;
	switch (State) {
	case 0:
		NormalObj.RenderObject();
		LastDown = false;
		break;
	case 1:
		HoverObj.RenderObject();
		if (LastDown && PressedFun != NULL)
			PressedFun(PressedParam);
		if (HoverFun != NULL)
			HoverFun(HoverParam);
		LastDown = false;
		break;
	case 2:
		PressedObj.RenderObject();
		if (HoverFun != NULL)
			HoverFun(HoverParam);
		LastDown = true;
		break;
	default:
		throw InstructionFailureException();
	}
	return true;
}

bool	GuiButton::SetProperties(
		int			lBnd,
		int			rBnd,
		int			uBnd,
		int			dBnd,
		GLuint		NormalTex,
		GLuint		HoverTex,
		GLuint		PressedTex,
		GuiFunction	HoverFunction,
		GuiFunction	PressedFunction,
		void*		HoverParameters,
		void*		PressedParameters)
{
	NormalObj.SetProperties(lBnd, rBnd, uBnd, dBnd, NormalTex);
	HoverObj.SetProperties(lBnd, rBnd, uBnd, dBnd, HoverTex);
	PressedObj.SetProperties(lBnd, rBnd, uBnd, dBnd, PressedTex);
	LBound = lBnd, RBound = rBnd, UBound = uBnd, DBound = dBnd;
	LastDown = false;
	HoverFun = HoverFunction;
	PressedFun = PressedFunction;
	HoverParam = HoverParameters;
	PressedParam = PressedParameters;
	vlRequireUpdate = false;
	return true;
}

bool	GuiButton::SetProperties(
		int			lBnd,
		int			rBnd,
		int			uBnd,
		int			dBnd,
		std::string	NormalTexPath,
		std::string	HoverTexPath,
		std::string	PressedTexPath,
		GuiFunction	HoverFunction,
		GuiFunction	PressedFunction,
		void*		HoverParameters,
		void*		PressedParameters)
{
	NormalObj.SetProperties(lBnd, rBnd, uBnd, dBnd, NormalTexPath);
	HoverObj.SetProperties(lBnd, rBnd, uBnd, dBnd, HoverTexPath);
	PressedObj.SetProperties(lBnd, rBnd, uBnd, dBnd, PressedTexPath);
	LBound = lBnd, RBound = rBnd, UBound = uBnd, DBound = dBnd;
	LastDown = false;
	HoverFun = HoverFunction;
	PressedFun = PressedFunction;
	HoverParam = HoverParameters;
	PressedParam = PressedParameters;
	vlRequireUpdate = false;
	return true;
}

bool	GuiButton::SetProperties(
		int			lBnd,
		int			rBnd,
		int			uBnd,
		int			dBnd,
		std::string	NormalTexPath,
		std::string	HoverTexPath,
		std::string	PressedTexPath,
		GuiFunction	HoverFunction,
		GuiFunction	PressedFunction)
{
	return SetProperties(lBnd, rBnd, uBnd, dBnd,
			NormalTexPath, HoverTexPath, PressedTexPath,
			HoverFunction, PressedFunction, NULL, NULL);
}

GuiButton::GuiButton(
		void)
{
	LBound = RBound = UBound = DBound = 0;
	LastDown = false;
	NormalObj = GuiObject(0, 0, 0, 0, "");
	HoverObj = GuiObject(0, 0, 0, 0, "");
	PressedObj = GuiObject(0, 0, 0, 0, "");
	HoverFun = PressedFun = NULL;
	HoverParam = PressedParam = NULL;
//	Updater settings
	RenderUpdater(1, &vlRequireUpdate);
	vlRequireUpdate = true;
	return ;
}

GuiButton::GuiButton(
		int			lBnd,
		int			rBnd,
		int			uBnd,
		int			dBnd,
		std::string	NormalTexPath,
		std::string	HoverTexPath,
		std::string	PressedTexPath,
		GuiFunction	HoverFunction,
		GuiFunction	PressedFunction,
		void*		HoverParameters,
		void*		PressedParameters)
{
	SetProperties(lBnd, rBnd, uBnd, dBnd, NormalTexPath, HoverTexPath, PressedTexPath,
			HoverFunction, PressedFunction, HoverParameters, PressedParameters);
//	Updater settings
	RenderUpdater(1, &vlRequireUpdate);
	vlRequireUpdate = false;
	return ;
}

GuiButton::GuiButton(
		int			lBnd,
		int			rBnd,
		int			uBnd,
		int			dBnd,
		std::string	NormalTexPath,
		std::string	HoverTexPath,
		std::string	PressedTexPath,
		GuiFunction	HoverFunction,
		GuiFunction	PressedFunction)
{
	SetProperties(lBnd, rBnd, uBnd, dBnd, NormalTexPath, HoverTexPath, PressedTexPath,
			HoverFunction, PressedFunction);
//	Updater settings
	RenderUpdater(1, &vlRequireUpdate);
	vlRequireUpdate = false;
	return ;
}

GuiButton::~GuiButton(
		void)
{
	RenderUpdater(2, &vlRequireUpdate);
	return ;
}

bool	GuiFont::RequireUpdate(void)
	{ return vlRequireUpdate; }

bool	GuiFont::ForceUpdate(void)
	{ vlRequireUpdate = true; return true; }

bool	GuiFont::RenderObject(
		void)
{
	const char*	curStr = Content.c_str();
//	Draw every character in curStr
	glDisable(GL_TEXTURE_2D);
	glColor3d(ColourR, ColourG, ColourB);
	glRasterPos2i(PosX, PosY);
	for(; *curStr != '\0'; curStr++)
		glCallList(FontTextures + *curStr);
	return true;
}

bool	GuiFont::SetContent(
		std::string	content)
{
	Content = content;
	return true;
}

bool	GuiFont::SetProperties(
		GLuint		posX,
		GLuint		posY,
		GLdouble	colourR,
		GLdouble	colourG,
		GLdouble	colourB,
		int			FontSize,
		int			CharSet,
		const char*	TypeFace)
{
	const int	MAX_CHAR = 128;
	HDC			hFontHandle = NULL;
	triple_pair<int, int, std::string>	indexPair =
			make_triple_pair(FontSize, CharSet, (std::string)(TypeFace));
	PosX = posX, PosY = posY;
	ColourR = colourR, ColourG = colourG, ColourB = colourB;
	hFontHandle = wglGetCurrentDC();
//	Initialise font typeset settings
	static	std::map<triple_pair<int, int,
			std::string>, GLuint>	TypesetIndexer;
	if (TypesetIndexer[indexPair] == 0) {
		HFONT	hFont = CreateFontA(FontSize, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
			CharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TypeFace);
		HFONT	hOldFont = (HFONT)SelectObject(hFontHandle, hFont);
		DeleteObject(hOldFont);
		GLuint	newTex = glGenLists(MAX_CHAR);
		wglUseFontBitmaps(hFontHandle, 0, MAX_CHAR, newTex);
		TypesetIndexer[indexPair] = newTex;
	}
//	Updating object configuration
	FontTextures = TypesetIndexer[indexPair];
	vlRequireUpdate = false;
	return true;
}

bool	GuiFont::SetProperties(
		GLuint		posX,
		GLuint		posY,
		GLdouble	colourR,
		GLdouble	colourG,
		GLdouble	colourB,
		int			FontSize,
		int			CharSet,
		const char*	TypeFace,
		std::string	Content)
{
	SetProperties(posX, posY, colourR, colourG, colourB,
			FontSize, CharSet, TypeFace);
	SetContent(Content);
	vlRequireUpdate = false;
	return true;
}

GuiFont::GuiFont(
		void)
{
	SetProperties(0, 0, 0.0, 0.0, 0.0, 16, ANSI_CHARSET, "Arial");
	SetContent("");
	RenderUpdater(1, &vlRequireUpdate);
	vlRequireUpdate = true;
	return ;
}

GuiFont::GuiFont(
		GLuint		posX,
		GLuint		posY,
		GLdouble	colourR,
		GLdouble	colourG,
		GLdouble	colourB,
		int			FontSize,
		int			CharSet,
		const char*	TypeFace,
		std::string	content)
{
	SetProperties(posX, posY, colourR, colourG, colourB,
			FontSize, CharSet, TypeFace, content);
	RenderUpdater(1, &vlRequireUpdate);
	vlRequireUpdate = false;
	return ;
}

GuiFont::GuiFont(
		GLuint		posX,
		GLuint		posY,
		int			FontSize,
		int			CharSet,
		const char*	TypeFace,
		std::string	content)
{
	SetProperties(posX, posY, 0.0, 0.0, 0.0,
			FontSize, CharSet, TypeFace);
	RenderUpdater(1, &vlRequireUpdate);
	vlRequireUpdate = false;
	return ;
}

GuiFont::~GuiFont(
		void)
{
	RenderUpdater(2, &vlRequireUpdate);
	return ;
}

bool	PostResolutionRequireUpdate(
		void)
{
	RenderUpdater(3, NULL);
	return true;
}

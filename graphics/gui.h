
//	"gui.h" -*- C++ -*-
//	Created on: 16 April 2016
//	GUI objects' rendering.
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

#ifndef GRAPHICS_GUI_H_
#define GRAPHICS_GUI_H_

#include "include/public.h"

#include <GL/gl.h>
#include <GL/glext.h>
#include <windows.h>

typedef void (*GuiFunction) (void*);

std::string	GuiInputDialog(
		std::string	Notice);

bool	GuiDrawTooltip(
		std::string	Notice);

bool	GuiDrawTooltip(
		int			LBound,
		int			UBound,
		std::string	Notice);

bool	GuiDrawLoadingDialog(
		std::string	Notice);

class GuiObject
{
	friend	class GuiButton;
protected:
	GLuint	Tex;
	int 	LBound, RBound, UBound, DBound;
	bool	vlRequireUpdate;
public:
	bool	RequireUpdate(
			void);
	bool	ForceUpdate(
			void);
	bool	LoadTexture(
			std::string RelativePath);
	bool	RenderObject(
			void);
	bool	SetProperties(
			int 		lBnd,
			int 		rBnd,
			int			uBnd,
			int			dBnd,
			std::string TexPath);
	bool	SetProperties(
			int		lBnd,
			int		rBnd,
			int		uBnd,
			int		dBnd,
			GLuint	NewTex);
	bool	SetProperties(
			int	lBnd,
			int	rBnd,
			int	uBnd,
			int	dBnd);
	GuiObject(
			void);
	GuiObject(
			int 		lBnd,
			int 		rBnd,
			int			uBnd,
			int			dBnd,
			std::string TexPath);
	~GuiObject(
			void);
};

class GuiButton
{
protected:
	GuiObject	NormalObj, HoverObj, PressedObj;
	GuiFunction	HoverFun;
	GuiFunction	PressedFun;
	void*		HoverParam;
	void*		PressedParam;
	int 		LBound, RBound, UBound, DBound;
	bool		LastDown;
	bool		vlRequireUpdate;
public:
	bool	RequireUpdate(
			void);
	bool	ForceUpdate(
			void);
	bool	RenderObject(
			void);
	bool	SetProperties(
			int			lBnd,
			int			rBnd,
			int			uBnd,
			int			dBnd,
			GLuint		NormalTexPath,
			GLuint		HoverTexPath,
			GLuint		PressedTexPath,
			GuiFunction	HoverFunction,
			GuiFunction	PressedFunction,
			void*		HoverParam,
			void*		PressedParam);
	bool	SetProperties(
			int			lBnd,
			int			rBnd,
			int			uBnd,
			int			dBnd,
			std::string	NormalTexPath,
			std::string	HoverTexPath,
			std::string	PressedTexPath,
			GuiFunction	HoverFunction,
			GuiFunction	PressedFunction,
			void*		HoverParam,
			void*		PressedParam);
	bool	SetProperties(
			int			lBnd,
			int			rBnd,
			int			uBnd,
			int			dBnd,
			std::string	NormalTexPath,
			std::string	HoverTexPath,
			std::string	PressedTexPath,
			GuiFunction	HoverFunction,
			GuiFunction	PressedFunction);
	GuiButton(
			void);
	GuiButton(
			int			lBnd,
			int			rBnd,
			int			uBnd,
			int			dBnd,
			std::string	NormalTexPath,
			std::string	HoverTexPath,
			std::string	PressedTexPath,
			GuiFunction	HoverFunction,
			GuiFunction	PressedFunction,
			void*		HoverParam,
			void*		PressedParam);
	GuiButton(
			int			lBnd,
			int			rBnd,
			int			uBnd,
			int			dBnd,
			std::string	NormalTexPath,
			std::string	HoverTexPath,
			std::string	PressedTexPath,
			GuiFunction	HoverFunction,
			GuiFunction	PressedFunction);
	~GuiButton(
			void);
};

class GuiFont
{
protected:
	std::string	Content;
	GLuint		FontTextures;
	GLuint		PosX, PosY;
	GLdouble	ColourR, ColourG, ColourB;
	bool		vlRequireUpdate;
public:
	bool	RequireUpdate(
			void);
	bool	ForceUpdate(
			void);
	bool	RenderObject(
			void);
	bool	SetContent(
			std::string	content);
	bool	SetProperties(
			GLuint		posX,
			GLuint		posY,
			GLdouble	colourR,
			GLdouble	colourG,
			GLdouble	colourB,
			int			FontSize,
			int			CharSet,
			const char*	TypeFace);
	bool	SetProperties(
			GLuint		posX,
			GLuint		posY,
			GLdouble	colourR,
			GLdouble	colourG,
			GLdouble	colourB,
			int			FontSize,
			int			CharSet,
			const char*	TypeFace,
			std::string	Content);
	GuiFont(
			void);
	GuiFont(
			GLuint		posX,
			GLuint		posY,
			GLdouble	colourR,
			GLdouble	colourG,
			GLdouble	colourB,
			int			FontSize,
			int			CharSet,
			const char*	TypeFace,
			std::string	Content);
	GuiFont(
			GLuint		posX,
			GLuint		posY,
			int			FontSize,
			int			CharSet,
			const char*	TypeFace,
			std::string	Content);
	~GuiFont(
			void);
};

#define GuiDeclareObject(_a,_b,_c...)\
	static _a _b;\
	if (_b.RequireUpdate()) _b.SetProperties(_c);

bool	PostResolutionRequireUpdate(
		void);

#endif /* GRAPHICS_GUI_H_ */

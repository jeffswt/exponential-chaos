
//	"main.cpp" -*- C++ -*-
//	Created on: 4 March 2016
//	Main routine.
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

#include "include/public.h"

#include <iostream>
#include <cstdio>

#include "graphics/entry.h"
#include "main/util_entitygen.h"
#include "native/directory.h"
#include "native/inputctrl.h"
#include "network/chat.h"
#include "physics/config.h"
#include "physics/importer.h"
#include "physics/engine.h"

std::string		CurrentDirectory = "";
InputCtrlType	InputControl;
ConfigType		GameConfig;

/**
 *  @brief  trick to solve multi-threading import of textures.
 *  Better not use these things...
 */
HDC		hDeviceContext = NULL;
HGLRC	hRenderContextMain = NULL;
HGLRC	hRenderContextShared = NULL;

/**
 *  @brief  main function
 *  @param  argc: how many arguments
 *          argv: the argument strings
 *  Should only be called by routines of the operating system!
 */
int main(int argc, char** argv)
{
//	To disable the console window, use "-mwindows" in MinGW-Linker.
	eclogThreadStart();
	eclogSetVerbosity(9);
//	Some necessary initialisations, which are to fix the program.
//	Environment definitions and initialisations
	srand(GetProcessTime());
	CurrentDirectory = dGetCurrentDirectory(argv);
	GameConfig.ImportFromJson();
	chatInitialise();
	graphicsEntry(argc, argv);
//	Device and rendering contexts
	hDeviceContext = wglGetCurrentDC();
	hRenderContextMain = wglGetCurrentContext();
	hRenderContextShared = wglCreateContext(hDeviceContext);
	wglShareLists(hRenderContextShared, hRenderContextMain);
	wglMakeCurrent(hDeviceContext, hRenderContextMain);
	eclogPost(1, "Updated rendering contexts");
//	Entering main function, then the rest is given to separate functions
	beginImportGameEntityTypes();
	glutMainLoop();
//	Wait for physics thread to terminate, avoiding SIGSEGVs.
	endImportGameEntityTypes();
	TerminatePhysicsEngine();
//	Saving data to config files
	GameConfig.ExportToJson();
//	Final termination
	eclogThreadEnd();
	return 0;
}

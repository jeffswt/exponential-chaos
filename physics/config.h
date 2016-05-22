
//	"config.h" -*- C++ -*-
//	Created on: 17 April 2016
//	Game configuration management.
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

#ifndef PHYSICS_CONFIG_H_
#define PHYSICS_CONFIG_H_

#include "include/public.h"

#include <vector>

class ConfigType
{
public:
	int			CameraDistTolerance;
	double		CameraSpeed;
	int			ViewDistance;
	double		PixelRatio;
	int			WindowWidth, WindowHeight;
	bool		WindowFullScreen;
	bool		OpenToLan;
	std::string	PlayerName;
	std::vector<std::string>	ServerList;
//	JSON-related functions
	/**
	 *  @brief import data from JSON configuration
	 *  The path of the file is already specified inside the function. Therefore
	 *  users might not need to specify the source path.
	 */
	bool	ImportFromJson(
			void);
	/**
	 *  @brief export data to JSON file
	 *  The path of the file is already specified inside the function. Therefore
	 *  users might not need to specify the destination path.
	 */
	bool	ExportToJson(
			void);
//	Constructors and destruction operators
	ConfigType(
			void);
};

#endif /* PHYSICS_CONFIG_H_ */

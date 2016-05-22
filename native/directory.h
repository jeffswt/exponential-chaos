
//	"directory.h" -*- C++ -*-
//	Created on: 2016
//	Native file system iteration library.
//
//	Copyright (C) 2016  Geoffrey Tang.
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef NATIVE_DIRECTORY_
#define NATIVE_DIRECTORY_

#include "include/public.h"

#include <iostream>
#include <vector>

/**
 *  @brief  get directory and children content
 *  @param  Depth: the maximum iteration depth
 *          SearchDirectory: to output directories or not
 *          Path: the parent path
 *          FileList: a reference to the buffer output
 *  This function calls an iterative function to generate the output. Enclosed
 *  for it to function on different platforms.
 */
int	IterateDirectory(
		int							Depth,
		bool						SearchDirectory,
		std::string					Path,
		std::vector<std::string>&	FileList);

/**
 *  @brief  get program directory.
 *  @param  argv: The arguments given in main(argc, argv).
 *  Returns the current directory. Under special occasions would it act strange.
 *  Many ..s would appear as a result of separation between CWD and PWD.
 */
std::string	dGetCurrentDirectory(
		void);
std::string	dGetCurrentDirectory(
		char**	argv);

#endif /* NATIVE_DIRECTORY_ */

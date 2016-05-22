
//	"directory.cpp" -*- C++ -*-
//	Created on: 2016
//	Definitions for native file system iteration library.
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

#include "native/directory.h"

#include <direct.h>
#include <windows.h>

/*
 * iterate_directory_win: - Find all files under specific directory (Windows)
 * Works only under Windows. Use with precaution.
 *
 * Initially this function is copied from the internet. The original webpage
 * was forgotten, but it was clearant that it was from CSDN. After particular
 * discoveries it appeared to be a recursive one.
 * Several modifications were done to this function. Output the data to a string
 * is considerable. Recursion can be disabled due to performance and
 * compatibility issues. (Disabled by default).
 * This function is only imposed by dict reader in commence.
 * Upon iterating this function for the first time, (int)level should be 0.
 *
 * The return value is 0 if succeeded, -EGENERAL if something unpredicted (and
 * unknown) happened.
 */
int	iterate_directory_win(
		char*						lpPath,
		std::vector<std::string>&	file_list,
		int							level,
		int							max_depth,
		bool						searchdirectory)
{
	/*
	 * As I have mentioned before, file depth over 1 directory is not
	 * welcomed. Because of the peculiar structure StarDict dictionaries
	 * have, putting them under different directories is not practical.
	 */
	if (level > max_depth - 1)
		return 0;
	/*
	 * Looks like Linus Torvalds will be irritated if he sees these lines of
	 * code. I did not follow the Linux coding style in primary, and decided
	 * that if this is a Windows-specific code snippet, then it should be in
	 * Windows style. @VariableCapitalization
	 */
	char szFile[MAX_PATH];
	char szFind[MAX_PATH];
	char root[MAX_PATH];
	/* Just do it, neither do I know what do these lines mean. */
	WIN32_FIND_DATA FindFileData;
	strcpy(szFind,lpPath);
	strcat(szFind,"*.*");
	HANDLE hFind = NULL;
	hFind = FindFirstFile(szFind,&FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return -1;
	while (true) {
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(FindFileData.cFileName[0]!='.') {
				/* szFile is the name of the directory. */
				strcpy(szFile,lpPath);
				strcat(szFile,FindFileData.cFileName);
				strcat(szFile,"/");
				if (searchdirectory) {
					std::string import_string;
					for (unsigned int i = 0; i < strlen(szFile); i++)
						import_string += szFile[i];
					file_list.push_back(import_string);
				}
				/*
				 * We don't necessarily need this file to really
				 * enter the vector of files, so we are omitting
				 * it this time.
				 */
				iterate_directory_win(szFile, file_list,
						level + 1, max_depth, searchdirectory);
			}
		} else {
			/* root is the general path of the file. */
			strcpy(root, lpPath);
			strcat(root, FindFileData.cFileName);
			/*
			 * The filenames which we actually need is the actual
			 * filenames. These should go into the vector of files.
			 */
			std::string import_string;
			for (unsigned int i = 0; i < strlen(root); i++)
				import_string += root[i];
			file_list.push_back(import_string);
		}
		if(!FindNextFile(hFind,&FindFileData))
			break;
	}
	FindClose(hFind);
	return 0;
}

/*
 * IterateDirectory: - Find files directly under given directory
 *
 * This function is the public entry function of the previously declared and
 * defined function iterate_directory_****(). It provides a better and easier
 * way to enter the procedure.
 *
 * The return value is same as the iterate_directory_unit() function.
 */
int	IterateDirectory(
		int							Depth,
		bool						SearchDirectory,
		std::string					Path,
		std::vector<std::string>&	FileList)
{
	int ret_val = 0;
	int str_len;
	char *path_chr;
	str_len = Path.length() + 8;
	path_chr = new char[str_len];
	memset(path_chr, 0, str_len * sizeof(char));
	for (unsigned int i = 0; i < Path.length(); i++)
		path_chr[i] = Path[i];
	ret_val = iterate_directory_win(path_chr, FileList, 0, Depth, SearchDirectory);
	return ret_val;
}

std::string	dGetCurrentDirectory(
		void)
{
	char*		Str = new char[256];
	std::string	Output = "";
	getcwd(Str, 256);
	Output = Str;
	for (unsigned int i = 0; i < Output.length(); i++)
		if (Output[i] == '\\')
			Output[i] = '/';
	if (Output[Output.length() - 1] != '/')
		Output += '/';
	return Output;
}

std::string	dGetCurrentDirectory(
		char**	argv)
{
	std::string	Exec = argv[0];
	std::string	Path = dGetCurrentDirectory();
	int			execEnd = Exec.length() - 1;
	for (unsigned int i = 0; i < Exec.length(); i++)
		if (Exec[i] == '\\')
			Exec[i] = '/';
	for (; execEnd > 0 && Exec[execEnd] != '/'; execEnd--);
	if (Exec.length() >= 2) {
		if (Exec[1] == ':') {
			Path = "";
			for (int i = 0; i < execEnd; i++)
				Path += Exec[i];
		} else {
			for (int i = 0; i < execEnd; i++)
				Path += Exec[i];
		}
	}
	if (Path[Path.length() - 1] != '/')
		Path += '/';
	return Path;
}

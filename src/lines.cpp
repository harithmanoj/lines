//	This file is part of Lines.
//
//	Copyright 2020 Harith Manoj <harithpub@gmail.com>
//
//	Lines is free software : you can redistribute itand /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.

//	Lines is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with Lines.If not, see < https://www.gnu.org/licenses/>.



//
//	This File is for the defining functions declared in lines.h.
//

#include "lines.h"

lines::directory lines::getFileStructure(path element, bool recursive, std::vector<std::string> extensions)
{
	directory ret{ {element,0,0},{},{} };
	if (fs::is_regular_file(element))
		return ret; // only file
	if (fs::is_directory(element)) // if directory
	{
		for (auto& el : fs::directory_iterator(element)) // iterate over elements
		{
			auto element_path = el.path();

			if (recursive)
				if (fs::is_directory(element_path))
					ret.dirs.push_back(getFileStructure(element_path, true, extensions));
			//if recursive add to directory::dirs
			if (fs::is_regular_file(element_path)) // if file add to directory::files
			{
				bool reg = extensions.size() == 0;
				reg = reg ||
					(std::find(extensions.begin(), extensions.end(),
						element_path.extension()) != extensions.end());
				if (reg) // only add if extensions is empty or file extension is present in extensions vector
				{
					ret.files.push_back({ element_path,0,0 });
				}
			}

		}
	}
	return ret;
}
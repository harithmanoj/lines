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
//	This File is for the main function.
//


#include "lines.h"
#include <iostream>





int main(int argc, char* argv[])
{
	lines::banner();
	if (argc == 1)
	{
		return 0;
	}

	lines::mode working;
	std::vector<std::string> extensions;
	lines::path file;
	try {
		file = lines::parse(argc, argv, working, extensions);
	}
	catch (std::invalid_argument ex)
	{
		std::cout << "Error : " << ex.what();
	}
	lines::execute(working, extensions, file);

	LOG("after no file error");
	return 0;
}
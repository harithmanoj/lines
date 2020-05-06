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
	lines::banner(); // banner
	if (argc == 1)
	{
		lines::help(); //display help if no argument
		return 0;
	}

	REPLACE(for (int i = 0; i < argc; ++i))
		LOG(argv[i] + std::string(" ")); //log command line argument

	lines::mode working;
	std::vector<std::string> extensions;
	lines::path file;
	try {
		file = lines::parse(argc, argv, working, extensions); //parse command
	}
	catch (std::invalid_argument ex)
	{
		std::cout << "Error : " << ex.what() << "\n\n"; // if invalid input display help
		lines::help();
		return 0;
	}
	lines::execute(working, extensions, file); //execute using mode specified

	LOG("after no file error");
	return 0;
}
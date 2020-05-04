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
		std::cout << " no file ?\n";
		return 0;
	}

	LOG("after no file error");

	//display only
	bool local = false;
	int argexp = 2;

	if (std::string(argv[argexp - 1]) == "?")
		lines::help();

	if (std::string(argv[argexp - 1]) == "-l")
	{
		local = true;
		++argexp;
	}

	if (argc < argexp)
	{
		std::cout << "no file?";
		return 0;
	}

	//path of target
	lines::path dir = argv[argc - 1];

	if (!fs::exists(dir))
	{
		std::cout << dir << " is niether a directory or a file\n";
		return 0;
	}

	LOG(dir.string() << "  dir");

	//has extension constraints
	std::vector<std::string> ex;

	//is recursive
	bool rec = false;

	if (std::string(argv[argexp - 1]) == "-r")
	{
		rec = true;
		++argexp;
	}

	if (std::string(argv[argexp - 1]) == "-e")
	{
		for (int i = argexp; i < argc - 1; ++i)
			ex.push_back(argv[i]);
		LOG("registered extensions");
	}
	

	// populated directory structure
	auto ret = lines::count_all(dir, rec, ex);
	LOG("count complete");

	if (!local)
	{
		auto p = dir;
		if (fs::is_directory(dir))
			p /= p.stem().string() + ".lin";
		else if (fs::is_regular_file(dir))
			p.replace_extension(".lin");
		std::ofstream out(p);

		lines::write(out, ret);
	}
	
	lines::write(std::cout, ret);

	return 0;
}
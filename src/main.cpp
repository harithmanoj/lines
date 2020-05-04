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

	lines::path dir = argv[argc - 1];

	std::vector<std::string> ex;

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
	}

	auto ret = lines::all_files(dir, ex, rec);

	if (!local)
	{
		auto p = dir;
		if (lines::fs::is_directory(dir))
			p /= "line_total.lin";
		else
			p = p.parent_path();
		std::ofstream out(dir / "line_total.lin");

		out << "total lines in directory : " << ret << "\n";
	}
	std::cout << dir << " has total lines " << ret << "\n";
	return 0;
}
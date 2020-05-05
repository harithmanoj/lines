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
//
//
//	You should have received a copy of the GNU General Public License
//	along with Lines. If not, see < https://www.gnu.org/licenses/>.



//
//	This File is for all functions and classes the program may use.
//

#ifndef LINES_H

#define LINES_H
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#ifdef _DEBUG

#define LINES_DEBUG

#endif

#ifdef LINES_DEBUG

#define REPLACE(x)			x

#else

#define REPLACE(x)

#endif

#define LOG(x)				REPLACE(((std::cout << __LINE__ << " " << __func__ << " " )<< x) << "\n")

namespace fs = std::filesystem;

// so as not to pollute the global space
namespace lines
{

	const constexpr char* version_lines = "v1.0.1";
	using fs::path;

	//struct for pairing line count with comments and without comments
	struct LineCount
	{
		path component;

		// total line count
		unsigned long long total = 0;

		//comment and whitespace stripped count
		unsigned long long stripped = 0;

		inline LineCount& operator += (LineCount other)
		{
			total += other.total;
			stripped += other.stripped;
			return *this;
		}
	};



	//encapsulates directory information including sub-directories
	struct directory
	{
		LineCount current; // line count info
		std::vector<LineCount> files; // files
		std::vector<directory*> dirs; // sub-directories

		void add_dirs(directory&& in)
		{
			dirs.push_back(new directory(in));
		}

		~directory()
		{
			for (auto& i : dirs)
				if (i != nullptr)
					delete i;
		}
	};

	

	directory count_all(path argument, bool isRecursive, std::vector<std::string> extensions);

	// Write to file generalized function
	void write(std::ostream& out, const directory& dir);

	/*
	// Find line count of specified file
	unsigned long find_length(path file)
	{
		unsigned long ret = 0;

		// if not file throw error
		if (!fs::is_regular_file(file))
			throw std::invalid_argument("not regular file");

		std::ifstream in(file);

		std::string st;
		std::getline(in, st);
		while (!in.eof()) //till end of file
		{
			if (st.find_first_not_of(" \t\n\v\f\r") != std::string::npos)
				++ret; // if line is not only whitespace
			std::getline(in, st);
		}
		return ret;
	}

	// get total lines for all files in the directory.
	unsigned long long all_files(path directory, std::vector<std::string> permitted, bool rec = false)
	{
		unsigned long long ret = 0;

		if (!fs::is_directory(directory))
		{
			if (!fs::is_regular_file(directory))
				throw std::invalid_argument("not directory nor file");
			else
				return find_length(directory);
		}

		if (!rec)
		{
			if (permitted.size() == 0)
				for (auto& p : fs::directory_iterator(directory))
				{
					auto& file = p.path();
					if (fs::is_regular_file(file))
						ret += find_length(file);
				}
			else
				for (auto& p : fs::directory_iterator(directory))
				{
					auto& file = p.path();
					if (fs::is_regular_file(file))
					{
						if ((std::find(permitted.begin(), permitted.end(), file.extension())
							!= permitted.end()))
						{
							ret += find_length(file);
						}
					}
				}
		}
		else
		{
			if (permitted.size() == 0)
				for (auto& p : fs::recursive_directory_iterator(directory))
				{
					auto& file = p.path();
					if (fs::is_regular_file(file))
						ret += find_length(file);
				}
			else
				for (auto& p : fs::recursive_directory_iterator(directory))
				{
					auto& file = p.path();
					if (fs::is_regular_file(file))
					{
						if ((std::find(permitted.begin(), permitted.end(), file.extension())
							!= permitted.end()))
						{
							ret += find_length(file);
						}
					}
				}
		}
		return ret;
	}
	*/
	inline void banner()
	{
		std::cout << "\n-----------------------------------------------------------------------------\n";
		std::cout << "\n\nProgram to count lines\n\n";
		std::cout << " \t\t Open source project hosted at github\n\n";
		std::cout << " \t\t\tVersion " << version_lines << "\n";
		std::cout << "\n-----------------------------------------------------------------------------\n";
	}

	inline void help()
	{
		std::cout << "lines [directory / filename] : counts line in file / dir outputs to display and line_total.lin\n"
			<< " lines -l [directory / filename] : counts line in file / dir and displays it\n"
			<< " lines -e [ext] [dir / file] : counts line in file that has extensions in [ext]\n"
			<< " lines -r [dir / file] : counts lines of all files in directory and sub directory\n"
			<< " switches can be mixed in the order : -l, -r, -e\n\n";
	}

}

#endif
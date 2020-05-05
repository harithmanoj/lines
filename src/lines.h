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

	const constexpr char* version_lines = "v1.1.2";
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

	// To identify which all switches were passed
	enum mode : unsigned char
	{
		HELP = 0x00, //help
		LOCAL = 0x01, //local
		EXTENSIONS = 0x02, // extensions
		RECURSIVE = 0x04, // recursive
		DIR = 0x8, // directory only output
		MASTER_DIR = DIR + 0x10 // master directory only output
	};

	inline mode& operator |= (mode& lhs, const mode& rhs) noexcept
	{
		lhs = static_cast<mode>(static_cast<unsigned char>(lhs)
			| static_cast<unsigned char>(rhs));
	}

	//function to check if specific bitfields are high
	template<class enumT>
	inline bool checkField(enumT in, enumT field) noexcept
	{
		return ((in | field) == in);
	}

	//encapsulates directory information including sub-directories
	struct directory
	{
		LineCount current; // line count info
		std::vector<LineCount> files; // files
		std::vector<directory*> dirs; // sub-directories

		inline directory() : current{ path(),0,0 } { ; }

		directory(LineCount c, std::vector<LineCount> fl, std::vector<directory*>&& dr) : current(c), files(fl), dirs(dr)
		{
			for (auto& i : dr)
				i = nullptr;
		}

		directory(const directory& in) : current(in.current), files(in.files)
		{
			for (auto& i : in.dirs)
				dirs.push_back(new directory(*i));
		}

		directory(directory&& in) noexcept : current(in.current), files(in.files)
		{
			for (auto& i : in.dirs)
			{
				dirs.push_back(i);
				i = nullptr;
			}
		}

		directory& operator = (const directory& in)
		{
			current = in.current;
			files = in.files;
			for (auto& i : in.dirs)
				dirs.push_back(new directory(*i));
			return *this;
		}

		directory& operator = (directory&& in) noexcept
		{
			current = in.current;
			files = in.files;
			for (auto& i : in.dirs)
			{
				dirs.push_back(i);
				i = nullptr;
			}
			return *this;
		}

		void add_dirs(directory&& in)
		{
			current += in.current;
			dirs.push_back(new directory(std::move(in)));
		}

		void add_file(path file);

		~directory()
		{
			for (auto& i : dirs)
				if (i != nullptr)
					delete i;
		}
	};



	// get file structure of element, if directory gets subdirectory if recursive is true
	// if file directory::files and directory::dirs is empty
	// if no recursive directory::dirs is empty
	// also count lines in each file and add it to the structure
	directory count_lines(path element, bool recursive, std::vector<std::string> extensions);

	// Write to file generalized function
	void write(std::ostream& out, const directory& dir, bool master = false, bool dir_only = false);


	// parse command recieved and converts to mode, extensions and path
	path parse(int argc, char* argv[], mode& out, std::vector<std::string>& extensions);


	//executes according to mode indicatd by type
	void execute(mode type, std::vector<std::string>& extensions, path element);	
	

}

#endif
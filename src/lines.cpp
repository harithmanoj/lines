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

namespace lines
{
	//checks if string can be added to stripped count, checks if comment starts or stops there
	bool check_string(const std::string& in, LineCount& info, bool prev_state)
	{
		auto beg = in.find_first_not_of(" \t\n\v\f\r"); //begining discarding whitespace
		auto end = in.find_last_not_of(" \t\n\v\f\r"); //ending discarding whitespace
		++info.total; //total line is incremented either way

		if (beg == std::string::npos)
			return prev_state;

		if (prev_state) //if multi line comment is active
		{
			auto previous_comment_end = in.find_first_of('*'); // find end of comment
			auto prev_cmnt_end = in.find_first_of('/', previous_comment_end);
			if (previous_comment_end != std::string::npos)
				if (prev_cmnt_end != std::string::npos)
					if (previous_comment_end == prev_cmnt_end - 1)
					{
						if (prev_cmnt_end == end)
							return false;
						else
						{
							++info.stripped;
							return false;
						}
					}
			return true;
		}

		auto comment_single_beg = in.find_first_of('/');
		auto comment_single_end = in.find_first_of('/', comment_single_beg + 1);
		if (comment_single_beg != std::string::npos) // if single line commet found in begining, no code
			if (comment_single_end != std::string::npos)
				if (comment_single_end == comment_single_beg + 1)
					if (comment_single_beg == beg)
						return false;

		auto multi_line_beg_b = in.find_first_of('/'); // get multi-line start position
		auto multi_line_beg_e = in.find_first_of('*', multi_line_beg_b);

		if (multi_line_beg_b != std::string::npos) // if it exists
			if (multi_line_beg_e != std::string::npos)
				if (multi_line_beg_e == multi_line_beg_b + 1)
				{
					if (multi_line_beg_b != beg)
						++info.stripped; //increments actual code line count if not begining

					return true; // regardless of code multi line comment is active
				}

		++info.stripped; // else actual code exists
		return false;
	}

	//counts total lines in a file
	void count_file_lines(LineCount& count)
	{
		if (count.component.empty())
			throw std::invalid_argument("empty path");
		if (!fs::is_regular_file(count.component))
			throw std::invalid_argument("argument must be directory");
		LOG(count.component.string());
		std::ifstream file(count.component);

		std::string input;
		bool comment = false;
		std::getline(file, input);
		while (!file.eof())
		{
			comment = check_string(input, count, comment);
			std::getline(file, input);
		}
		LOG(count.total << " " << count.stripped);
	}

	
	lines::directory lines::count_lines(path element, bool recursive, std::vector<std::string> extensions)
	{
		//directory to return
		directory ret{ {element,0,0},{},{} };
		if (fs::is_regular_file(element))
		{
			count_file_lines(ret.current);
			return ret; // only file
		}
		if (fs::is_directory(element)) // if directory
		{
			LOG("directory");
			for (auto& el : fs::directory_iterator(element)) // iterate over elements
			{
				auto element_path = el.path();
				LOG(element_path.string());
				if (recursive)
					if (fs::is_directory(element_path))
					{
						auto t = count_lines(element_path, true, extensions);
						if (t.files.size() != 0 || t.dirs.size() != 0)
							ret.add_dirs(std::move(t));
					}

				//if recursive add to directory::dirs
				if (fs::is_regular_file(element_path)) // if file add to directory::files
				{
					bool reg = extensions.size() == 0;
					reg = reg ||
						(std::find(extensions.begin(), extensions.end(),
							element_path.extension()) != extensions.end());
					if (reg) // only add if extensions is empty or file extension is present in extensions vector
					{
						ret.add_file(element_path);
					}
				}
			}
		}
		return std::move(ret);
	}

	// Write filename, lines counted etc for the given file 
	//(depth means depth in the filesystem from passed path)
	void write_to_stream(std::ostream& out, const LineCount& file, unsigned depth = 0)
	{
		std::string intendation(depth, '\t');

		out << intendation << file.component.filename().string() << "\n";
		out << intendation << "\tTotal lines : " << file.total << "\n";
		out << intendation << "\tCode lines : " << file.stripped << "\n";
	}

	// Write directory name, lines counted etc for the given file and all elements
	//(depth means depth in the filesystem from passed path)
	void write_to_stream(std::ostream& out, const directory& dir, unsigned depth = 0, bool dir_only = false)
	{
		std::string intendation(depth, '\t');

		out << intendation << "directory : " << dir.current.component.stem().string() << "\n";
		out << intendation << "\tTotal lines : " << dir.current.total << "\n";
		out << intendation << "\tCode lines : " << dir.current.stripped << "\n\n";

		if (!dir_only)
			for (auto& i : dir.files)
				write_to_stream(out, i, depth + 1);

		for (auto& i : dir.dirs)
			write_to_stream(out, *i, depth + 1, dir_only);
	}

	void write(std::ostream& out, const directory& dir, bool master, bool dir_only)
	{
		if (dir.dirs.size() == 0 && dir.files.size() == 0)
			write_to_stream(out, dir.current);
		else if (master)
		{
			directory ps{ dir.current,{},{} };
			write_to_stream(out, ps, 0, true);
		}
		else
			write_to_stream(out, dir, 0, dir_only);
	}

	void directory::add_file(path file)
	{
		LineCount in{ file,0,0 };
		count_file_lines(in);
		current += in;
		files.push_back(in);
	}

}
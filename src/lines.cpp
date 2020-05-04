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

lines::directory lines::get_file_structure(path element, bool recursive, std::vector<std::string> extensions)
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
					ret.dirs.push_back(get_file_structure(element_path, true, extensions));
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

bool lines::check_string(const std::string& in, LineCount& info, bool prev_state)
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

void lines::count_file_lines(LineCount& count)
{
	if (count.component.empty())
		throw std::invalid_argument("empty path");
	if (!fs::is_directory(count.component))
		throw std::invalid_argument("argument must be directory");

	std::ifstream file(count.component);

	std::string input;
	bool comment = false;
	std::getline(file, input);
	while (!file.eof())
	{
		comment = check_string(input, count, comment);
		std::getline(file, input);
	}
}


void lines::count_directory_r(directory& dir)
{
	for (auto& i : dir.files)
	{
		count_file_lines(i);
		dir.current += i;
	}

	for (auto& i : dir.dirs)
	{
		count_directory_r(i);
		dir.current += i.current;
	}
}


void lines::write_to_stream(std::ostream& out, const LineCount& file, unsigned depth = 0)
{
	std::string intendation(depth, '\t');

	out << intendation << file.component.filename().string() << "\n";
	out << intendation << "Total lines : " << file.total << "\n";
	out << intendation << "Code lines : " << file.stripped << "\n";
}

void lines::write_to_stream(std::ostream& out, const directory& dir, unsigned depth = 0)
{
	std::string intendation(depth, '\t');

	out << intendation << dir.current.component.stem().string() << "\n";
	out << intendation << "Total lines : " << dir.current.total << "\n";
	out << intendation << "Code lines : " << dir.current.stripped << "\n";

	for (auto& i : dir.files)
		write_to_stream(out, i, depth + 1);

	for (auto& i : dir.dirs)
		write_to_stream(out, i, depth + 1);
}
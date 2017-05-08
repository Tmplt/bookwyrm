/*
 * This file contains a bunch of utility functions.
 *
 * Copyright (C) 2017 Tmplt <tmplt@dragons.rocks>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cerrno>

#include "utils.hpp"

namespace utils {

std::error_code validate_download_dir(const fs::path &path)
{
    if (!fs::exists(path))
        return {ENOENT, std::generic_category()};

    if (fs::space(path).available == 0)
        return {ENOSPC, std::generic_category()};

    if (!fs::is_directory(path))
        return {ENOTDIR, std::generic_category()};

    /* Can we write to the directory? */
    if (access(path.c_str(), W_OK) != 0)
        return {EACCES, std::generic_category()};

    return {};
}

string vector_to_string(const vector<string> &vec)
{
    string retstring = "";
    for (const auto &e : vec)
        retstring += e + (e != vec.back() ? ", " : "");

    return retstring;
}

/* ns utils */
}


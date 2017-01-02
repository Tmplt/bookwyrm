#pragma once

#include <iostream>

#include "version.hpp"

#define PROG_NAME "bookwyrm"
/* #undef PROG_VERSION */
#ifndef PROG_VERSION
    #define PROG_VERSION GIT_TAG
#endif

auto print_build_info = []() {
    std::cout << PROG_NAME << " " << PROG_VERSION << '\n'
              << "Copyright (C) 2016-2017 Tmplt <tmplt@dragons.rocks>.\n"
              << PROG_NAME << " is licensed under the MIT license, "
              << "see <https://github.com/Tmplt/bookwyrm/LICENSE.md>.\n\n"
              << "Written by Tmplt and others, see <https://github.com/Tmplt/bookwyrm/AUTHORS.md>."
              << std::endl;
};

/* vim: ft=cpp */

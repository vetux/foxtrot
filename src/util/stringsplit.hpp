/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FOXTROT_STRINGSPLIT_HPP
#define FOXTROT_STRINGSPLIT_HPP

#include <string>
#include <vector>

namespace StringSplit {
    inline std::vector<std::string> split(const std::string &src, char delim) {
        if (src.empty())
            return {};

        std::vector<std::string> ret{""};
        for (auto &c: src) {
            auto &str = *(ret.end() - 1);
            if (c == delim) {
                ret.emplace_back("");
            } else {
                str += c;
            }
        }
        return ret;
    }
}

#endif //FOXTROT_STRINGSPLIT_HPP

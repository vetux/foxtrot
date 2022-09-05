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

#ifndef FOXTROT_CONSOLECOMMAND_HPP
#define FOXTROT_CONSOLECOMMAND_HPP

#include <string>
#include <vector>

#include "util/stringsplit.hpp"

struct ConsoleCommand {
    std::string commandLine;
    std::string cmd;
    std::vector<std::string> arguments;

    /**
     * Default command parsing with whitespace separated command and argument keys / values
     *
     * @param command
     */
    ConsoleCommand(const std::string &command)
            : commandLine(command) {
        auto sep = StringSplit::split(command, ' ');
        for (auto &str: sep) {
            if (cmd.empty()) {
                cmd = str;
            } else {
                arguments.emplace_back(str);
            }
        }
    }
};

#endif //FOXTROT_CONSOLECOMMAND_HPP

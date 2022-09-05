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

#ifndef FOXTROT_CONSOLE_HPP
#define FOXTROT_CONSOLE_HPP

#include "consoleparser.hpp"
#include "consoleoutput.hpp"

#include <set>

class Console : public ConsoleOutput {
public:
    void addParser(ConsoleParser &parser) {
        parsers.insert(&parser);
    }

    void removeParser(ConsoleParser &parser) {
        parsers.erase(&parser);
    }

    void addOutput(ConsoleOutput &printer) {
        printers.insert(&printer);
    }

    void removePrinter(ConsoleOutput &printer) {
        printers.erase(&printer);
    }

    void invokeCommand(const std::string &command) {
        invokeCommand(ConsoleCommand(command));
    }

    void invokeCommand(const ConsoleCommand &command) {
        for (auto &parser: parsers) {
            if (parser->parseCommand(command, *this))
                return;
        }
        print("Command not found " + command.commandLine);
    }

    void print(const std::string &str) override {
        for (auto &printer: printers) {
            printer->print(str);
        }
    }

private:
    std::set<ConsoleParser *> parsers;
    std::set<ConsoleOutput *> printers;
};

#endif //FOXTROT_CONSOLE_HPP

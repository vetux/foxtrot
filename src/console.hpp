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
#include "consoleprinter.hpp"

#include <set>

class Console : public ConsolePrinter {
public:
    void addParser(ConsoleParser &parser) {
        parsers.insert(&parser);
    }

    void removeParser(ConsoleParser &parser) {
        parsers.erase(&parser);
    }

    void addPrinter(ConsolePrinter &printer) {
        printers.insert(&printer);
    }

    void removePrinter(ConsolePrinter &printer) {
        printers.erase(&printer);
    }

    void invokeCommand(const std::string &command) {
        invokeCommand(ConsoleCommand(command));
    }

    void invokeCommand(const ConsoleCommand &command) {
        for (auto &parser: parsers) {
            if (parser->parseCommand(command, *this))
                break;
        }
    }

    void printInfo(const std::string &str) override {
        for (auto &printer: printers) {
            printer->printInfo(str);
        }
    }

    void printError(const std::string &str) override {
        for (auto &printer: printers) {
            printer->printError(str);
        }
    }

private:
    std::set<ConsoleParser *> parsers;
    std::set<ConsolePrinter *> printers;
};

#endif //FOXTROT_CONSOLE_HPP
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

#ifndef FOXTROT_INVENTORY_HPP
#define FOXTROT_INVENTORY_HPP

#include <utility>

#include "xengine.hpp"

#include "item.hpp"

using namespace xng;

class Inventory : public Messageable {
public:
    struct Cell {
        Item::ID item = ITEM_NULL;
        int count = 0;

        Cell() = default;

        explicit Cell(Item::ID item, int count = 1) : item(item), count(count) {}
    };

    void setCell(int index, Cell cell) {
        cells[index] = cell;
    }

    const Cell &getCell(int index) {
        return cells[index];
    }

    void clearCell(int index) {
        cells.erase(index);
    }

    const std::map<int, Cell> &getCells() const {
        return cells;
    }

    bool checkCell(int index) {
        return cells.find(index) != cells.end();
    }

    void add(Item::ID item, int count) {
        for (int i = 0; i < cells.size(); i++) {
            if (!checkCell(i)) {
                setCell(i, Cell(item, count));
            }
        }
    }

    Messageable &operator<<(const Message &message) override {
        cells.clear();
        for (auto &pair: message.asMap()) {
            cells.insert(std::pair(std::stoi(pair.first), Cell(pair.second["id"], pair.second["count"])));
        }
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);
        for (auto &pair: cells) {
            auto cell = Message(xng::Message::DICTIONARY);
            cell["id"] = pair.second.item;
            cell["count"] = pair.second.count;
            message[std::to_string(pair.first).c_str()] = cell;
        }
        return message;
    }

private:
    std::map<int, Cell> cells;
};

#endif //FOXTROT_INVENTORY_HPP

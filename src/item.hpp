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

#ifndef FOXTROT_ITEM_HPP
#define FOXTROT_ITEM_HPP

#include "xengine.hpp"

using namespace xng;

struct Item {
    typedef int ID;

    std::string name;
    std::string description;
    ResourceHandle<Sprite> icon;
    bool stackable;

    Item(std::string name, std::string description, ResourceHandle<Sprite> icon, bool stackable)
            : name(std::move(name)), description(std::move(description)), icon(std::move(icon)), stackable(stackable) {}

    static const std::map<ID, Item> items;
};

enum ItemIds : Item::ID {
    ITEM_NULL = 0,

    // Weapons
    ITEM_REVOLVER = 1,
    ITEM_GATLING = 2,

    // Ammunition
    ITEM_AMMO_5_56 = 50,
    ITEM_AMMO_45 = 51,
    ITEM_AMMO_12_GAUGE_BUCKSHOT = 52,
    ITEM_AMMO_12_GAUGE_SLUG = 53,

    // Consumables
    ITEM_MEDKIT = 100,
};

#endif //FOXTROT_ITEM_HPP

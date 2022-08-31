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

#include "item.hpp"

const std::map<Item::ID, Item> Item::items = {
        {ITEM_NULL, Item("NULL", "NULL ITEM", ResourceHandle<Sprite>(), true)},

        {ITEM_REVOLVER, Item("Revolver", "A revolver", ResourceHandle<Sprite>(), false)},
        {ITEM_GATLING, Item("Gatling", "A gatling gun", ResourceHandle<Sprite>(), false)},

        {ITEM_AMMO, Item("Ammo", "Ammunition", ResourceHandle<Sprite>(), true)},
        {ITEM_MEDKIT, Item("Medkit", "A Medkit for healing", ResourceHandle<Sprite>(), true)}
};
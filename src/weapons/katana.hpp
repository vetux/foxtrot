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

#ifndef FOXTROT_KATANA_HPP
#define FOXTROT_KATANA_HPP

#include "weapon.hpp"

class Katana : public Weapon {
public:
    Katana() {

    }

    void update(DeltaTime deltaTime) override {
        Weapon::update(deltaTime);
    }

    Type getType() override {
        return KATANA;
    }

    Visuals getVisuals() override {
        return visuals;
    }

    bool shoot() override {
        return true;
    }

    float weight() override {
        return 0;
    }

    Vec2f getAngleBounds() override {
        return Weapon::getAngleBounds();
    }

private:
    Visuals visuals;
};

#endif //FOXTROT_KATANA_HPP

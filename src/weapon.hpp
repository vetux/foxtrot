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

#ifndef FOXTROT_WEAPON_HPP
#define FOXTROT_WEAPON_HPP

#include "xengine.hpp"

class Weapon {
public:
    enum Type {
        NONE,
        PISTOL,
        GATLING
    };

    struct Visuals {
        ResourceHandle<Sprite> sprite;
        Vec2f size;
        Vec2f center; // The rotation center of the sprite
        Vec2f offset; // The transform offset relative to the player facing to the right

        ResourceHandle<SpriteAnimation> muzzleFlash;
        Vec2f muzzleSize;
        Vec2f muzzleCenter;
        Vec2f muzzleOffset; // The muzzle offset relative to offset
    };

    virtual ~Weapon() = default;

    virtual void update(DeltaTime deltaTime) {}

    virtual Type getType() { return NONE; }

    virtual Visuals getVisuals() { return {}; }

    virtual void setAmmo(int ammo) {}

    virtual int getAmmo() { return 0; }

    virtual bool shoot() { return false; }

    virtual float weight() { return 0; }

    virtual Vec2f getAngleBounds() { return {}; }
};

#endif //FOXTROT_WEAPON_HPP

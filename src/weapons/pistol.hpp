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

#ifndef FOXTROT_PISTOL_HPP
#define FOXTROT_PISTOL_HPP

#include "weapon.hpp"

class Pistol : public Weapon {
public:
    Pistol() = default;

    explicit Pistol(ResourceHandle<Sprite> sprite)
            : sprite(std::move(sprite)) {
        reloadDuration = 2;
        clipSize = 9;
    }

    ~Pistol() override = default;

    Type getType() const override {
        return PISTOL;
    }

    Visuals getVisuals() const override {
        Visuals ret;
        ret.size = {40, 20};
        ret.center = {10, 16};
        ret.offset = {0, 0};
        ret.sprite = sprite;

        ret.muzzleFlash = ResourceHandle<SpriteAnimation>(Uri("animations/muzzle_a.json"));
        ret.muzzleSize = {50, 50};
        ret.muzzleCenter = {5, 25};
        ret.muzzleOffset = {-30, 10};

        return ret;
    }

    float weight() const override {
        return 0.1f;
    }

    Vec2f getAngleBounds() const override {
        return {-360, 360};
    }

    bool shoot(DeltaTime deltaTime) override {
        if (hammer) {
            hammer = !hammer;
            return Weapon::shoot(deltaTime);
        }
        return false;
    }

    void pullTrigger(DeltaTime deltaTime) override {
        hammer = true;
    }

private:
    bool hammer = false;
    ResourceHandle<Sprite> sprite;
};

#endif //FOXTROT_PISTOL_HPP

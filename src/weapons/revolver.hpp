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

#ifndef FOXTROT_REVOLVER_HPP
#define FOXTROT_REVOLVER_HPP

#include "weapon.hpp"

class Revolver : public Weapon {
public:
    explicit Revolver()
            : sprite(Uri("sprites/revolver.json$0")),
              spriteReload(Uri("sprites/revolver.json$2")) {
        reloadDuration = 2;
        clipSize = 9;
    }

    ~Revolver() override = default;

    Type getType() const override {
        return REVOLVER;
    }

    Visuals getVisuals() override {
        Visuals ret;
        ret.size = {70, 30};
        ret.center = {10, 20};
        ret.offset = {0, 0};

        if (reloadTimer > 0) {
            ret.sprite = spriteReload;
        } else {;
            ret.sprite = sprite;
        }

        ret.muzzleFlash = ResourceHandle<SpriteAnimation>(Uri("animations/muzzle_a.json"));
        ret.muzzleSize = {50, 50};
        ret.muzzleCenter = {5, 25};
        ret.muzzleOffset = {-60, 15};

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
    ResourceHandle<Sprite> spriteReload;
};

#endif //FOXTROT_REVOLVER_HPP

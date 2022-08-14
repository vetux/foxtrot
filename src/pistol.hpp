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
            : sprite(std::move(sprite)) {}

    ~Pistol() override = default;

    void update(DeltaTime deltaTime) override {
        if (chamberTimer > 0) {
            chamberTimer -= deltaTime;
            if (chamberTimer < 0) {
                chamberTimer = 0;
            }
        }
    }

    Type getType() override {
        return PISTOL;
    }

    Visuals getVisuals() override {
        Visuals ret;
        ret.size = {50, 25};
        ret.center = {10, 16};
        ret.offset = {-15, 7};
        ret.sprite = sprite;

        ret.muzzleFlash = ResourceHandle<SpriteAnimation>(Uri("animations/muzzle_a.json"));
        ret.muzzleSize = {50, 50};
        ret.muzzleCenter = {5, 25};
        ret.muzzleOffset = {-40, 10};

        return ret;
    }

    void setAmmo(int value) override {
        ammo = value;
    }

    int getAmmo() override {
        return ammo;
    }

    bool shoot() override {
        if (ammo <= 0 || chamberTimer > 0)
            return false;
        ammo--;
        chamberTimer += 1.0f / roundsPerSecond;
        return true;
    }

    float weight() override {
        return 0.75f;
    }

    Vec2f getAngleBounds() override {
        return {-360, 360};
    }

private:
    int ammo = 0;

    float roundsPerSecond = 1;

    float chamberTimer = 0;

    ResourceHandle<Sprite> sprite;
};

#endif //FOXTROT_PISTOL_HPP

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

#ifndef FOXTROT_GATLING_HPP
#define FOXTROT_GATLING_HPP

#include "weapon.hpp"

class Gatling : public Weapon {
public:
    Gatling() = default;

    Gatling(ResourceHandle<Sprite> gatling_fire_0,
            ResourceHandle<Sprite> gatling_fire_1,
            ResourceHandle<Sprite> gatling_lowammo_1,
            ResourceHandle<Sprite> gatling_lowammo_2,
            ResourceHandle<Sprite> gatling_lowammo_3,
            ResourceHandle<Sprite> gatling_lowammo_4,
            ResourceHandle<Sprite> gatling_lowammo_5,
            ResourceHandle<Sprite> gatling_lowammo_6,
            ResourceHandle<Sprite> gatling_unloaded_0,
            ResourceHandle<Sprite> gatling_unloaded_1)
            : gatling_fire_0(std::move(gatling_fire_0)),
              gatling_fire_1(std::move(gatling_fire_1)),
              gatling_lowammo_1(std::move(gatling_lowammo_1)),
              gatling_lowammo_2(std::move(gatling_lowammo_2)),
              gatling_lowammo_3(std::move(gatling_lowammo_3)),
              gatling_lowammo_4(std::move(gatling_lowammo_4)),
              gatling_lowammo_5(std::move(gatling_lowammo_5)),
              gatling_lowammo_6(std::move(gatling_lowammo_6)),
              gatling_unloaded_0(std::move(gatling_unloaded_0)),
              gatling_unloaded_1(std::move(gatling_unloaded_1)) {}

    ~Gatling() override = default;

    void update(DeltaTime deltaTime) override {
        if (chamberTimer > 0) {
            chamberTimer -= deltaTime;
            if (chamberTimer < 0) {
                chamberTimer = 0;
            }
        }
    }

    Type getType() override {
        return GATLING;
    }

    Visuals getVisuals() override {
        Visuals ret;
        ret.size = {100, 100};
        ret.center = {20, 50};
        ret.offset = {-25, 5};

        switch (ammo) {
            case 0:
                ret.sprite = gatling_unloaded_0;
                break;
            case 1:
                ret.sprite = gatling_lowammo_1;
                break;
            case 2:
                ret.sprite = gatling_lowammo_2;
                break;
            case 3:
                ret.sprite = gatling_lowammo_3;
                break;
            case 4:
                ret.sprite = gatling_lowammo_4;
                break;
            case 5:
                ret.sprite = gatling_lowammo_5;
                break;
            case 6:
                ret.sprite = gatling_lowammo_6;
                break;
            default:
                ret.sprite = ammo % 2 == 0 ? gatling_fire_0 : gatling_fire_1;
                break;
        }

        ret.muzzleFlash = ResourceHandle<SpriteAnimation>(Uri("animations/muzzle_a.json"));
        ret.muzzleSize = {100, 100};
        ret.muzzleCenter = {10, 50};
        ret.muzzleOffset = {-80, 0};
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
        return 0.8f;
    }

    Vec2f getAngleBounds() override {
        return {-45, 45};
    }

private:
    int ammo = 0;

    float roundsPerSecond = 10;

    float chamberTimer = 0;

    ResourceHandle<Sprite> gatling_fire_0;
    ResourceHandle<Sprite> gatling_fire_1;
    ResourceHandle<Sprite> gatling_lowammo_1;
    ResourceHandle<Sprite> gatling_lowammo_2;
    ResourceHandle<Sprite> gatling_lowammo_3;
    ResourceHandle<Sprite> gatling_lowammo_4;
    ResourceHandle<Sprite> gatling_lowammo_5;
    ResourceHandle<Sprite> gatling_lowammo_6;
    ResourceHandle<Sprite> gatling_unloaded_0;
    ResourceHandle<Sprite> gatling_unloaded_1;
};

#endif //FOXTROT_GATLING_HPP

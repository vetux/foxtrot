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

#include <set>
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
            ResourceHandle<Sprite> gatling_unloaded_1,
            ResourceHandle<Sprite> gatling_fire_0_cycle,
            ResourceHandle<Sprite> gatling_fire_1_cycle,
            ResourceHandle<Sprite> gatling_lowammo_1_cycle,
            ResourceHandle<Sprite> gatling_lowammo_2_cycle,
            ResourceHandle<Sprite> gatling_lowammo_3_cycle,
            ResourceHandle<Sprite> gatling_lowammo_4_cycle,
            ResourceHandle<Sprite> gatling_lowammo_5_cycle,
            ResourceHandle<Sprite> gatling_lowammo_6_cycle,
            ResourceHandle<Sprite> gatling_unloaded_0_cycle,
            ResourceHandle<Sprite> gatling_unloaded_1_cycle)
            : gatling_fire_0(std::move(gatling_fire_0)),
              gatling_fire_1(std::move(gatling_fire_1)),
              gatling_lowammo_1(std::move(gatling_lowammo_1)),
              gatling_lowammo_2(std::move(gatling_lowammo_2)),
              gatling_lowammo_3(std::move(gatling_lowammo_3)),
              gatling_lowammo_4(std::move(gatling_lowammo_4)),
              gatling_lowammo_5(std::move(gatling_lowammo_5)),
              gatling_lowammo_6(std::move(gatling_lowammo_6)),
              gatling_unloaded_0(std::move(gatling_unloaded_0)),
              gatling_unloaded_1(std::move(gatling_unloaded_1)),
              gatling_fire_0_cycle(std::move(gatling_fire_0_cycle)),
              gatling_fire_1_cycle(std::move(gatling_fire_1_cycle)),
              gatling_lowammo_1_cycle(std::move(gatling_lowammo_1_cycle)),
              gatling_lowammo_2_cycle(std::move(gatling_lowammo_2_cycle)),
              gatling_lowammo_3_cycle(std::move(gatling_lowammo_3_cycle)),
              gatling_lowammo_4_cycle(std::move(gatling_lowammo_4_cycle)),
              gatling_lowammo_5_cycle(std::move(gatling_lowammo_5_cycle)),
              gatling_lowammo_6_cycle(std::move(gatling_lowammo_6_cycle)),
              gatling_unloaded_0_cycle(std::move(gatling_unloaded_0_cycle)),
              gatling_unloaded_1_cycle(std::move(gatling_unloaded_1_cycle)) {
        clipSize = 300;
        reloadDuration = 1;
        bulletSpread = 10;
    }

    ~Gatling() override = default;

    void update(DeltaTime deltaTime) override {
        if (engagedRotor) {
            engagedRotor = false;
        } else {
            if (rpm > 0)
                rpm -= deltaTime * spinDeceleration;
        }
        if (rpm > 0) {
            chamberTimer += deltaTime;
            if (chamberTimer >= 60 / rpm) {
                chamberTimer = 0;
                cycle = !cycle;
                chamber = true;
            }
        } else {
            chamberTimer = 0;
        }
        Weapon::update(deltaTime);
    }

    Type getType() const override {
        return GATLING;
    }

    std::set<Item::ID> getAmmoItems() const override {
        return {
                ItemIds::ITEM_AMMO_5_56
        };
    }

    Visuals getVisuals() override {
        Visuals ret;
        ret.size = {100, 100};
        ret.center = {20, 50};
        ret.offset = {};

        switch (clip) {
            case 0:
                if (!cycle)
                    ret.sprite = gatling_unloaded_0;
                else
                    ret.sprite = gatling_unloaded_0_cycle;
                break;
            case 6:
                if (!cycle)
                    ret.sprite = gatling_lowammo_1;
                else
                    ret.sprite = gatling_lowammo_1_cycle;
                break;
            case 5:
                if (!cycle)
                    ret.sprite = gatling_lowammo_2;
                else
                    ret.sprite = gatling_lowammo_2_cycle;
                break;
            case 4:
                if (!cycle)
                    ret.sprite = gatling_lowammo_3;
                else
                    ret.sprite = gatling_lowammo_3_cycle;
                break;
            case 3:
                if (!cycle)
                    ret.sprite = gatling_lowammo_4;
                else
                    ret.sprite = gatling_lowammo_4_cycle;
                break;
            case 2:
                if (!cycle)
                    ret.sprite = gatling_lowammo_5;
                else
                    ret.sprite = gatling_lowammo_5_cycle;
                break;
            case 1:
                if (!cycle)
                    ret.sprite = gatling_lowammo_6;
                else
                    ret.sprite = gatling_lowammo_6_cycle;
                break;
            default:
                if (!cycle)
                    ret.sprite = clip % 2 == 0 ? gatling_fire_0 : gatling_fire_1;
                else
                    ret.sprite = clip % 2 == 0 ? gatling_fire_0_cycle : gatling_fire_1_cycle;
                break;
        }

        ret.muzzleFlash = ResourceHandle<SpriteAnimation>(Uri("animations/muzzle_a.json"));
        ret.muzzleSize = {100, 100};
        ret.muzzleCenter = {10, 50};
        ret.muzzleOffset = {-80, 0};
        return ret;
    }

    bool shoot(DeltaTime deltaTime) override {
        accelerateRotor(deltaTime);

        if (chamber && reloadTimer <= 0) {
            if (Weapon::shoot(deltaTime)) {
                chamber = false;
                cycle = !cycle;
                return true;
            }
        }

        return false;
    }

    float weight() const override {
        return 0.8f;
    }

    Vec2f getAngleBounds() const override {
        return {-45, 45};
    }

    int reload(DeltaTime deltaTime) override {
        accelerateRotor(deltaTime);
        return Weapon::reload(deltaTime);
    }

    void accelerateRotor(DeltaTime deltaTime) {
        engagedRotor = true;
        rpm += deltaTime * spinAcceleration;
        if (rpm >= maxRpm) {
            rpm = maxRpm;
        }
    }

private:
    bool cycle = false;
    bool chamber = false;
    bool engagedRotor = false;

    float lastRotation = 0;
    float chamberTimer = 0;

    float rpm = 0;

    float maxRpm = 1750;
    float spinAcceleration = 50;
    float spinDeceleration = 50;

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
    ResourceHandle<Sprite> gatling_fire_0_cycle;
    ResourceHandle<Sprite> gatling_fire_1_cycle;
    ResourceHandle<Sprite> gatling_lowammo_1_cycle;
    ResourceHandle<Sprite> gatling_lowammo_2_cycle;
    ResourceHandle<Sprite> gatling_lowammo_3_cycle;
    ResourceHandle<Sprite> gatling_lowammo_4_cycle;
    ResourceHandle<Sprite> gatling_lowammo_5_cycle;
    ResourceHandle<Sprite> gatling_lowammo_6_cycle;
    ResourceHandle<Sprite> gatling_unloaded_0_cycle;
    ResourceHandle<Sprite> gatling_unloaded_1_cycle;
};

#endif //FOXTROT_GATLING_HPP

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

#ifndef FOXTROT_PLAYER_HPP
#define FOXTROT_PLAYER_HPP

#include "weapons/gatling.hpp"
#include "weapons/pistol.hpp"
#include "weapons/katana.hpp"

#include "inventory.hpp"
#include "account.hpp"

class Player : public Messageable {
public:
    enum Pose {
        GUN_AIM,
        GUN_HIP,
    };

    Player()
            : idleAnimationAim(Uri("animations/dante_idle.json")),
              walkAnimationAim(Uri("animations/dante_run.json")),
              runAnimationAim(Uri("animations/dante_run.json")),
              idleAnimationHip(Uri("animations/dante_idle_low.json")),
              runAnimationHip(Uri("animations/dante_run_low.json")),
              walkAnimationHip(Uri("animations/dante_run_low.json")),
              fallAnimation(Uri("animations/dante_fall.json")),
              deathAnimation(Uri("animations/dante_death.json")),
              pistol(ResourceHandle<Sprite>(Uri("sprites/pistol.json"))),
              gatling(ResourceHandle<Sprite>(Uri("sprites/gatling_fire_0.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_fire_1.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_lowammo_1.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_lowammo_2.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_lowammo_3.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_lowammo_4.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_lowammo_5.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_lowammo_6.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_unloaded_0.json")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling_unloaded_1.json"))) {}

    Weapon &getWeapon() {
        switch (equippedWeapon) {
            default:
            case Weapon::NONE:
                return noWeapon;
            case Weapon::PISTOL:
                return pistol;
            case Weapon::GATLING:
                return gatling;
        }
    }

    void setEquippedWeapon(Weapon::Type type) {
        equippedWeapon = type;
    }

    Weapon::Type getEquippedWeapon() const {
        return equippedWeapon;
    }

    void setPose(Pose value) {
        pose = value;
    }

    Pose getPose() const {
        return pose;
    }

    Vec2f getWeaponOffset() {
        if (falling){
            return Vec2f(-30, 50);
        } else {
            switch (pose) {
                default:
                case GUN_AIM:
                    return Vec2f(-45, 35);
                case GUN_HIP:
                    return Vec2f(-25, 5);
            }
        }
    }

    void setInventory(const Inventory &inv) {
        inventory = inv;
    }

    const Inventory &getInventory() const {
        return inventory;
    }

    void setAccount(const Account &acc) {
        account = acc;
    }

    const Account &getAccount() const {
        return account;
    }

    void setIsFalling(bool value){
        falling = value ;
    }

    float getMaxVelocity() {
        return 20;
    }

    ResourceHandle<SpriteAnimation> getIdleAnimation() const {
        switch (pose) {
            default:
            case GUN_HIP:
                return idleAnimationHip;
            case GUN_AIM:
                return idleAnimationAim;

        }
    }

    ResourceHandle<SpriteAnimation> getWalkAnimation() const {
        switch (pose) {
            default:
            case GUN_HIP:
                return walkAnimationHip;
            case GUN_AIM:
                return walkAnimationAim;

        }
    }

    ResourceHandle<SpriteAnimation> getRunAnimation() const {
        switch (pose) {
            default:
            case GUN_HIP:
                return runAnimationHip;
            case GUN_AIM:
                return runAnimationAim;

        }
    }

    ResourceHandle<SpriteAnimation> getFallAnimation() const {
        return fallAnimation;
    }

    ResourceHandle<SpriteAnimation> getDeathAnimation() const {
        return deathAnimation;
    }

    void update(DeltaTime deltaTime) {
        pistol.update(deltaTime);
        gatling.update(deltaTime);
    }

    Messageable &operator<<(const Message &message) override {
        account << message.value("account");
        inventory << message.value("inventory");
        equippedWeapon = (Weapon::Type) message.value("equippedWeapon", (int) Weapon::NONE);
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);
        account >> message["account"];
        inventory >> message["inventory"];
        message["equippedWeapon"] = equippedWeapon;
        return message;
    }

private:
    Account account;
    Inventory inventory;
    Weapon::Type equippedWeapon = Weapon::NONE;
    Pose pose;

    Vec2f velocity;

    ResourceHandle<SpriteAnimation> idleAnimationAim;
    ResourceHandle<SpriteAnimation> walkAnimationAim;
    ResourceHandle<SpriteAnimation> runAnimationAim;

    ResourceHandle<SpriteAnimation> idleAnimationHip;
    ResourceHandle<SpriteAnimation> walkAnimationHip;
    ResourceHandle<SpriteAnimation> runAnimationHip;

    ResourceHandle<SpriteAnimation> fallAnimation;

    ResourceHandle<SpriteAnimation> deathAnimation;

    Weapon noWeapon;
    Pistol pistol;
    Gatling gatling;
    Katana katana;

    bool falling = false;
};

#endif //FOXTROT_PLAYER_HPP

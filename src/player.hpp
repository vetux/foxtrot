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
#include "weapons/revolver.hpp"
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
              pistol(),
              gatling(ResourceHandle<Sprite>(Uri("sprites/gatling.json/0")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/2")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/4")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/6")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/8")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/10")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/12")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/14")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/16")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/18")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/1")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/3")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/5")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/7")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/9")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/11")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/13")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/15")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/17")),
                      ResourceHandle<Sprite>(Uri("sprites/gatling.json/19"))) {}

    Weapon &getWeapon() {
        switch (equippedWeapon) {
            default:
            case Weapon::NONE:
                return noWeapon;
            case Weapon::REVOLVER:
                return pistol;
            case Weapon::GATLING:
                return gatling;
        }
    }

    const Weapon &getWeapon() const {
        switch (equippedWeapon) {
            default:
            case Weapon::NONE:
                return noWeapon;
            case Weapon::REVOLVER:
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
       /* if (falling){
            return Vec2f(-30, 50);
        } else {*/
            switch (pose) {
                default:
                case GUN_AIM:
                    return Vec2f(-45, 35);
                case GUN_HIP:
                    return Vec2f(-25, 5);
            }
       // }
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
        return 12;
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
    Revolver pistol;
    Gatling gatling;
    Katana katana;

    bool falling = false;
};

#endif //FOXTROT_PLAYER_HPP

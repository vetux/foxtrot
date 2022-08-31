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
#include "inventory.hpp"
#include "account.hpp"

class Player : public Messageable {
public:
    Player()
            : pistol(ResourceHandle<Sprite>(Uri("sprites/pistol.json"))),
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

    Weapon noWeapon;
    Pistol pistol;
    Gatling gatling;
};

#endif //FOXTROT_PLAYER_HPP

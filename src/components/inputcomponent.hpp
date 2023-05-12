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

#ifndef FOXTROT_INPUTCOMPONENT_HPP
#define FOXTROT_INPUTCOMPONENT_HPP

#include "xng/xng.hpp"

#include "weapon.hpp"
#include "player.hpp"

using namespace xng;

struct InputComponent : public Component {
    int slot = 0; // The InputSystem decides how to map input devices to slots, negative slots are not updated by the InputSystem

    bool aim = false; // If true the user wants to aim to the aimPosition
    Vec2f aimPosition; // The cursor position relative to the screen
    bool fire = false; // If true the user wants to shoot in this frame
    bool fireHold = false; // If true the user wants to shoot in this frame
    bool reload = false; // If true the user wants to reload in this frame
    Vec2f movement = Vec2f(0); // The normalized input movement vector
    Weapon::Type weapon = Weapon::NONE; // The requested weapon
    Player::Pose pose = Player::GUN_HIP;

    Messageable &operator<<(const Message &message) override {
        message.value("slot", slot);
        message.value("aim", aim, false);
        message.value("aimPosition", aimPosition);
        message.value("fire", fire);
        message.value("reload", reload);
        message.value("movement", movement);
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);
        message["slot"] = slot;
        message["aim"] = aim;
        aimPosition >> message["aimPosition"];
        message["fire"] = fire;
        message["reload"] = reload;
        movement >> message["movement"];
        return message;
    }

    std::type_index getType() const override {
        return typeid(InputComponent);
    }
};

#endif //FOXTROT_INPUTCOMPONENT_HPP

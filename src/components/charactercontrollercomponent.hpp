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

#ifndef FOXTROT_CHARACTERCONTROLLERCOMPONENT_HPP
#define FOXTROT_CHARACTERCONTROLLERCOMPONENT_HPP

#include "xng/xng.hpp"

#include "player.hpp"

struct CharacterControllerComponent : public Component {
    bool enabled = true;

    ResourceHandle<SpriteAnimation> idleAnimation;
    ResourceHandle<SpriteAnimation> walkAnimation;
    ResourceHandle<SpriteAnimation> runAnimation;
    ResourceHandle<SpriteAnimation> fallAnimation;
    ResourceHandle<SpriteAnimation> deathAnimation;

    float damageTimer = 0;
    float damageMix = 0.3f;
    ColorRGB damageColor = ColorRGB(255, 26, 26);

    float maxVelocity = 20;
    float acceleration = 0.5;
    float drag = 0.2f;
    float walkVelocity = 0.1;
    float runVelocity = 5;
    float fallVelocity = 10;

    bool facingLeft = false;

    bool isOnFloor = true;

    Messageable &operator<<(const Message &message) override {
        message.value("enabled", enabled, true);
        message.value("idleAnimation", idleAnimation);
        message.value("walkAnimation", walkAnimation);
        message.value("runAnimation", runAnimation);
        message.value("deathAnimation", deathAnimation);
        message.value("facingLeft", facingLeft, false);
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);
        message["enabled"] = enabled;
        idleAnimation >> message["idleAnimation"];
        walkAnimation >> message["walkAnimation"];
        runAnimation >> message["runAnimation"];
        deathAnimation >> message["deathAnimation"];
        message["facingLeft"] = facingLeft;
        return message;
    }

    std::type_index getType() const override {
        return typeid(CharacterControllerComponent);
    }
};

#endif //FOXTROT_CHARACTERCONTROLLERCOMPONENT_HPP

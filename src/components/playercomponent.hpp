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

#ifndef FOXTROT_PLAYERCOMPONENT_HPP
#define FOXTROT_PLAYERCOMPONENT_HPP

#include "xengine.hpp"

#include "player.hpp"

using namespace xng;

struct PlayerComponent : public Component {
    ResourceHandle<SpriteAnimation> idleAnimation;
    ResourceHandle<SpriteAnimation> walkAnimation;

    ResourceHandle<SpriteAnimation> idleAnimationLow;
    ResourceHandle<SpriteAnimation> walkAnimationLow;

    Player player;

    Messageable &operator<<(const Message &message) override {
        idleAnimation << message.value("idleAnimation");
        walkAnimation << message.value("walkAnimation");
        idleAnimationLow << message.value("idleAnimationLow");
        walkAnimationLow << message.value("walkAnimationLow");
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);
        idleAnimation >> message["idleAnimation"];
        walkAnimation >> message["walkAnimation"];
        idleAnimationLow >> message["idleAnimationLow"];
        walkAnimationLow >> message["walkAnimationLow"];
        return message;
    }

    std::type_index getType() const override {
        return typeid(PlayerComponent);
    }
};

#endif //FOXTROT_PLAYERCOMPONENT_HPP

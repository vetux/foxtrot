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

#ifndef FOXTROT_HEALTHCOMPONENT_HPP
#define FOXTROT_HEALTHCOMPONENT_HPP

#include "xng/xng.hpp"

struct HealthComponent : public Component {
    float health = 100.0f;

    Messageable &operator<<(const Message &message) override {
        health = message.value("health", 100.0f);
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);
        message["health"] = health;
        return message;
    }

    std::type_index getType() const override {
        return typeid(HealthComponent);
    }
};

#endif //FOXTROT_HEALTHCOMPONENT_HPP

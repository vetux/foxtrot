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

#ifndef FOXTROT_GAMEGUISYSTEM_HPP
#define FOXTROT_GAMEGUISYSTEM_HPP

#include "xengine.hpp"

#include "components/playercomponent.hpp"

using namespace xng;

class GameGuiSystem : public System, public EventListener {
public:
    explicit GameGuiSystem(Input &input, EventBus &bus)
            : input(input), eventBus(bus) {}

    void start(EntityScene &scene) override {
        eventBus.addListener(*this);
    }

    void stop(EntityScene &scene) override {
        eventBus.removeListener(*this);
    }

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        for (auto &pair: scene.getPool<InputComponent>()) {
            auto &health = scene.lookup<HealthComponent>(pair.first);
            if (health.health > 0 && pair.second.aim) {
                input.setMouseCursorHidden(true);
            } else {
                input.setMouseCursorHidden(false);
            }
            break;
        }
    }

    void onEvent(const Event &event) override {

    }

private:
    Input &input;
    EventBus &eventBus;
};

#endif //FOXTROT_GAMEGUISYSTEM_HPP

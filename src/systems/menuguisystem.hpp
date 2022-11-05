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

#ifndef FOXTROT_MENUGUISYSTEM_HPP
#define FOXTROT_MENUGUISYSTEM_HPP

#include "xng/xng.hpp"

using namespace xng;

class MenuGuiSystem : public System, public EventListener {
public:
    explicit MenuGuiSystem(Input &input)
            : input(input) {}

    void start(EntityScene &scene, EventBus &eventBus) override {
        eventBus.addListener(*this);
    }

    void stop(EntityScene &scene, EventBus &eventBus) override {
        eventBus.removeListener(*this);
    }

    void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override {
        for (auto &ev : events){
            eventBus.invoke(ev);
        }
        events.clear();
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(GuiEvent)) {
            auto &ev = event.as<GuiEvent>();
            if (ev.id == "button_start") {
                if (ev.type == GuiEvent::BUTTON_CLICK) {
                    events.emplace_back(LoadLevelEvent(LEVEL_ZERO));
                }
            }
        }
    }

private:
    Input &input;
    std::vector<LoadLevelEvent> events;
};

#endif //FOXTROT_MENUGUISYSTEM_HPP

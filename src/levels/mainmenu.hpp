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

#ifndef FOXTROT_MAINMENU_HPP
#define FOXTROT_MAINMENU_HPP

#include "xengine.hpp"

#include "level.hpp"
#include "events/loadlevelevent.hpp"

using namespace xng;

class MainMenu : public Level, public EventListener {
public:
    MainMenu(EventBus &eventBus,
             Window &window,
             Renderer2D &ren2d,
             FontDriver &fontDriver,
             Archive &archive,
             std::vector<std::shared_ptr<EntityScene>> scenes)
            : eventBus(eventBus),
              guiEventSystem(window, eventBus),
              canvasRenderSystem(ren2d,
                                 window.getRenderTarget(),
                                 fontDriver,
                                 archive),
              scenes(scenes) {}

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(GuiEvent)) {
            auto &ev = event.as<GuiEvent>();
            if (ev.id == "button_start") {
                if (ev.type == GuiEvent::BUTTON_CLICK) {
                    eventBus.invoke(LoadLevelEvent(LEVEL_0));
                }
            }
        }
    }

    LevelName getName() override {
        return MAIN_MENU;
    }

    void onCreate(ECS &ecs) override {
        eventBus.addListener(*this);

        ecs.setSystems({guiEventSystem, spriteAnimationSystem, canvasRenderSystem});
        ecs.setScene(scenes.at(0));
        ecs.start();
    }

    void onDestroy(ECS &ecs) override {
        eventBus.removeListener(*this);
    }

private:
    GuiEventSystem guiEventSystem;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    std::vector<std::shared_ptr<EntityScene>> scenes;
    EventBus &eventBus;
};

#endif //FOXTROT_MAINMENU_HPP

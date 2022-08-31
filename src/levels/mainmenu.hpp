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

#include "systems/menuguisystem.hpp"

using namespace xng;

class MainMenu : public Level {
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
              menuGuiSystem(eventBus),
              scenes(scenes) {}

    LevelName getName() override {
        return MAIN_MENU;
    }

    void onCreate(ECS &ecs) override {
        ecs.setSystems({guiEventSystem, menuGuiSystem, spriteAnimationSystem, canvasRenderSystem});
        ecs.setScene(scenes.at(0));
        ecs.start();
    }

    void onDestroy(ECS &ecs) override {}

private:
    GuiEventSystem guiEventSystem;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    MenuGuiSystem menuGuiSystem;
    std::vector<std::shared_ptr<EntityScene>> scenes;
    EventBus &eventBus;
};

#endif //FOXTROT_MAINMENU_HPP

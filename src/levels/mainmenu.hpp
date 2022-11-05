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

#include "xng/xng.hpp"

#include "level.hpp"
#include "events/loadlevelevent.hpp"

#include "systems/menuguisystem.hpp"

using namespace xng;

class MainMenu : public Level, public EventListener {
public:
    MainMenu(std::shared_ptr<EventBus> eventBus,
             Window &window,
             Renderer2D &ren2d,
             FontDriver &fontDriver)
            : eventBus(eventBus),
              guiEventSystem(window),
              canvasRenderSystem(ren2d,
                                 window.getRenderTarget(),
                                 fontDriver),
              menuGuiSystem(window.getInput()) {
    }

    LevelID getID() override {
        return LEVEL_MAIN_MENU;
    }

    void onStart(ECS &ecs) override {
        auto handle = ResourceHandle<EntityScene>(Uri("scenes/menu.json"));
        eventBus->addListener(*this);
        scene = std::make_shared<EntityScene>(handle.get());
        ecs.setScene(scene);
        ecs.setEventBus(eventBus);
        ecs.setSystems({guiEventSystem, menuGuiSystem, spriteAnimationSystem, canvasRenderSystem});
        ecs.start();
    }

    void onStop(ECS &ecs) override {
        eventBus->removeListener(*this);
        ecs.stop();
        ecs.setScene({});
        ecs.setSystems({});
        scene = {};
    }

    void onUpdate(ECS &ecs, DeltaTime deltaTime) override {
        ecs.update(deltaTime);
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(InputEvent)) {
            auto &ev = event.as<InputEvent>();
            if (ev.deviceType == xng::InputEvent::DEVICE_KEYBOARD) {
                auto &kbev = std::get<KeyboardEventData>(ev.data);
                if (kbev.type == xng::KeyboardEventData::KEYBOARD_KEY_DOWN
                    && kbev.key == xng::KEY_F1) {
                    drawDebug = !drawDebug;
                    canvasRenderSystem.setDrawDebugGeometry(drawDebug);
                }
            }
        }
    }

private:
    std::shared_ptr<EventBus> eventBus;

    GuiEventSystem guiEventSystem;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    MenuGuiSystem menuGuiSystem;

    std::shared_ptr<EntityScene> scene;

    bool drawDebug = false;
};

#endif //FOXTROT_MAINMENU_HPP

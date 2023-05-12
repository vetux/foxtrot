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

#include <utility>

#include "xng/xng.hpp"

#include "level.hpp"
#include "events/loadlevelevent.hpp"

#include "systems/menuguisystem.hpp"

using namespace xng;

class MainMenu : public Level, public EventListener {
public:
    MainMenu(std::shared_ptr<EventBus> eventBus,
             Window &window,
             RenderTarget &target,
             Renderer2D &ren2d,
             FontDriver &fontDriver)
            : eventBus(std::move(eventBus)),
              guiEventSystem(std::make_shared<GuiEventSystem>(window)),
              menuGuiSystem(std::make_shared<MenuGuiSystem>(window.getInput())),
              spriteAnimationSystem(std::make_shared<SpriteAnimationSystem>()),
              canvasRenderSystem(std::make_shared<CanvasRenderSystem>(ren2d,
                                                                      target,
                                                                      fontDriver)) {
    }

    ~MainMenu() {

    }

    LevelID getID() override {
        return LEVEL_MAIN_MENU;
    }

    void onStart() override {
        auto handle = ResourceHandle<EntityScene>(Uri("scenes/menu.json"));
        eventBus->addListener(*this);
        scene = std::make_shared<EntityScene>(handle.get());
        ecs = SystemRuntime({SystemPipeline(xng::SystemPipeline::TICK_FRAME,
                                            {guiEventSystem,
                                             menuGuiSystem,
                                             spriteAnimationSystem,
                                             canvasRenderSystem})},
                            scene,
                            eventBus);
        ecs.start();
    }

    void onStop() override {
        eventBus->removeListener(*this);
        ecs.stop();
        ecs = SystemRuntime();
        scene = {};
    }

    void onUpdate(DeltaTime deltaTime) override {
        ecs.update(deltaTime);
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(KeyboardEvent)) {
            auto &kbev = event.as<KeyboardEvent>();
            if (kbev.type == xng::KeyboardEvent::KEYBOARD_KEY_DOWN
                && kbev.key == xng::KEY_F1) {
                drawDebug = !drawDebug;
                canvasRenderSystem->setDrawDebugGeometry(drawDebug);
            }
        }
    }

private:
    std::shared_ptr<EventBus> eventBus;

    std::shared_ptr<GuiEventSystem> guiEventSystem;
    std::shared_ptr<CanvasRenderSystem> canvasRenderSystem;
    std::shared_ptr<SpriteAnimationSystem> spriteAnimationSystem;
    std::shared_ptr<MenuGuiSystem> menuGuiSystem;

    std::shared_ptr<EntityScene> scene;

    SystemRuntime ecs;

    bool drawDebug = false;
};

#endif //FOXTROT_MAINMENU_HPP

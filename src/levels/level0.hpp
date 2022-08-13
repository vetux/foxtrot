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

#ifndef FOXTROT_LEVEL0_HPP
#define FOXTROT_LEVEL0_HPP

#include <utility>

#include "level.hpp"

#include "systems/playercontrollersystem.hpp"
#include "systems/camerasystem.hpp"
#include "systems/daytimesystem.hpp"

class Level0 : public Level, public EventListener {
public:
    Level0(EventBus &eventBus,
           Window &window,
           Renderer2D &ren2d,
           FontDriver &fontDriver,
           Archive &archive,
           std::vector<std::shared_ptr<EntityScene>> scenes)
            : eventBus(eventBus),
              target(window.getRenderTarget()),
              eventSystem(window, eventBus),
              guiEventSystem(window, eventBus),
              playerControllerSystem(window.getInput(), eventBus),
              canvasRenderSystem(ren2d,
                                 window.getRenderTarget(),
                                 fontDriver,
                                 archive),
              physicsDriver(DriverRegistry::load<PhysicsDriver>("box2d")),
              world(physicsDriver->createWorld()),
              physicsSystem(*world, eventBus, 30),
              cameraSystem(window.getRenderTarget()),
              scenes(std::move(scenes)),
              ren2d(ren2d) {
        world->setGravity(Vec3f(0, -20, 0));
    }

    ~Level0() {
    }

    LevelName getName() override {
        return LEVEL_0;
    }

    void onCreate(ECS &ecs) override {
        eventBus.addListener(*this);
        ecs.setSystems(
                {daytimeSystem,
                 eventSystem,
                 guiEventSystem,
                 spriteAnimationSystem,
                 playerControllerSystem,
                 physicsSystem,
                 cameraSystem,
                 canvasRenderSystem});
        ecs.setScene(scenes.at(1));
        ecs.start();
    }

    void onUpdate(ECS &ecs, DeltaTime deltaTime) override {
    }

    void onDestroy(ECS &ecs) override {
        eventBus.removeListener(*this);
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(InputEvent)) {
            auto &ev = event.as<InputEvent>();
            if (ev.deviceType == xng::InputEvent::DEVICE_KEYBOARD) {
                auto &kbev = std::get<KeyboardEvent>(ev.event);
                if (kbev.type == xng::KeyboardEvent::KEYBOARD_KEY_DOWN
                    && kbev.key == xng::KEY_F1) {
                    drawDebug = !drawDebug;
                    canvasRenderSystem.setDrawDebug(drawDebug);
                }
            }
        }
    }

private:
    RenderTarget &target;
    Renderer2D &ren2d;

    std::unique_ptr<PhysicsDriver> physicsDriver;
    std::unique_ptr<World> world;
    std::vector<std::shared_ptr<EntityScene>> scenes;
    EventBus &eventBus;

    EventSystem eventSystem;
    GuiEventSystem guiEventSystem;
    DaytimeSystem daytimeSystem;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    PlayerControllerSystem playerControllerSystem;
    PhysicsSystem physicsSystem;
    CameraSystem cameraSystem;

    bool drawDebug = false;
};

#endif //FOXTROT_LEVEL0_HPP

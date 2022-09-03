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

#include "systems/inputsystem.hpp"
#include "systems/camerasystem.hpp"
#include "systems/timesystem.hpp"
#include "systems/bulletsystem.hpp"
#include "systems/inputsystem.hpp"
#include "systems/charactercontrollersystem.hpp"
#include "systems/playercontrollersystem.hpp"
#include "systems/gameguisystem.hpp"
#include "systems/cursorsystem.hpp"

class Level0 : public Level, public EventListener {
public:
    Level0(EventBus &eventBus,
           Window &window,
           Renderer2D &ren2d,
           FontDriver &fontDriver,
           std::vector<std::shared_ptr<EntityScene>> scenes)
            : eventBus(eventBus),
              target(window.getRenderTarget()),
              eventSystem(window, eventBus),
              guiEventSystem(window, eventBus),
              inputSystem(window.getInput()),
              characterControllerSystem(eventBus),
              playerControllerSystem(),
              canvasRenderSystem(ren2d,
                                 window.getRenderTarget(),
                                 fontDriver),
              bulletSystem(eventBus),
              gameGuiSystem(window.getInput(), eventBus),
              physicsDriver(DriverRegistry::load<PhysicsDriver>("box2d")),
              world(physicsDriver->createWorld()),
              physicsSystem(*world, eventBus, 30),
              cameraSystem(window.getRenderTarget(), Vec2f(-10100, -10100), Vec2f(10100, 100)),
              cursorSystem(window.getInput()),
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
                {eventSystem,
                 guiEventSystem,

                 physicsSystem,

                 daytimeSystem,
                 inputSystem,
                 characterControllerSystem,
                 playerControllerSystem,
                 bulletSystem,

                 cameraSystem,

                 gameGuiSystem,
                 cursorSystem,

                 spriteAnimationSystem,
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
                auto &kbev = std::get<KeyboardEventData>(ev.data);
                if (kbev.type == xng::KeyboardEventData::KEYBOARD_KEY_DOWN
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
    TimeSystem daytimeSystem;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    InputSystem inputSystem;
    CharacterControllerSystem characterControllerSystem;
    PlayerControllerSystem playerControllerSystem;
    GameGuiSystem gameGuiSystem;
    CursorSystem cursorSystem;

    PhysicsSystem physicsSystem;
    CameraSystem cameraSystem;
    BulletSystem bulletSystem;

    bool drawDebug = false;
};

#endif //FOXTROT_LEVEL0_HPP

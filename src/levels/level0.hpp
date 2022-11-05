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

//TODO: Fix box2d collisions not working.
class Level0 : public Level, public EventListener {
public:
    Level0(std::shared_ptr<EventBus> eventBus,
           Window &window,
           Renderer2D &ren2d,
           FontDriver &fontDriver,
           AudioDevice &audioDevice)
            : eventBus(eventBus),
              target(window.getRenderTarget()),
              eventSystem(window),
              guiEventSystem(window),
              inputSystem(window.getInput()),
              characterControllerSystem(),
              playerControllerSystem(),
              canvasRenderSystem(ren2d,
                                 window.getRenderTarget(),
                                 fontDriver),
              bulletSystem(),
              gameGuiSystem(window.getInput()),
              physicsDriver(DriverRegistry::load<PhysicsDriver>("box2d")),
              world(physicsDriver->createWorld()),
              physicsSystem(*world, 30, 1.0f / 300),
              cameraSystem(window.getRenderTarget(), Vec2f(-10100, -10100), Vec2f(10100, 100)),
              cursorSystem(window.getInput()),
              audioSystem(audioDevice, ResourceRegistry::getDefaultRegistry()),
              ren2d(ren2d) {
        world->setGravity(Vec3f(0, -20, 0));
    }

    ~Level0() {}

    LevelID getID() override {
        return LEVEL_ZERO;
    }

    void startLoad(LoadListener &listener) override {
        loadTask = ThreadPool::getPool().addTask([this, &listener]() {
            auto handle = ResourceHandle<EntityScene>(Uri("scenes/level_0.json"));
            auto s = handle.get();
            scene = std::make_shared<EntityScene>(s);
            listener.onLoadProgress(getID(), 0.5);
            ResourceRegistry::getDefaultRegistry().awaitImports();
            listener.onLoadProgress(getID(), 1);
            listener.onLoadFinish(getID());
        });
    }

    void awaitLoad() override {
        loadTask->join();
    }

    void unload() override {
        Level::unload();
    }

    void onStart(ECS &ecs) override {
        eventBus->addListener(*this);
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
                 canvasRenderSystem,

                 audioSystem});
        ecs.setScene(scene);
        ecs.setEventBus(eventBus);
        ecs.start();
    }

    void onUpdate(ECS &ecs, DeltaTime deltaTime) override {
        ecs.update(deltaTime);
    }

    void onStop(ECS &ecs) override {
        ecs.stop();
        ecs.setScene({});
        ecs.setSystems({});
        scene = {};
        eventBus->removeListener(*this);
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
    RenderTarget &target;
    Renderer2D &ren2d;

    std::shared_ptr<EventBus> eventBus;

    std::shared_ptr<EntityScene> scene;

    std::unique_ptr<PhysicsDriver> physicsDriver;
    std::unique_ptr<World> world;

    EventSystem eventSystem;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    AudioSystem audioSystem;
    InputSystem inputSystem;

    GuiEventSystem guiEventSystem;
    TimeSystem daytimeSystem;
    CharacterControllerSystem characterControllerSystem;
    PlayerControllerSystem playerControllerSystem;
    GameGuiSystem gameGuiSystem;
    CursorSystem cursorSystem;

    PhysicsSystem physicsSystem;
    CameraSystem cameraSystem;
    BulletSystem bulletSystem;

    bool drawDebug = false;

    std::shared_ptr<Task> loadTask;
};

#endif //FOXTROT_LEVEL0_HPP

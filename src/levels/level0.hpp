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
    Level0(std::shared_ptr<EventBus> eventBus,
           Window &window,
           RenderTarget &target,
           Renderer2D &ren2d,
           FontDriver &fontDriver,
           PhysicsDriver &physicsDriver,
           AudioDevice &audioDevice)
            : eventBus(std::move(eventBus)),
              target(target),
              physicsDriver(physicsDriver),
              world(physicsDriver.createWorld()),
              guiEventSystem(std::make_shared<GuiEventSystem>(window)),
              inputSystem(std::make_shared<InputSystem>(window.getInput())),
              characterControllerSystem(std::make_shared<CharacterControllerSystem>()),
              playerControllerSystem(std::make_shared<PlayerControllerSystem>()),
              canvasRenderSystem(std::make_shared<CanvasRenderSystem>(ren2d,
                                                                      target,
                                                                      fontDriver)),
              bulletSystem(std::make_shared<BulletSystem>()),
              gameGuiSystem(std::make_shared<GameGuiSystem>(window.getInput())),
              physicsSystem(std::make_shared<PhysicsSystem>(*world, 30, 1.0f / 300)),
              cameraSystem(std::make_shared<CameraSystem>(target, Vec2f(-10100, -10100), Vec2f(10100, 100))),
              cursorSystem(std::make_shared<CursorSystem>(window.getInput())),
              audioSystem(std::make_shared<AudioSystem>(audioDevice, ResourceRegistry::getDefaultRegistry())),
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

    void onStart() override {
        eventBus->addListener(*this);
        ecs = SystemRuntime({SystemPipeline(xng::SystemPipeline::TICK_FRAME,
                                            {guiEventSystem,

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

                                             audioSystem})},
                            scene,
                            eventBus);
        ecs.start();
    }

    void onUpdate(DeltaTime deltaTime) override {
        ecs.update(deltaTime);
    }

    void onStop() override {
        ecs = SystemRuntime();
        scene = {};
        eventBus->removeListener(*this);
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
    RenderTarget &target;
    Renderer2D &ren2d;

    PhysicsDriver &physicsDriver;

    std::shared_ptr<EventBus> eventBus;

    std::shared_ptr<EntityScene> scene;

    std::unique_ptr<World> world;

    SystemRuntime ecs;

    std::shared_ptr<CanvasRenderSystem> canvasRenderSystem;
    std::shared_ptr<SpriteAnimationSystem> spriteAnimationSystem;
    std::shared_ptr<AudioSystem> audioSystem;
    std::shared_ptr<InputSystem> inputSystem;

    std::shared_ptr<GuiEventSystem> guiEventSystem;
    std::shared_ptr<TimeSystem> daytimeSystem;
    std::shared_ptr<CharacterControllerSystem> characterControllerSystem;
    std::shared_ptr<PlayerControllerSystem> playerControllerSystem;
    std::shared_ptr<GameGuiSystem> gameGuiSystem;
    std::shared_ptr<CursorSystem> cursorSystem;

    std::shared_ptr<PhysicsSystem> physicsSystem;
    std::shared_ptr<CameraSystem> cameraSystem;
    std::shared_ptr<BulletSystem> bulletSystem;

    bool drawDebug = false;

    std::shared_ptr<Task> loadTask;
};

#endif //FOXTROT_LEVEL0_HPP

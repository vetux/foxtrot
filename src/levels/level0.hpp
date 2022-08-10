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

#include "level.hpp"

#include "systems/playercontrollersystem.hpp"

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
              guiEventSystem(window, eventBus),
              playerControllerSystem(window.getInput()),
              canvasRenderSystem(ren2d,
                                 window.getRenderTarget(),
                                 fontDriver,
                                 archive),
              physicsDriver(DriverRegistry::load<PhysicsDriver>("box2d")),
              world(physicsDriver->createWorld()),
              physicsSystem(*world, eventBus),
              scenes(scenes),
              ren2d(ren2d) {}

    LevelName getName() override {
        return LEVEL_0;
    }

    void onCreate(ECS &ecs) override {
        eventBus.addListener(*this);
        canvasRenderSystem.setDrawDebug(true);
        ecs.setSystems(
                {playerControllerSystem, guiEventSystem, spriteAnimationSystem, physicsSystem, canvasRenderSystem});
        ecs.setScene(scenes.at(1));
        ecs.start();
    }

    void onUpdate(ECS &ecs, DeltaTime deltaTime) override {
        // ren2d.renderClear(target, ColorRGBA::green(), {}, target.getDescription().size);
    }

    void onDestroy(ECS &ecs) override {
        eventBus.removeListener(*this);
    }

    void onEvent(const Event &event) override {

    }

private:
    RenderTarget &target;
    Renderer2D &ren2d;

    std::unique_ptr<PhysicsDriver> physicsDriver;
    std::unique_ptr<World> world;
    std::vector<std::shared_ptr<EntityScene>> scenes;
    EventBus &eventBus;

    GuiEventSystem guiEventSystem;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    PlayerControllerSystem playerControllerSystem;
    PhysicsSystem physicsSystem;
};

#endif //FOXTROT_LEVEL0_HPP

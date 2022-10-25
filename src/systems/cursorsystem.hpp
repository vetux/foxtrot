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

#ifndef FOXTROT_CURSORSYSTEM_HPP
#define FOXTROT_CURSORSYSTEM_HPP

#include "xengine.hpp"

using namespace xng;

class CursorSystem : public System {
public:
    CursorSystem(Input &input) : input(input) {}

    void start(EntityScene &scene) override {
        Entity ent = scene.createEntity();
        auto rt = CanvasTransformComponent();
        rt.center = Vec2f(32, 32);
        rt.rect.dimensions = Vec2f(64, 64);
        rt.canvas = "OverlayCanvas";
        ent.createComponent(rt);
        auto sprite = SpriteComponent();
        sprite.sprite = ResourceHandle<Sprite>(Uri("file://sprites/crosshair.json/target"));
        sprite.layer = 10;
        ent.createComponent(sprite);
        crossHairEntity = ent;
    }

    void stop(EntityScene &scene) override {
        scene.destroy(crossHairEntity.getHandle());
    }

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        for (auto &player: scene.getPool<PlayerComponent>()) {
            auto &ic = scene.getComponent<InputComponent>(player.first);
            input.setMouseCursorHidden(ic.aim);

            auto rt = crossHairEntity.getComponent<CanvasTransformComponent>();
            if (ic.aim) {
                rt.rect.position = input.getMouse().position.convert<float>();
            }
            rt.enabled = ic.aim;
            crossHairEntity.updateComponent(rt);
            break;
        }
    }

private:
    Input &input;

    Entity crossHairEntity;
};

#endif //FOXTROT_CURSORSYSTEM_HPP

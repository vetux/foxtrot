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

#ifndef FOXTROT_CAMERASYSTEM_HPP
#define FOXTROT_CAMERASYSTEM_HPP

#include "xng/xng.hpp"

#include "components/charactercontrollercomponent.hpp"

using namespace xng;

class CameraSystem : public System {
public:
    explicit CameraSystem(RenderTarget &target, Vec2f cameraMin, Vec2f cameraMax)
            : target(target),
              cameraBoundMin(std::move(cameraMin)),
              cameraBoundMax(std::move(cameraMax)) {}

    void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override {
        Vec3f playerPosition;
        for (auto &pair: scene.getPool<CharacterControllerComponent>()) {
            auto &tcomp = scene.getComponent<TransformComponent>(pair.first);
            playerPosition = tcomp.transform.getPosition();
            break;
        }

        std::vector<EntityHandle> ents = {
                scene.getEntityByName("MainCanvas"),
        };

        for (auto &canvasEnt: ents) {
            auto comp = scene.getComponent<CanvasComponent>(canvasEnt);

            auto halfSize = target.getDescription().size.convert<float>() / 2;

            comp.cameraPosition.x = -playerPosition.x;
            if (comp.cameraPosition.x - halfSize.x < cameraBoundMin.x) {
                comp.cameraPosition.x = cameraBoundMin.x + halfSize.x;
            } else if (comp.cameraPosition.x + halfSize.x > cameraBoundMax.x) {
                comp.cameraPosition.x = cameraBoundMax.x - halfSize.x;
            }
            comp.cameraPosition.x -= halfSize.x;

            comp.cameraPosition.y = -playerPosition.y;
            if (comp.cameraPosition.y - halfSize.y < cameraBoundMin.y) {
                comp.cameraPosition.y = cameraBoundMin.y + halfSize.y;
            } else if (comp.cameraPosition.y + halfSize.y > cameraBoundMax.y) {
                comp.cameraPosition.y = cameraBoundMax.y - halfSize.y;
            }
            comp.cameraPosition.y -= halfSize.y;

            scene.updateComponent(canvasEnt, comp);
        }
    }

    void setCameraBounds(const Vec2f &boundMin, const Vec2f &boundMax) {
        cameraBoundMin = boundMin;
        cameraBoundMax = boundMax;
    }

private:
    RenderTarget &target;

    Vec2f cameraBoundMin;
    Vec2f cameraBoundMax;
};

#endif //FOXTROT_CAMERASYSTEM_HPP

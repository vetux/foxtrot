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

#include "xengine.hpp"

#include "components/playercontrollercomponent.hpp"

using namespace xng;

class CameraSystem : public System {
public:
    explicit CameraSystem(RenderTarget &target) : target(target) {}

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        Vec3f playerPosition;
        for (auto &pair: scene.getPool<PlayerControllerComponent>()) {
            auto &tcomp = scene.lookup<TransformComponent>(pair.first);
            playerPosition = tcomp.transform.getPosition();
            break;
        }

        for (auto &pair: scene.getPool<CanvasComponent>()) {
            auto comp = pair.second;
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

            scene.updateComponent(pair.first, comp);
        }
    }

private:
    RenderTarget &target;

    Vec2f cameraBoundMin = Vec2f(-2000, -2000);
    Vec2f cameraBoundMax = Vec2f(2000, 2000);
};

#endif //FOXTROT_CAMERASYSTEM_HPP

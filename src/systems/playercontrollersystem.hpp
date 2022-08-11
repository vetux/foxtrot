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

#ifndef FOXTROT_PLAYERCONTROLLERSYSTEM_HPP
#define FOXTROT_PLAYERCONTROLLERSYSTEM_HPP

#include "xengine.hpp"

#include "components/playercontrollercomponent.hpp"

using namespace xng;

class PlayerControllerSystem : public System {
public:
    explicit PlayerControllerSystem(Input &input)
            : input(input) {}

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        auto inp = getInput();
        for (auto &pair: scene.getPool<PlayerControllerComponent>()) {
            auto &tcomp = scene.lookup<TransformComponent>(pair.first);
            auto rb = scene.lookup<RigidBodyComponent>(pair.first);

            const float factor = 100;
            rb.force = inp * Vec3f(factor);
            rb.forcePoint = tcomp.transform.getPosition();
            if (input.getKeyboard().getKey(KEY_Q)) {
                rb.torque = Vec3f(0, 0, -factor);
            } else if (input.getKeyboard().getKey(KEY_E)) {
                rb.torque = Vec3f(0, 0, factor);
            }

            scene.updateComponent(pair.first, rb);
        }
    }

private:
    Vec3f getInput() {
        Vec3f ret;
        auto &kb = input.getKeyboard();
        if (kb.getKey(KEY_W)) {
            ret.y = 1;
        } else if (kb.getKey(KEY_S)) {
            ret.y = -1;
        }
        if (kb.getKey(KEY_A)) {
            ret.x = 1;
        } else if (kb.getKey(KEY_D)) {
            ret.x = -1;
        }
        return ret;
    }

    Input &input;
};

#endif //FOXTROT_PLAYERCONTROLLERSYSTEM_HPP

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

#ifndef FOXTROT_INPUTSYSTEM_HPP
#define FOXTROT_INPUTSYSTEM_HPP

#include "xengine.hpp"

#include "components/inputcomponent.hpp"
#include "components/healthcomponent.hpp"

using namespace xng;

class InputSystem : public System {
public:
    explicit InputSystem(Input &input)
            : input(input) {}

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        std::map<EntityHandle, InputComponent> updates;
        for (auto &pair: scene.getPool<InputComponent>()) {
            if (pair.second.slot < 0)
                continue;

            auto comp = pair.second;

            auto &kb = input.getKeyboard();
            auto &mouse = input.getMouse();

            comp.movement = {};

            if (kb.getKey(KEY_W)) {
                comp.movement.y = 1;
            } else if (kb.getKey(xng::KEY_S)) {
                comp.movement.y = -1;
            }

            if (kb.getKey(xng::KEY_A)) {
                comp.movement.x = 1;
            } else if (kb.getKey(xng::KEY_D)) {
                comp.movement.x = -1;
            }

            if (kb.getKeyDown(xng::KEY_T)
                || mouse.getButtonDown(xng::RIGHT)) {
                comp.aim = !comp.aim;
            }

            if (kb.getKeyDown(xng::KEY_LCTRL)){
                switch(comp.pose){
                    case Player::GUN_HIP:
                        comp.pose = Player::GUN_AIM;
                        break;
                    default:
                        comp.pose = Player::GUN_HIP;
                        break;
                }
            }

            if (kb.getKeyDown(xng::KEY_1)){
                comp.weapon = Weapon::NONE;
            } else if (kb.getKeyDown(xng::KEY_2)){
                comp.weapon = Weapon::PISTOL;
            } else if (kb.getKeyDown(xng::KEY_3)){
                comp.weapon = Weapon::GATLING;
            }

            comp.aimPosition = mouse.position.convert<float>();

            comp.fire = mouse.getButton(xng::LEFT) || kb.getKey(xng::KEY_SPACE);
            comp.reload = kb.getKeyDown(xng::KEY_R);

            updates[pair.first] = comp;
        }

        for (auto &pair: updates) {
            scene.updateComponent(pair.first, pair.second);
        }
    }

private:
    Input &input;
};

#endif //FOXTROT_INPUTSYSTEM_HPP

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
#include "components/jumpresetcomponent.hpp"

using namespace xng;

class PlayerControllerSystem : public System, public EventListener {
public:
    explicit PlayerControllerSystem(Input &input, EventBus &eventBus)
            : input(input), eventBus(eventBus) {
        eventBus.addListener(*this);
    }

    virtual ~PlayerControllerSystem() {
        eventBus.removeListener(*this);
    }

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        for (auto &ev: contactEvents) {
            EntityHandle playerEnt;
            EntityHandle otherEnt;

            if (scene.check<PlayerControllerComponent>(ev.entityA)) {
                playerEnt = ev.entityA;
                otherEnt = ev.entityB;
            } else if (scene.check<PlayerControllerComponent>(ev.entityB)) {
                playerEnt = ev.entityB;
                otherEnt = ev.entityA;
            }

            if (playerEnt && otherEnt) {
                auto pl = scene.lookup<PlayerControllerComponent>(playerEnt);
                if (ev.type == xng::ContactEvent::BEGIN_CONTACT) {
                    pl.collidingEntities.insert(otherEnt);
                } else {
                    pl.collidingEntities.erase(otherEnt);
                }
                scene.updateComponent(playerEnt, pl);
            }
        }

        contactEvents.clear();

        auto inp = getInput();
        for (auto &pair: scene.getPool<PlayerControllerComponent>()) {
            auto &tcomp = scene.lookup<TransformComponent>(pair.first);
            auto rb = scene.lookup<RigidBodyComponent>(pair.first);
            auto anim = scene.lookup<SpriteAnimationComponent>(pair.first);
            auto sprite = scene.lookup<SpriteComponent>(pair.first);
            auto player = pair.second;

            if (rb.velocity.x < 0.1 && rb.velocity.x > -0.1) {
                anim.animation = pair.second.idleAnimation;
            } else {
                anim.animation = pair.second.walkAnimation;
            }

            if (rb.velocity.x != 0) {
                player.facingLeft = rb.velocity.x > 0;
                sprite.flipSprite.x = player.facingLeft;
            }

            const float factor = 50;

            if (inp.x != 0) {
                rb.velocity.x = inp.x * factor;
            } else {
                if (rb.velocity.x < -drag) {
                    rb.velocity.x += drag;
                } else if (rb.velocity.x > drag) {
                    rb.velocity.x -= drag;
                }
            }

            if (inp.y > 0) {
                bool canJump = false;
                for (auto &ent : player.collidingEntities){
                    if (scene.check<JumpResetComponent>(ent)){
                        canJump = true;
                        break;
                    }
                }
                if (canJump) {
                    rb.impulse = Vec3f(0, 10, 0);
                    rb.impulsePoint = tcomp.transform.getPosition();
                }
            }

            if (rb.velocity.x < 0.1 && rb.velocity.x > -0.1) {
                anim.animation = pair.second.idleAnimation;
            } else {
                anim.animation = pair.second.walkAnimation;
            }

            if (rb.velocity.x != 0) {
                player.facingLeft = rb.velocity.x > 0;
                sprite.flipSprite.x = player.facingLeft;
            }

            scene.updateComponent(pair.first, rb);
            scene.updateComponent(pair.first, anim);
            scene.updateComponent(pair.first, sprite);
            scene.updateComponent(pair.first, player);
        }
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(ContactEvent)) {
            contactEvents.emplace_back(event.as<ContactEvent>());
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
    EventBus &eventBus;

    std::vector<ContactEvent> contactEvents;

    const float drag = 0.1f;
};

#endif //FOXTROT_PLAYERCONTROLLERSYSTEM_HPP

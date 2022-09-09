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

#ifndef FOXTROT_CHARACTERCONTROLLERSYSTEM_HPP
#define FOXTROT_CHARACTERCONTROLLERSYSTEM_HPP

#include "xengine.hpp"

#include "components/charactercontrollercomponent.hpp"
#include "components/floorcomponent.hpp"
#include "components/healthcomponent.hpp"
#include "components/inputcomponent.hpp"
#include "components/playercomponent.hpp"
#include "components/npccomponent.hpp"

using namespace xng;

class CharacterControllerSystem : public System, public EventListener, public EntityScene::Listener {
public:
    explicit CharacterControllerSystem(EventBus &eventBus)
            : eventBus(eventBus) {
        eventBus.addListener(*this);
    }

    virtual ~CharacterControllerSystem() {
        eventBus.removeListener(*this);
    }

    void start(EntityScene &scene) override {
        scene.addListener(*this);
    }

    void stop(EntityScene &scene) override {
        scene.removeListener(*this);
    }

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        for (auto &ent: damageEnts) {
            if (scene.checkComponent<CharacterControllerComponent>(ent)) {
                auto comp = scene.getComponent<CharacterControllerComponent>(ent);
                comp.damageTimer = 0.2f;
                scene.updateComponent(ent, comp);
            }
        }

        damageEnts.clear();

        for (auto &ev: contactEvents) {
            EntityHandle playerEnt;
            EntityHandle otherEnt;

            if (scene.checkComponent<CharacterControllerComponent>(ev.entityA)) {
                if (ev.colliderIndexA == 1) {
                    playerEnt = ev.entityA;
                    otherEnt = ev.entityB;
                }
            } else if (scene.checkComponent<CharacterControllerComponent>(ev.entityB)) {
                if (ev.colliderIndexB == 1) {
                    playerEnt = ev.entityB;
                    otherEnt = ev.entityA;
                }
            }

            if (playerEnt && otherEnt) {
                auto pl = scene.getComponent<CharacterControllerComponent>(playerEnt);
                if (ev.type == xng::ContactEvent::BEGIN_CONTACT) {
                    pl.collidingEntities.insert(otherEnt);
                } else {
                    pl.collidingEntities.erase(otherEnt);
                }
                scene.updateComponent(playerEnt, pl);
            }
        }

        contactEvents.clear();

        std::map<EntityHandle, CharacterControllerComponent> characterUpdates;
        for (auto &pair: scene.getPool<CharacterControllerComponent>()) {
            auto &tcomp = scene.getComponent<TransformComponent>(pair.first);
            auto &rt = scene.getComponent<CanvasTransformComponent>(pair.first);
            auto rb = scene.getComponent<RigidBodyComponent>(pair.first);
            auto anim = scene.getComponent<SpriteAnimationComponent>(pair.first);
            auto sprite = scene.getComponent<SpriteComponent>(pair.first);
            auto health = scene.getComponent<HealthComponent>(pair.first);
            auto input = scene.getComponent<InputComponent>(pair.first);
            auto character = pair.second;

            auto &canvas = scene.getComponent<CanvasComponent>(scene.getEntityByName(rt.canvas));

            character.isOnFloor = false;
            for (auto &ent: character.collidingEntities) {
                if (scene.checkComponent<FloorComponent>(ent)) {
                    character.isOnFloor = true;
                    break;
                }
            }

            // Apply movement
            if (character.isOnFloor
                && health.health > 0) {
                if (input.movement.x != 0) {
                    auto maxVel = character.maxVelocity;
                    if ((input.movement.x < 0) || (input.movement.x > 0)) {
                        rb.velocity.x += input.movement.x * character.acceleration;
                        if (rb.velocity.x < -maxVel)
                            rb.velocity.x = -maxVel;
                        else if (rb.velocity.x > maxVel)
                            rb.velocity.x = maxVel;
                    }
                } else {
                    if (rb.velocity.x < -character.drag) {
                        rb.velocity.x += character.drag;
                    } else if (rb.velocity.x > character.drag) {
                        rb.velocity.x -= character.drag;
                    }
                }

                // Apply jumping
                if (input.movement.y > 0) {
                    rb.impulse = Vec3f(0, rb.mass * 2 * input.movement.y, 0);
                    rb.impulsePoint = tcomp.transform.getPosition();
                }
            }

            bool isFalling = (rb.velocity.y > character.fallVelocity || rb.velocity.y < -character.fallVelocity)
                             && !character.isOnFloor;

            // Apply animation
            if (health.health <= 0
                && character.deathAnimation.assigned()) {
                anim.animation = character.deathAnimation;
            } else if (isFalling
                       && character.fallAnimation.assigned()) {
                anim.animation = character.fallAnimation;
            } else if (character.runAnimation.assigned() &&
                       (rb.velocity.x > character.runVelocity || rb.velocity.x < -character.runVelocity)) {
                anim.animation = character.runAnimation;
            } else if (character.walkAnimation.assigned() &&
                       (rb.velocity.x > character.walkVelocity || rb.velocity.x < -character.walkVelocity)) {
                anim.animation = character.walkAnimation;
            } else {
                anim.animation = character.idleAnimation;
            }

            // Apply direction
            if (rb.velocity.x != 0 && input.movement.x != 0) {
                character.facingLeft = rb.velocity.x > 0;
                sprite.flipSprite.x = character.facingLeft;
            }

            // Apply damage mix color
            if (character.damageTimer > 0) {
                sprite.mix = character.damageMix;
                sprite.mixColor = character.damageColor;
            } else {
                sprite.mix = 0;
            }

            // Update damageTimer
            if (character.damageTimer > 0) {
                character.damageTimer -= deltaTime;
            }

            scene.updateComponent(pair.first, rb);
            scene.updateComponent(pair.first, anim);
            scene.updateComponent(pair.first, sprite);

            characterUpdates[pair.first] = character;
        }

        for (auto &pair: characterUpdates) {
            scene.updateComponent(pair.first, pair.second);
        }
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(ContactEvent)) {
            contactEvents.emplace_back(event.as<ContactEvent>());
        }
    }

    void onComponentUpdate(const EntityHandle &entity,
                           const std::any &oldComponent,
                           const std::any &newComponent) override {
        if (oldComponent.type() == typeid(HealthComponent)) {
            auto oldHealth = std::any_cast<HealthComponent>(oldComponent);
            auto newHealth = std::any_cast<HealthComponent>(newComponent);
            if (newHealth.health < oldHealth.health) {
                damageEnts.insert(entity);
            }
        }
    }

private:
    EventBus &eventBus;

    std::vector<ContactEvent> contactEvents;

    std::set<EntityHandle> damageEnts;
};

#endif //FOXTROT_CHARACTERCONTROLLERSYSTEM_HPP

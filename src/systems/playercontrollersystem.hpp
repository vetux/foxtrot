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
#include "components/floorcomponent.hpp"
#include "components/muzzleflashcomponent.hpp"

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
                if (ev.colliderIndexA == 1) {
                    playerEnt = ev.entityA;
                    otherEnt = ev.entityB;
                }
            } else if (scene.check<PlayerControllerComponent>(ev.entityB)) {
                if (ev.colliderIndexB == 1) {
                    playerEnt = ev.entityB;
                    otherEnt = ev.entityA;
                }
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

        auto mousePos = input.getMouse().position;
        auto inp = getInput();
        for (auto pair: scene.getPool<PlayerControllerComponent>()) {
            auto &tcomp = scene.lookup<TransformComponent>(pair.first);
            auto &rt = scene.lookup<RectTransformComponent>(pair.first);
            auto rb = scene.lookup<RigidBodyComponent>(pair.first);
            auto anim = scene.lookup<SpriteAnimationComponent>(pair.first);
            auto sprite = scene.lookup<SpriteComponent>(pair.first);
            auto &player = pair.second;

            auto &canvas = scene.lookup<CanvasComponent>(scene.getEntityByName(rt.parent));

            if (weaponEntities.find(pair.first) == weaponEntities.end()) {
                createWeaponEntity(pair.first, scene);
            }

            bool isOnFloor = false;
            for (auto &ent: player.collidingEntities) {
                if (scene.check<FloorComponent>(ent)) {
                    isOnFloor = true;
                    break;
                }
            }

            // Apply movement
            if (inp.x != 0) {
                auto maxVel = maxVelocity;
                if (player.player.getEquippedWeapon() != Weapon::NONE) {
                    maxVel = maxVel * player.player.getWeapon().weight();
                }
                if ((inp.x < 0 && rb.velocity.x > -maxVel)
                    || (inp.x > 0 && rb.velocity.x < maxVel)) {
                    rb.velocity.x += inp.x * acceleration;
                    if (rb.velocity.x < -maxVel)
                        rb.velocity.x = -maxVel;
                    else if (rb.velocity.x > maxVel)
                        rb.velocity.x = maxVel;
                }
            } else {
                if (rb.velocity.x < -drag) {
                    rb.velocity.x += drag;
                } else if (rb.velocity.x > drag) {
                    rb.velocity.x -= drag;
                }
            }

            // Apply jumping
            if (inp.y > 0) {
                if (isOnFloor) {
                    rb.impulse = Vec3f(0, rb.mass, 0);
                    rb.impulsePoint = tcomp.transform.getPosition();
                }
            }

            // Apply animation
            if (player.player.getEquippedWeapon() != Weapon::NONE || (rb.velocity.x < 0.1 && rb.velocity.x > -0.1)) {
                anim.animation = pair.second.idleAnimation;
            } else {
                anim.animation = pair.second.walkAnimation;
            }

            // Apply direction
            if (rb.velocity.x != 0 && inp.x != 0) {
                player.facingLeft = rb.velocity.x > 0;
                sprite.flipSprite.x = player.facingLeft;
            }

            if (input.getKeyboard().getKeyDown(xng::KEY_1)) {
                pair.second.player.setEquippedWeapon(Weapon::NONE);
            } else if (input.getKeyboard().getKeyDown(xng::KEY_2)) {
                pair.second.player.setEquippedWeapon(Weapon::PISTOL);
            } else if (input.getKeyboard().getKeyDown(xng::KEY_3)) {
                pair.second.player.setEquippedWeapon(Weapon::GATLING);
            }

            scene.updateComponent(pair.first, pair.second);

            auto weaponEnt = weaponEntities.at(pair.first);
            auto weaponSprite = weaponEnt.getComponent<SpriteComponent>();
            auto weaponTransform = weaponEnt.getComponent<TransformComponent>();
            auto weaponRect = weaponEnt.getComponent<RectTransformComponent>();

            auto visuals = pair.second.player.getWeapon().getVisuals();

            weaponSprite.layer = -1;
            weaponSprite.sprite = visuals.sprite;
            weaponTransform.transform.setPosition(
                    {player.facingLeft
                     ? tcomp.transform.getPosition().x - visuals.offset.x
                     : tcomp.transform.getPosition().x + visuals.offset.x,
                     tcomp.transform.getPosition().y + visuals.offset.y,
                     0});
            weaponRect.parent = "MainCanvas";
            weaponRect.rect.dimensions = visuals.size;
            weaponRect.center = visuals.center;
            if (player.facingLeft) {
                weaponRect.center.x = weaponRect.rect.dimensions.x - weaponRect.center.x;
            }
            weaponSprite.flipSprite.x = player.facingLeft;

            auto dir = mousePos.convert<float>() -
                       Vec2f(-weaponTransform.transform.getPosition().x - canvas.cameraPosition.x,
                             -weaponTransform.transform.getPosition().y - canvas.cameraPosition.y);

            auto angle = numeric_cast<float>(getAngle(dir));

            auto bounds = player.player.getWeapon().getAngleBounds();

            if (player.facingLeft) {
                if (angle < 0)
                    angle = std::clamp(angle + 360, bounds.x + 180, bounds.y + 180);
                else
                    angle = std::clamp(angle, bounds.x + 180, bounds.y + 180);
                weaponRect.rotation = angle + 180;
            } else {
                angle = std::clamp(angle, bounds.x, bounds.y);
                weaponRect.rotation = angle;
            }

            weaponEnt.updateComponent(weaponSprite);
            weaponEnt.updateComponent(weaponTransform);
            weaponEnt.updateComponent(weaponRect);

            scene.updateComponent(pair.first, rb);
            scene.updateComponent(pair.first, anim);
            scene.updateComponent(pair.first, sprite);
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

    Entity &createWeaponEntity(EntityHandle targetPlayer, EntityScene &scene) {
        auto it = weaponEntities.find(targetPlayer);
        if (it != weaponEntities.end())
            scene.destroyEntity(it->second);

        auto ent = scene.createEntity();
        TransformComponent transform;
        RectTransformComponent rect;
        SpriteComponent sprite;

        ent.createComponent(transform);
        ent.createComponent(rect);
        ent.createComponent(sprite);

        weaponEntities[targetPlayer] = ent;

        return weaponEntities[targetPlayer];
    }

    Input &input;
    EventBus &eventBus;

    std::vector<ContactEvent> contactEvents;

    std::map<EntityHandle, Entity> weaponEntities;
    std::map<EntityHandle, Entity> muzzleFlashEntities;

    const float maxVelocity = 10;
    const float acceleration = 5;
    const float drag = 0.2f;
};

#endif //FOXTROT_PLAYERCONTROLLERSYSTEM_HPP

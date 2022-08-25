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

    void start(EntityScene &scene) override {
        Entity ent = scene.createEntity();
        auto rt = RectTransformComponent();
        rt.center = Vec2f(32, 32);
        rt.rect.dimensions = Vec2f(64, 64);
        rt.parent = "GuiCanvas";
        ent.createComponent(rt);
        auto sprite = SpriteComponent();
        sprite.sprite = ResourceHandle<Sprite>(Uri("file://sprites/crosshair_duplex.json"));
        sprite.layer = 10;
        ent.createComponent(sprite);
        crossHairEntity = ent.getHandle();
    }

    void stop(EntityScene &scene) override {
        scene.destroy(crossHairEntity);
    }

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        std::set<EntityHandle> delHandles;
        for (auto &pair: scene.getPool<MuzzleFlashComponent>()) {
            auto &anim = scene.lookup<SpriteAnimationComponent>(pair.first);
            if (anim.finished) {
                delHandles.insert(pair.first);
            }
        }
        for (auto &ent: delHandles) {
            scene.destroy(ent);
        }
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

        bool isAiming = false;

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
                auto maxVel = player.maxVelocity;
                if (player.player.getEquippedWeapon() != Weapon::NONE) {
                    maxVel = maxVel * player.player.getWeapon().weight();
                }
                if ((inp.x < 0 && rb.velocity.x > -maxVel)
                    || (inp.x > 0 && rb.velocity.x < maxVel)) {
                    rb.velocity.x += inp.x * player.acceleration;
                    if (rb.velocity.x < -maxVel)
                        rb.velocity.x = -maxVel;
                    else if (rb.velocity.x > maxVel)
                        rb.velocity.x = maxVel;
                }
            } else {
                if (rb.velocity.x < -player.drag) {
                    rb.velocity.x += player.drag;
                } else if (rb.velocity.x > player.drag) {
                    rb.velocity.x -= player.drag;
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

            player.player.update(deltaTime);

            // Apply Input
            if (input.getKeyboard().getKeyDown(xng::KEY_1)) {
                pair.second.player.setEquippedWeapon(Weapon::NONE);
            } else if (input.getKeyboard().getKeyDown(xng::KEY_2)) {
                pair.second.player.setEquippedWeapon(Weapon::PISTOL);
            } else if (input.getKeyboard().getKeyDown(xng::KEY_3)) {
                pair.second.player.setEquippedWeapon(Weapon::GATLING);
            }

            if (input.getKeyboard().getKeyDown(KEY_T)) {
                player.isAiming = !player.isAiming;
            }

            bool shoot = false;

            if (input.getMouse().getButton(xng::LEFT)
                || input.getKeyboard().getKey(xng::KEY_SPACE)) {
                shoot = player.player.getWeapon().shoot();
            }

            if (input.getKeyboard().getKeyDown(KEY_R)) {
                player.player.getWeapon().setAmmo(player.player.getWeapon().getAmmo() + 10);
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

            if (!player.isAiming) {
                weaponRect.rotation = 0;
            }

            if (shoot) {
                auto muzzleEnt = createMuzzleEntity(pair.first, scene);

                auto muzzleSprite = muzzleEnt.getComponent<SpriteComponent>();
                auto muzzleTransform = muzzleEnt.getComponent<TransformComponent>();
                auto muzzleRect = muzzleEnt.getComponent<RectTransformComponent>();
                auto muzzleAnim = muzzleEnt.getComponent<SpriteAnimationComponent>();

                muzzleAnim.animation = visuals.muzzleFlash;
                muzzleAnim.enabled = true;

                muzzleRect.rect.dimensions = visuals.muzzleSize;

                auto flash = visuals.muzzleFlash.get();
                muzzleSprite.sprite = flash.getFrame();
                muzzleSprite.layer = -1;

                if (player.isAiming) {
                    if (player.facingLeft) {
                        visuals.muzzleOffset.y = -visuals.muzzleOffset.y;
                    }
                    auto shootDir = rotateVectorAroundPoint(visuals.muzzleOffset, {}, angle);
                    muzzleTransform.transform.setPosition(
                            weaponTransform.transform.getPosition() + Vec3f(shootDir.x, shootDir.y, 0));
                } else {
                    if (player.facingLeft) {
                        visuals.muzzleOffset.x = -visuals.muzzleOffset.x;
                    }
                    muzzleTransform.transform.setPosition(weaponTransform.transform.getPosition() +
                                                          Vec3f(visuals.muzzleOffset.x,
                                                                visuals.muzzleOffset.y, 0));
                }

                muzzleRect.parent = "MainCanvas";
                muzzleRect.rect.dimensions = visuals.muzzleSize;
                muzzleRect.center = visuals.muzzleCenter;
                if (player.isAiming) {
                    muzzleRect.rotation = angle;
                } else {
                    muzzleRect.rotation = player.facingLeft ? 180 : 0;
                }

                muzzleEnt.updateComponent(muzzleSprite);
                muzzleEnt.updateComponent(muzzleTransform);
                muzzleEnt.updateComponent(muzzleRect);
                muzzleEnt.updateComponent(muzzleAnim);
            }

            weaponEnt.updateComponent(weaponSprite);
            weaponEnt.updateComponent(weaponTransform);
            weaponEnt.updateComponent(weaponRect);

            scene.updateComponent(pair.first, rb);
            scene.updateComponent(pair.first, anim);
            scene.updateComponent(pair.first, sprite);

            isAiming = player.isAiming;
        }

        auto rt = scene.lookup<RectTransformComponent>(crossHairEntity);
        rt.enabled = isAiming;
        rt.rect.position = input.getMouse().position.convert<float>();
        scene.updateComponent(crossHairEntity, rt);

        input.setMouseCursorHidden(isAiming);
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

    Entity createMuzzleEntity(EntityHandle targetPlayer, EntityScene &scene) {
        auto ent = scene.createEntity();

        TransformComponent transform;
        RectTransformComponent rect;
        SpriteComponent sprite;
        MuzzleFlashComponent muzzleFlashComponent;
        SpriteAnimationComponent animationComponent;

        sprite.sprite = {};
        animationComponent.enabled = false;

        ent = scene.createEntity();

        ent.createComponent(transform);
        ent.createComponent(rect);
        ent.createComponent(sprite);
        ent.createComponent(animationComponent);
        ent.createComponent(muzzleFlashComponent);

        muzzleFlashEntities[targetPlayer].emplace_back(ent);

        return ent;
    }

    Input &input;
    EventBus &eventBus;

    std::vector<ContactEvent> contactEvents;

    std::map<EntityHandle, Entity> weaponEntities;
    std::map<EntityHandle, std::vector<Entity>> muzzleFlashEntities;

    EntityHandle crossHairEntity;
};

#endif //FOXTROT_PLAYERCONTROLLERSYSTEM_HPP

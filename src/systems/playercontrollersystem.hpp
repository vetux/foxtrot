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

#include "components/muzzleflashcomponent.hpp"
#include "components/charactercontrollercomponent.hpp"

#include "bullets/smallbullet.hpp"

using namespace xng;

class PlayerControllerSystem : public System {
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

        bool isAiming = false;
        Vec2f aimPosition = {};

        std::map<EntityHandle, PlayerComponent> playerUpdates;
        for (auto &pair: scene.getPool<PlayerComponent>()) {
            auto &tcomp = scene.lookup<TransformComponent>(pair.first);
            auto &rt = scene.lookup<RectTransformComponent>(pair.first);
            auto &rb = scene.lookup<RigidBodyComponent>(pair.first);
            auto &anim = scene.lookup<SpriteAnimationComponent>(pair.first);
            auto &sprite = scene.lookup<SpriteComponent>(pair.first);
            auto &health = scene.lookup<HealthComponent>(pair.first);
            auto character = scene.lookup<CharacterControllerComponent>(pair.first);
            auto &input = scene.lookup<InputComponent>(pair.first);
            auto player = pair.second;

            auto &canvas = scene.lookup<CanvasComponent>(scene.getEntityByName(rt.parent));

            if (weaponEntities.find(pair.first) == weaponEntities.end()) {
                createWeaponEntity(pair.first, scene);
            }

            player.player.setEquippedWeapon(input.weapon);
            player.player.update(deltaTime);

            bool shoot = false;

            if (input.fire) {
                shoot = player.player.getWeapon().shoot();
            }

            if (input.reload) {
                player.player.getWeapon().setAmmo(player.player.getWeapon().getAmmo() + 10);
            }

            auto weaponEnt = weaponEntities.at(pair.first);
            auto weaponSprite = weaponEnt.getComponent<SpriteComponent>();
            auto weaponTransform = weaponEnt.getComponent<TransformComponent>();
            auto weaponRect = weaponEnt.getComponent<RectTransformComponent>();

            auto visuals = player.player.getWeapon().getVisuals();

            weaponSprite.layer = -1;
            weaponSprite.sprite = visuals.sprite;
            weaponTransform.transform.setPosition(
                    {character.facingLeft
                     ? tcomp.transform.getPosition().x - visuals.offset.x
                     : tcomp.transform.getPosition().x + visuals.offset.x,
                     tcomp.transform.getPosition().y + visuals.offset.y,
                     0});
            weaponRect.parent = "MainCanvas";
            weaponRect.rect.dimensions = visuals.size;
            weaponRect.center = visuals.center;
            if (character.facingLeft) {
                weaponRect.center.x = weaponRect.rect.dimensions.x - weaponRect.center.x;
            }
            weaponSprite.flipSprite.x = character.facingLeft;

            auto dir = input.aimPosition.convert<float>() -
                       Vec2f(-weaponTransform.transform.getPosition().x - canvas.cameraPosition.x,
                             -weaponTransform.transform.getPosition().y - canvas.cameraPosition.y);

            auto angle = numeric_cast<float>(getAngle(dir));

            auto bounds = player.player.getWeapon().getAngleBounds();

            if (character.facingLeft) {
                if (angle < 0)
                    angle = std::clamp(angle + 360, bounds.x + 180, bounds.y + 180);
                else
                    angle = std::clamp(angle, bounds.x + 180, bounds.y + 180);
                weaponRect.rotation = angle + 180;
            } else {
                angle = std::clamp(angle, bounds.x, bounds.y);
                weaponRect.rotation = angle;
            }

            if (!input.aim) {
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

                if (input.aim) {
                    if (character.facingLeft) {
                        visuals.muzzleOffset.y = -visuals.muzzleOffset.y;
                    }
                    auto shootDir = rotateVectorAroundPoint(visuals.muzzleOffset, {}, angle);
                    muzzleTransform.transform.setPosition(
                            weaponTransform.transform.getPosition() + Vec3f(shootDir.x, shootDir.y, 0));
                } else {
                    if (character.facingLeft) {
                        visuals.muzzleOffset.x = -visuals.muzzleOffset.x;
                    }
                    muzzleTransform.transform.setPosition(weaponTransform.transform.getPosition() +
                                                          Vec3f(visuals.muzzleOffset.x,
                                                                visuals.muzzleOffset.y, 0));
                }

                muzzleRect.parent = "MainCanvas";
                muzzleRect.rect.dimensions = visuals.muzzleSize;
                muzzleRect.center = visuals.muzzleCenter;
                if (input.aim) {
                    muzzleRect.rotation = angle;
                } else {
                    muzzleRect.rotation = character.facingLeft ? 180 : 0;
                }

                muzzleEnt.updateComponent(muzzleSprite);
                muzzleEnt.updateComponent(muzzleTransform);
                muzzleEnt.updateComponent(muzzleRect);
                muzzleEnt.updateComponent(muzzleAnim);


                auto shootDir = normalize(rotateVectorAroundPoint(Vec2f(-1, 0), {}, muzzleRect.rotation));

                auto rotation = Vec3f(0, 0, muzzleRect.rotation);
                SmallBullet::create(scene,
                                    Transform(muzzleTransform.transform.getPosition(),
                                              rotation, Vec3f(1)),
                                    Vec3f(shootDir.x, shootDir.y, 0) * 100,
                                    "MainCanvas");
            }

            switch (player.player.getEquippedWeapon()){
                case Weapon::PISTOL:
                    character.idleAnimation = player.idleAnimation;
                    character.walkAnimation = player.walkAnimation;
                    break;
                case Weapon::NONE:
                case Weapon::GATLING:
                    character.idleAnimation = player.idleAnimationLow;
                    character.walkAnimation = player.walkAnimationLow;
                    break;
            }

            weaponEnt.updateComponent(weaponSprite);
            weaponEnt.updateComponent(weaponTransform);
            weaponEnt.updateComponent(weaponRect);

            scene.updateComponent(pair.first, character);

            playerUpdates[pair.first] = player;

            isAiming = input.aim;
            aimPosition = input.aimPosition;
        }

        for (auto &pair: playerUpdates) {
            scene.updateComponent(pair.first, pair.second);
        }

        auto rt = scene.lookup<RectTransformComponent>(crossHairEntity);
        rt.enabled = isAiming;
        rt.rect.position = aimPosition;
        scene.updateComponent(crossHairEntity, rt);
    }

private:
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

    std::map<EntityHandle, Entity> weaponEntities;
    std::map<EntityHandle, std::vector<Entity>> muzzleFlashEntities;

    EntityHandle crossHairEntity;
};

#endif //FOXTROT_PLAYERCONTROLLERSYSTEM_HPP

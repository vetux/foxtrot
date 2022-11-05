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

#include <random>

#include "xng/xng.hpp"

#include "components/muzzleflashcomponent.hpp"
#include "components/charactercontrollercomponent.hpp"

#include "bullets/smallbullet.hpp"

using namespace xng;

class PlayerControllerSystem : public System {
public:
    PlayerControllerSystem()
            : rng(dev()) {}

    void start(EntityScene &scene, EventBus &eventBus) override {}

    void stop(EntityScene &scene, EventBus &eventBus) override {
        for (auto &ent: weaponEntities) {
            scene.destroy(ent.first);
        }
        for (auto &muzzle: muzzleFlashEntities) {
            for (auto &muz: muzzle.second) {
                scene.destroyEntity(muz);
            }
        }
        weaponEntities.clear();
        muzzleFlashEntities.clear();
    }

private:
    void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override {
        std::set<EntityHandle> delHandles;
        for (auto &pair: scene.getPool<MuzzleFlashComponent>()) {
            auto &anim = scene.getComponent<SpriteAnimationComponent>(pair.first);
            if (anim.finished) {
                delHandles.insert(pair.first);
            }
        }
        for (auto &ent: delHandles) {
            scene.destroy(ent);
        }

        std::map<EntityHandle, PlayerComponent> playerUpdates;
        for (auto &pair: scene.getPool<PlayerComponent>()) {
            auto &tcomp = scene.getComponent<TransformComponent>(pair.first);
            auto &rt = scene.getComponent<CanvasTransformComponent>(pair.first);
            auto &rb = scene.getComponent<RigidBodyComponent>(pair.first);
            auto anim = scene.getComponent<SpriteAnimationComponent>(pair.first);
            auto &sprite = scene.getComponent<SpriteComponent>(pair.first);
            auto &health = scene.getComponent<HealthComponent>(pair.first);
            auto character = scene.getComponent<CharacterControllerComponent>(pair.first);
            auto &input = scene.getComponent<InputComponent>(pair.first);
            auto player = pair.second;

            auto &canvas = scene.getComponent<CanvasComponent>(scene.getEntityByName(rt.canvas));

            if (weaponEntities.find(pair.first) == weaponEntities.end()) {
                createWeaponEntity(pair.first, scene);
            }

            bool isFalling = (rb.velocity.y > character.fallVelocity || rb.velocity.y < -character.fallVelocity)
                             && !character.isOnFloor;

            bool isDead = health.health <= 0;

            player.player.setIsFalling(isFalling);
            player.player.setEquippedWeapon(input.weapon);
            player.player.setPose(input.pose);
            player.player.update(deltaTime);

            character.idleAnimation = player.player.getIdleAnimation();
            character.walkAnimation = player.player.getWalkAnimation();
            character.runAnimation = player.player.getRunAnimation();
            // character.fallAnimation = player.player.getFallAnimation();
            character.deathAnimation = player.player.getDeathAnimation();

            bool shoot = false;

            if (input.fire) {
                player.player.getWeapon().pullTrigger(deltaTime);
            } else {
                player.player.getWeapon().releaseTrigger(deltaTime);
            }

            if (input.fireHold && !isDead) {
                shoot = player.player.getWeapon().shoot(deltaTime);
            }

            if (input.reload && !isDead) {
                player.player.getWeapon().setAmmo(player.player.getWeapon().getAmmo() + 10);
                player.player.getWeapon().reload(deltaTime);
            }

            auto weaponEnt = weaponEntities.at(pair.first);
            auto weaponSprite = weaponEnt.getComponent<SpriteComponent>();
            auto weaponTransform = weaponEnt.getComponent<TransformComponent>();
            auto weaponRect = weaponEnt.getComponent<CanvasTransformComponent>();

            auto visuals = player.player.getWeapon().getVisuals();

            auto offset = visuals.offset + player.player.getWeaponOffset();
            if (character.facingLeft)
                offset.x *= -1;

            weaponSprite.layer = -1;
            weaponSprite.sprite = visuals.sprite;
            weaponTransform.transform.setPosition(
                    {offset.x,
                     offset.y,
                     0});
            weaponRect.canvas = "MainCanvas";
            weaponRect.rect.dimensions = visuals.size;
            weaponRect.center = visuals.center;
            if (character.facingLeft) {
                weaponRect.center.x = weaponRect.rect.dimensions.x - weaponRect.center.x;
            }
            weaponSprite.flipSprite.x = character.facingLeft;

            auto weaponWorld = TransformComponent::walkHierarchy(weaponTransform, scene);
            auto dir = input.aimPosition.convert<float>() -
                       Vec2f(-weaponWorld.getPosition().x - canvas.cameraPosition.x,
                             -weaponWorld.getPosition().y - canvas.cameraPosition.y);

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
                createSoundEffectEntity(scene.getEntityName(pair.first), scene, Uri("/sound/effects/gunshot_0.wav"));
                auto muzzleEnt = createMuzzleEntity(pair.first, scene);

                auto muzzleSprite = muzzleEnt.getComponent<SpriteComponent>();
                auto muzzleTransform = muzzleEnt.getComponent<TransformComponent>();
                auto muzzleRect = muzzleEnt.getComponent<CanvasTransformComponent>();
                auto muzzleAnim = muzzleEnt.getComponent<SpriteAnimationComponent>();

                muzzleAnim.animation = visuals.muzzleFlash;
                muzzleAnim.enabled = true;

                muzzleRect.rect.dimensions = visuals.muzzleSize;

                auto flash = visuals.muzzleFlash.get();
                muzzleSprite.sprite = flash.getFrame();
                muzzleSprite.layer = -1;

                muzzleRect.canvas = "MainCanvas";
                muzzleRect.rect.dimensions = visuals.muzzleSize;
                muzzleRect.center = visuals.muzzleCenter;
                if (input.aim) {
                    muzzleRect.rotation = angle;
                } else {
                    muzzleRect.rotation = character.facingLeft ? 180 : 0;
                }

                if (character.facingLeft) {
                    visuals.muzzleOffset.y = -visuals.muzzleOffset.y;
                }

                auto vec = rotateVectorAroundPoint(Vec2f(weaponWorld.getPosition().x, weaponWorld.getPosition().y) +
                                                   visuals.muzzleOffset,
                                                   Vec2f(weaponWorld.getPosition().x, weaponWorld.getPosition().y),
                                                   muzzleRect.rotation);
                muzzleTransform.transform.setPosition({vec.x, vec.y, 0});

                muzzleEnt.updateComponent(muzzleSprite);
                muzzleEnt.updateComponent(muzzleTransform);
                muzzleEnt.updateComponent(muzzleRect);
                muzzleEnt.updateComponent(muzzleAnim);

                auto aimDir = normalize(rotateVectorAroundPoint(Vec2f(-1, 0), {}, muzzleRect.rotation));

                std::uniform_int_distribution<std::mt19937::result_type> distribution(0, 1000);

                float v = 1.0f * (((float) distribution(rng) / 1000.0f) - 0.5f);

                auto rotation = Vec3f(0, 0, muzzleRect.rotation);
                auto muzzleWorld = TransformComponent::walkHierarchy(muzzleTransform, scene);
                float spreadAngle = player.player.getWeapon().getBulletSpread() * v;
                auto velocity = rotateVectorAroundPoint(aimDir, {}, spreadAngle);
                SmallBullet::create(scene,
                                    Transform(muzzleWorld.getPosition(),
                                              rotation + muzzleWorld.getRotation().getEulerAngles(),
                                              Vec3f(1) + muzzleWorld.getScale()),
                                    Vec3f(velocity.x, velocity.y, 0) * player.player.getWeapon().getBulletSpeed(),
                                    "MainCanvas");
            }

            weaponRect.enabled = !isDead;

            weaponEnt.updateComponent(weaponSprite);
            weaponEnt.updateComponent(weaponTransform);
            weaponEnt.updateComponent(weaponRect);

            character.maxVelocity = player.player.getMaxVelocity() * (1 - player.player.getWeapon().weight());

            if (anim.animation.assigned()) {
                if (rb.velocity.x >= 0) {
                    anim.animationDurationOverride = anim.animation.get().getDuration() +
                                                     (anim.animation.get().getDuration() -
                                                      (anim.animation.get().getDuration() *
                                                       (rb.velocity.x / player.player.getMaxVelocity())));
                } else {
                    anim.animationDurationOverride = anim.animation.get().getDuration() +
                                                     (anim.animation.get().getDuration() -
                                                      (anim.animation.get().getDuration() *
                                                       (-rb.velocity.x / player.player.getMaxVelocity())));
                }
            } else {
                anim.animationDurationOverride = 0;
            }

            scene.updateComponent(pair.first, character);
            scene.updateComponent(pair.first, anim);

            playerUpdates[pair.first] = player;
        }

        for (auto &pair: playerUpdates) {
            scene.updateComponent(pair.first, pair.second);
        }

        delHandles.clear();
        for (auto &pair: sfxStarts) {
            if (std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::high_resolution_clock::now() - pair.second)
                > std::chrono::seconds(2)) {
                delHandles.insert(pair.first);
            }
        }
        for (auto &handle: delHandles) {
            scene.destroyEntity(sfxEntities.at(handle));
            sfxEntities.erase(handle);
            sfxStarts.erase(handle);
        }
    }

private:
    Entity &createWeaponEntity(EntityHandle targetPlayer, EntityScene &scene) {
        auto it = weaponEntities.find(targetPlayer);
        if (it != weaponEntities.end())
            scene.destroyEntity(it->second);

        auto ent = scene.createEntity();
        TransformComponent transform;
        CanvasTransformComponent rect;
        SpriteComponent sprite;

        transform.parent = scene.getEntityName(targetPlayer);

        ent.createComponent(transform);
        ent.createComponent(rect);
        ent.createComponent(sprite);

        weaponEntities[targetPlayer] = ent;
        return weaponEntities[targetPlayer];
    }

    Entity createMuzzleEntity(EntityHandle targetPlayer, EntityScene &scene) {
        auto ent = scene.createEntity();

        TransformComponent transform;
        CanvasTransformComponent rect;
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

    Entity createSoundEffectEntity(const std::string &transformParent, EntityScene &scene, const Uri &uri) {
        auto ret = scene.createEntity();
        auto tr = TransformComponent();
        tr.parent = transformParent;
        ret.createComponent(tr);
        auto snd = AudioSourceComponent();
        snd.play = true;
        snd.audio = ResourceHandle<Audio>(uri);
        ret.createComponent(snd);
        sfxEntities[ret.getHandle()] = ret;
        sfxStarts[ret.getHandle()] = std::chrono::high_resolution_clock::now();
        return ret;
    }

    std::map<EntityHandle, Entity> weaponEntities;
    std::map<EntityHandle, std::vector<Entity>> muzzleFlashEntities;
    std::map<EntityHandle, Entity> sfxEntities;
    std::map<EntityHandle, std::chrono::high_resolution_clock::time_point> sfxStarts;

    std::random_device dev;
    std::mt19937 rng;

};

#endif //FOXTROT_PLAYERCONTROLLERSYSTEM_HPP

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

#ifndef FOXTROT_BULLETSYSTEM_HPP
#define FOXTROT_BULLETSYSTEM_HPP

#include "xng/xng.hpp"

#include "components/bulletcomponent.hpp"
#include "components/healthcomponent.hpp"

using namespace xng;

class BulletSystem : public System, EventListener {
public:
    explicit BulletSystem() {
    }

    ~BulletSystem() override {
    }

    void start(EntityScene &scene, EventBus &eventBus) override {
        eventBus.addListener(*this);
    }

    void stop(EntityScene &scene, EventBus &eventBus) override {
        eventBus.removeListener(*this);
    }

    void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override {
        for (auto &ev: contactEvents) {
            if (ev.type == xng::ContactEvent::BEGIN_CONTACT) {
                EntityHandle bullet;
                EntityHandle health;
                if (scene.checkComponent<BulletComponent>(ev.entityA))
                    bullet = ev.entityA;
                else if (scene.checkComponent<BulletComponent>(ev.entityB))
                    bullet = ev.entityB;
                else
                    continue;
                EntityHandle other;
                if (bullet == ev.entityA) {
                    other = ev.entityB;
                } else {
                    other = ev.entityA;
                }
                if (other) {
                    if (scene.checkComponent<HealthComponent>(other))
                        health = other;
                    else if (scene.checkComponent<BulletComponent>(other))
                        continue;
                }
                if (bullet && health) {
                    auto bulletComp = scene.getComponent<BulletComponent>(bullet);
                    auto healthComp = scene.getComponent<HealthComponent>(health);
                    healthComp.health -= bulletComp.damage;
                    bulletComp.destroy = true;
                    scene.updateComponent(health, healthComp);
                    scene.updateComponent(bullet, bulletComp);
                } else if (bullet) {
                    auto bulletComp = scene.getComponent<BulletComponent>(bullet);
                    bulletComp.destroy = true;
                    scene.updateComponent(bullet, bulletComp);
                }
            }
        }

        contactEvents.clear();

        std::set<EntityHandle> destroyEnts;
        for (auto &pair: scene.getPool<BulletComponent>()) {
            if (pair.second.destroy) {
                if (pair.second.destroyAnimation.assigned()) {
                    auto sprite = scene.getComponent<SpriteAnimationComponent>(pair.first);
                    if (sprite.animation != pair.second.destroyAnimation) {
                        sprite.animation = pair.second.destroyAnimation;
                        scene.updateComponent(pair.first, sprite);
                    } else {
                        if (sprite.finished) {
                            destroyEnts.insert(pair.first);
                        }
                    }
                } else {
                    destroyEnts.insert(pair.first);
                }
            }
        }
        for (auto &ent: destroyEnts) {
            scene.destroy(ent);
        }
    }

private:
    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(ContactEvent)) {
            contactEvents.emplace_back(event.as<ContactEvent>());
        }
    }

    std::vector<ContactEvent> contactEvents;
};

#endif //FOXTROT_BULLETSYSTEM_HPP

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

#ifndef FOXTROT_SMALLBULLET_HPP
#define FOXTROT_SMALLBULLET_HPP

#include "xengine.hpp"

#include "components/bulletcomponent.hpp"

using namespace xng;

namespace SmallBullet {
    static const std::string animUri = "animations/bullet_small.json";

    Entity create(EntityScene &scene,
                  const Transform &transform,
                  const Vec3f &velocity,
                  const std::string &parent,
                  float damage = 10) {
        auto ent = scene.createEntity();
        auto t = TransformComponent();
        t.transform = transform;
        ent.createComponent(t);
        auto rt = RectTransformComponent();
        rt.rect.dimensions = Vec2f(16, 16);
        rt.center = Vec2f(8, 8);
        rt.parent = parent;
        ent.createComponent(rt);
        auto rb = RigidBodyComponent();
        auto col = ColliderDesc();
        col.isSensor = false;
        col.shape.vertices.emplace_back(Vec3f(-4, -4, 0));
        col.shape.vertices.emplace_back(Vec3f(4, -4, 0));
        col.shape.vertices.emplace_back(Vec3f(4, 4, 0));
        col.shape.vertices.emplace_back(Vec3f(-4, 4, 0));
        col.shape.primitive = xng::QUAD;
        rb.colliders.emplace_back(col);
        rb.type = RigidBody::DYNAMIC;
        rb.velocity = velocity;
        rb.is2D = true;
        rb.mass = 1;
        ent.createComponent(rb);
        auto sprite = SpriteComponent();
        sprite.flipSprite.x = velocity.x < 0;
        sprite.layer = 5;
        ent.createComponent(sprite);
        auto anim = SpriteAnimationComponent();
        anim.animation = ResourceHandle<SpriteAnimation>(Uri(animUri));
        ent.createComponent(anim);
        auto bullet = BulletComponent();
        bullet.damage = damage;
        ent.createComponent(bullet);
        return ent;
    }
}

#endif //FOXTROT_SMALLBULLET_HPP

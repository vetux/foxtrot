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

#include "xng/xng.hpp"

#include "components/bulletcomponent.hpp"

using namespace xng;

namespace SmallBullet {
    static const std::string animUri = "animations/bullet_small.xbundle";

    static const std::string colPath = "colliders/smallbullet.xbundle";

    static bool initialized = false;

    void init() {
        if (initialized){
            return;
        }
        initialized = true;
        ColliderDesc desc;
        desc.isSensor = false;
        desc.density = 10;
        desc.shape.vertices.emplace_back(Vec3f(-4, -4, 0));
        desc.shape.vertices.emplace_back(Vec3f(4, -4, 0));
        desc.shape.vertices.emplace_back(Vec3f(4, 4, 0));
        desc.shape.vertices.emplace_back(Vec3f(-4, 4, 0));
        desc.shape.type = xng::COLLIDER_2D;
        auto msg = JsonParser::createBundle({
                                                    std::make_pair<>(std::string(),
                                                                     std::reference_wrapper<ColliderDesc>(desc))
                                            });
        std::stringstream stream;
        JsonProtocol().serialize(stream, msg);
        std::vector<uint8_t> bytes;
        auto str = stream.str();
        for (auto &c: str) {
            bytes.emplace_back(c);
        }
        ResourceRegistry::getDefaultRegistry().getArchiveT<MemoryArchive>("memory").addData(
                colPath,
                bytes);
    }

    Entity create(EntityScene &scene,
                  const Transform &transform,
                  const Vec3f &velocity,
                  const std::string &canvas,
                  float damage = 10) {
        init();

        auto ent = scene.createEntity();
        auto t = TransformComponent();
        t.transform = transform;
        ent.createComponent(t);
        auto rt = CanvasTransformComponent();
        rt.rect.dimensions = Vec2f(16, 16);
        rt.center = Vec2f(8, 8);
        rt.canvas = canvas;
        ent.createComponent(rt);
        auto rb = RigidBodyComponent();
        rb.colliders.emplace_back(Uri("memory://" + colPath));
        rb.type = RigidBody::DYNAMIC;
        rb.velocity = velocity;
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

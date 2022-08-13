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

#ifndef FOXTROT_DAYTIMESYSTEM_HPP
#define FOXTROT_DAYTIMESYSTEM_HPP

#include "xengine.hpp"

#include "components/backdropcomponent.hpp"

using namespace xng;

class DaytimeSystem : public System {
public:
    void update(DeltaTime deltaTime, EntityScene &scene) override {
        accumulatedTime += deltaTime;
        if (accumulatedTime > dayDuration + nightDuration) {
            accumulatedTime = 0;
        }
        bool isDay = accumulatedTime < dayDuration;

        for (auto &pair: scene.getPool<BackdropComponent>()) {
            auto sprite = scene.lookup<SpriteComponent>(pair.first);
            sprite.sprite = pair.second.daySprite;
            sprite.spriteB = pair.second.nightSprite;
            if (isDay) {
                if (sprite.blendScale != 0) {
                    sprite.blendScale -= duskSpeed * deltaTime;
                    if (sprite.blendScale < 0)
                        sprite.blendScale = 0;
                }
            } else {
                if (sprite.blendScale != 1){
                    sprite.blendScale += duskSpeed * deltaTime;
                    if (sprite.blendScale > 1)
                        sprite.blendScale = 1;
                }
            }
            scene.updateComponent(pair.first, sprite);
        }
    }

private:
    DeltaTime accumulatedTime = std::numeric_limits<DeltaTime>::max();
    const float dayDuration = 10;
    const float nightDuration = 15;
    const float duskSpeed = 1;
};

#endif //FOXTROT_DAYTIMESYSTEM_HPP

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

#ifndef FOXTROT_TIMESYSTEM_HPP
#define FOXTROT_TIMESYSTEM_HPP

#include "xng/xng.hpp"

#include "components/backdropcomponent.hpp"

using namespace xng;

class TimeSystem : public System {
public:
    explicit TimeSystem(double time = 0,
                        float dayDuration = 120,
                        float nightDuration = 180,
                        float duskSpeed = 1)
            : time(time),
              dayDuration(dayDuration),
              nightDuration(nightDuration),
              duskSpeed(duskSpeed) {}

    void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override {
        time += deltaTime;

        const auto totalDuration = dayDuration + nightDuration;
        auto days = static_cast<int>(time / totalDuration);
        auto timeOfDay = time - (days * totalDuration);

        bool isDay = timeOfDay < dayDuration;

        for (auto &pair: scene.getPool<BackdropComponent>()) {
            auto sprite = scene.getComponent<SpriteComponent>(pair.first);
            sprite.sprite = pair.second.daySprite;
            sprite.mixColor = ColorRGBA::black();
            if (isDay) {
                if (sprite.mix != 0) {
                    sprite.mix -= duskSpeed * deltaTime;
                    if (sprite.mix < 0)
                        sprite.mix = 0;
                }
            } else {
                if (sprite.mix != 1) {
                    sprite.mix += duskSpeed * deltaTime;
                    if (sprite.mix > 1)
                        sprite.mix = 1;
                }
            }
            scene.updateComponent(pair.first, sprite);
        }
    }

    void setTime(double value) {
        time = value;
    }

    double getTime() const {
        return time;
    }

private:
    double time;
    const float dayDuration;
    const float nightDuration;
    const float duskSpeed;
};

#endif //FOXTROT_TIMESYSTEM_HPP

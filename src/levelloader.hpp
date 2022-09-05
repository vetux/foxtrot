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

#ifndef FOXTROT_LEVELLOADER_HPP
#define FOXTROT_LEVELLOADER_HPP

#include "xengine.hpp"

using namespace xng;

class LevelLoader : Level::LoadListener {
public:
    LevelLoader(RenderTarget &target,
                Renderer2D &ren2d,
                ECS &ecs,
                EventBus &eventBus,
                Window &window,
                FontDriver &fontDriver)
            : target(target),
              ren2d(ren2d),
              ecs(ecs) {
        levels[LEVEL_MAIN_MENU] = std::make_unique<MainMenu>(eventBus,
                                                             window,
                                                             ren2d,
                                                             fontDriver);
        levels[LEVEL_ZERO] = std::make_unique<Level0>(eventBus,
                                                      window,
                                                      ren2d,
                                                      fontDriver);
    }

    ~LevelLoader() {
        if (loadedLevel != LEVEL_NULL) {
            levels.at(loadedLevel)->awaitLoad();
            levels.at(loadedLevel)->onStop(ecs);
        }
    }

    void loadLevel(LevelID id) {
        requestedLevel = id;
    }

    Level &getLevel() {
        auto &ret = *levels.at(loadedLevel);
        ret.awaitLoad();
        return ret;
    }

    void update(DeltaTime deltaTime) {
        if (loading) {
            drawLoadingScreen();
        } else if (initLevel) {
            initLevel = false;
            levels.at(loadedLevel)->awaitLoad();
            levels.at(loadedLevel)->onStart(ecs);
            levels.at(loadedLevel)->onUpdate(ecs, deltaTime);
        } else if (requestedLevel != LEVEL_NULL) {
            if (loadedLevel != LEVEL_NULL) {
                levels.at(loadedLevel)->onStop(ecs);
                levels.at(loadedLevel)->unload();
            }
            loadedLevel = requestedLevel;
            requestedLevel = LEVEL_NULL;
            loading = true;
            loadingProgress = 0;
            levels.at(loadedLevel)->startLoad(*this);
            drawLoadingScreen();
        } else {
            levels.at(loadedLevel)->awaitLoad();
            levels.at(loadedLevel)->onUpdate(ecs, deltaTime);
        }
    }

private:
    void onLoadProgress(LevelID level, float progress) override {
        loadingProgress = progress;
    }

    void onLoadFinish(LevelID level) override {
        initLevel = true;
        loading = false;
    }

    void onLoadError(LevelID level, std::exception_ptr exception) override {
        loading = false;
        loadException = exception;
    }

private:
    void drawLoadingScreen() {
        auto targetSize = target.getDescription().size.convert<float>();
        auto size = targetSize;
        size.x /= 3;
        size.y /= 10;
        ren2d.renderBegin(target, true, clearColor);
        ren2d.draw(Rectf(targetSize / 2 - size / 2, size), barBgColor, true);
        ren2d.draw(Rectf(targetSize / 2 - size / 2, {size.x * loadingProgress, size.y}), barColor, true);
        ren2d.renderPresent();
    }

    RenderTarget &target;
    Renderer2D &ren2d;
    ECS &ecs;

    LevelID requestedLevel = LEVEL_NULL;
    LevelID loadedLevel = LEVEL_NULL;
    std::map<LevelID, std::unique_ptr<Level>> levels;

    bool initLevel = false;
    std::exception_ptr loadException;
    bool loading = false;

    ColorRGBA barBgColor = ColorRGBA::white(0.5);
    ColorRGBA barColor = ColorRGBA::white();
    ColorRGBA clearColor = ColorRGBA::black();
    float loadingProgress = 0;
};

#endif //FOXTROT_LEVELLOADER_HPP

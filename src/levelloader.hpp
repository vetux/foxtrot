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

#include "xng/xng.hpp"

using namespace xng;

class LevelLoader : Level::LoadListener {
public:
    LevelLoader(RenderTarget &target,
                Renderer2D &ren2d,
                Window &window,
                FontDriver &fontDriver,
                PhysicsDriver &physicsDriver,
                AudioDevice &audioDevice,
                std::shared_ptr<EventBus> eventBus)
            : target(target),
              ren2d(ren2d),
              window(window),
              fontDriver(fontDriver),
              physicsDriver(physicsDriver),
              audioDevice(audioDevice),
              eventBus(std::move(eventBus)) {}

    ~LevelLoader() {
        if (currentLevel) {
            currentLevel->awaitLoad();
            currentLevel->onStop();
        }
        if (nextLevel) {
            nextLevel->awaitLoad();
            nextLevel->onStop();
        }
    }

    void loadLevel(LevelID id) {
        switch (id) {
            default:
            case LEVEL_MAIN_MENU:
                nextLevel = std::make_unique<MainMenu>(eventBus,
                                                       window,
                                                       target,
                                                       ren2d,
                                                       fontDriver);
                break;
            case LEVEL_ZERO:
                nextLevel = std::make_unique<Level0>(eventBus,
                                                     window,
                                                     target,
                                                     ren2d,
                                                     fontDriver,
                                                     physicsDriver,
                                                     audioDevice);
                break;
        }
    }

    void update(DeltaTime deltaTime) {
        if (loading) {
            drawLoadingScreen();
        } else if (initLevel) {
            initLevel = false;
            currentLevel->awaitLoad();
            currentLevel->onStart();
            currentLevel->onUpdate(deltaTime);
        } else if (nextLevel) {
            if (currentLevel) {
                currentLevel->onStop();
                currentLevel->unload();
            }
            currentLevel = std::move(nextLevel);
            nextLevel = nullptr;
            loading = true;
            loadingProgress = 0;
            currentLevel->startLoad(*this);
            drawLoadingScreen();
        } else {
            currentLevel->awaitLoad();
            currentLevel->onUpdate(deltaTime);
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
        ren2d.renderBegin(target, clearColor);
        ren2d.draw(Rectf(targetSize / 2 - size / 2, size), barBgColor, true);
        ren2d.draw(Rectf(targetSize / 2 - size / 2, {size.x * loadingProgress, size.y}), barColor, true);
        ren2d.renderPresent();
    }

    RenderTarget &target;
    Renderer2D &ren2d;
    Window &window;
    FontDriver &fontDriver;
    PhysicsDriver &physicsDriver;
    AudioDevice &audioDevice;
    std::shared_ptr<EventBus> eventBus;

    std::unique_ptr<Level> currentLevel;
    std::unique_ptr<Level> nextLevel;

    bool initLevel = false;
    std::exception_ptr loadException;
    bool loading = false;

    ColorRGBA barBgColor = ColorRGBA::white(0.5);
    ColorRGBA barColor = ColorRGBA::white();
    ColorRGBA clearColor = ColorRGBA::black();
    float loadingProgress = 0;
};

#endif //FOXTROT_LEVELLOADER_HPP

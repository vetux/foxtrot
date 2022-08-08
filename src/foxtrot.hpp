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

#ifndef FOXTROT_FOXTROT_HPP
#define FOXTROT_FOXTROT_HPP

#include "app/application.hpp"
#include "xengine.hpp"

#include "levels/mainmenu.hpp"
#include "levels/level0.hpp"

#include "loadlevelevent.hpp"

using namespace xng;

class Foxtrot : public Application, public EventListener {
public:
    Foxtrot(int argc, char *argv[]) : Application(argc, argv),
                                      fontDriver(DriverRegistry::load<FontDriver>("freetype")),
                                      archive(std::filesystem::current_path().append("assets")),
                                      shaderCompiler(DriverRegistry::load<SPIRVCompiler>("shaderc")),
                                      shaderDecompiler(DriverRegistry::load<SPIRVDecompiler>("spirv-cross")),
                                      ren2d(*renderDevice,
                                            *shaderCompiler,
                                            *shaderDecompiler),
                                      ecs() {
        ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>(archive));
        ResourceRegistry::getDefaultRegistry().setDefaultScheme("file");
        auto parsers = std::vector<std::unique_ptr<ResourceParser>>();
        parsers.emplace_back(std::make_unique<JsonParser>());
        parsers.emplace_back(std::make_unique<StbiParser>());
        ResourceRegistry::getDefaultRegistry().setImporter(ResourceImporter(std::move(parsers)));

        currentLevel = MAIN_MENU;
        targetLevel = MAIN_MENU;

        eventBus.addListener(*this);

        ren2d.renderClear(window->getRenderTarget(), ColorRGBA::black(),  {}, window->getRenderTarget().getDescription().size);

        window->swapBuffers();
        window->update();
    }

    ~Foxtrot() override {
        eventBus.removeListener(*this);
    }

    void loadLevel(LevelName level){
        levels.at(currentLevel)->onDestroy(ecs);
        currentLevel = level;
        levels.at(currentLevel)->onCreate(ecs);
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(LoadLevelEvent)){
            auto &ev = event.as<LoadLevelEvent>();
            targetLevel = ev.name;
        }
    }

protected:
    void start() override {
        scenes = loadScenes({"scenes/menu.json", "scenes/level_0.json"});

        levels[MAIN_MENU] = std::make_unique<MainMenu>(eventBus, *window, ren2d, *fontDriver, archive, scenes);
        levels[LEVEL_0] = std::make_unique<Level0>(eventBus, *window, ren2d, *fontDriver, archive, scenes);

        levels.at(currentLevel)->onCreate(ecs);

        ecs.start();
    }

    void stop() override {
        levels.at(currentLevel)->onDestroy(ecs);

        ecs.stop();
    }

    void update(DeltaTime deltaTime) override {
        if (currentLevel != targetLevel){
            loadLevel(targetLevel);
            targetLevel = currentLevel;
        }
        ren2d.renderClear(window->getRenderTarget(),
                          ColorRGBA::black(),
                          {},
                          window->getRenderTarget().getDescription().size);
        ecs.update(deltaTime);
        levels.at(currentLevel)->onUpdate(ecs, deltaTime);
        Application::update(deltaTime);
    }

private:
    std::vector<std::shared_ptr<EntityScene>> loadScenes(const std::vector<std::string> &paths) {
        std::vector<std::shared_ptr<EntityScene>> ret;
        for (auto &path: paths) {
            auto scene = std::make_shared<EntityScene>();
            auto stream = archive.open(path);
            *scene << JsonProtocol().deserialize(*stream);
            ret.emplace_back(scene);
        }
        return ret;
    }

    std::unique_ptr<FontDriver> fontDriver;
    std::unique_ptr<SPIRVCompiler> shaderCompiler;
    std::unique_ptr<SPIRVDecompiler> shaderDecompiler;

    DirectoryArchive archive;
    Renderer2D ren2d;
    EventBus eventBus;
    ECS ecs;

    std::vector<std::shared_ptr<EntityScene>> scenes;

    LevelName currentLevel;
    LevelName targetLevel;

    std::map<LevelName, std::unique_ptr<Level>> levels;
};

#endif //FOXTROT_FOXTROT_HPP

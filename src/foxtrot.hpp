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

#include "scenemanager.hpp"

using namespace xng;

class Foxtrot : public Application {
public:
    Foxtrot(int argc, char *argv[]) : Application(argc, argv),
                                      fontDriver(DriverRegistry::load<FontDriver>("freetype")),
                                      archive(std::filesystem::current_path().append("assets")),
                                      shaderCompiler(DriverRegistry::load<SPIRVCompiler>("shaderc")),
                                      shaderDecompiler(DriverRegistry::load<SPIRVDecompiler>("spirv-cross")),
                                      ren2d(*renderDevice,
                                            *shaderCompiler,
                                            *shaderDecompiler),
                                      ecs(),
                                      sceneManager(scenes),
                                      guiEventSystem(*window, eventBus),
                                      canvasRenderSystem(ren2d,
                                                         window->getRenderTarget(),
                                                         *fontDriver,
                                                         archive) {
        ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>(archive));
        ResourceRegistry::getDefaultRegistry().setDefaultScheme("file");
        auto parsers = std::vector<std::unique_ptr<ResourceParser>>();
        parsers.emplace_back(std::make_unique<JsonParser>());
        parsers.emplace_back(std::make_unique<StbiParser>());
        ResourceRegistry::getDefaultRegistry().setImporter(ResourceImporter(std::move(parsers)));
    }

protected:
    void start() override {
        scenes = loadScenes({"scenes/menu.json", "scenes/level_0.json"});

        sceneManager = SceneManager(scenes);

        ecs.setSystems({guiEventSystem, spriteAnimationSystem, canvasRenderSystem});
        ecs.setScene(sceneManager.loadScene("menu"));
        ecs.start();
    }

    void stop() override {
        ecs.stop();
    }

    void update(DeltaTime deltaTime) override {
        ren2d.renderClear(window->getRenderTarget(),
                          ColorRGBA::black(),
                          {},
                          window->getRenderTarget().getDescription().size);
        ecs.update(deltaTime);
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
    GuiEventSystem guiEventSystem;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    std::vector<std::shared_ptr<EntityScene>> scenes;
    SceneManager sceneManager;
};

#endif //FOXTROT_FOXTROT_HPP

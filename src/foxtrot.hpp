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
        scene = std::make_shared<EntityScene>();

        ecs.setSystems({spriteAnimationSystem, canvasRenderSystem});
        ecs.setScene(scene);
        ecs.start();

        auto stream = archive.open("scene.json");
        *scene << JsonProtocol().deserialize(*stream);
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
    std::unique_ptr<FontDriver> fontDriver;
    std::unique_ptr<SPIRVCompiler> shaderCompiler;
    std::unique_ptr<SPIRVDecompiler> shaderDecompiler;
    DirectoryArchive archive;
    Renderer2D ren2d;
    ECS ecs;
    CanvasRenderSystem canvasRenderSystem;
    SpriteAnimationSystem spriteAnimationSystem;
    std::shared_ptr<EntityScene> scene;
};

#endif //FOXTROT_FOXTROT_HPP

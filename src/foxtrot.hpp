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

#include "console.hpp"

#include "levelloader.hpp"

#include "events/loadlevelevent.hpp"

using namespace xng;

class Foxtrot : public Application, public EventListener {
public:
    Foxtrot(int argc, char *argv[]) : Application(argc, argv, "Foxtrot", {640, 480}),
                                      fontDriver(DriverRegistry::load<FontDriver>("freetype")),
                                      archive(std::filesystem::current_path().append("assets")),
                                      shaderCompiler(DriverRegistry::load<SPIRVCompiler>("shaderc")),
                                      shaderDecompiler(DriverRegistry::load<SPIRVDecompiler>("spirv-cross")),
                                      ren2d(*renderDevice,
                                            *shaderCompiler,
                                            *shaderDecompiler),
                                      ecs(),
                                      levelLoader(window->getRenderTarget(),
                                                  ren2d,
                                                  ecs,
                                                  eventBus,
                                                  *window,
                                                  *fontDriver) {
        EntityScene::addComponentDeserializer<PlayerComponent>("player");
        EntityScene::addComponentDeserializer<FloorComponent>("floor");
        EntityScene::addComponentDeserializer<BackdropComponent>("backdrop");
        EntityScene::addComponentDeserializer<HealthComponent>("health");
        EntityScene::addComponentDeserializer<InputComponent>("input");
        EntityScene::addComponentDeserializer<CharacterControllerComponent>("character");

        ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>(archive));
        ResourceRegistry::getDefaultRegistry().setDefaultScheme("file");
        auto parsers = std::vector<std::unique_ptr<ResourceParser>>();
        parsers.emplace_back(std::make_unique<JsonParser>());
        parsers.emplace_back(std::make_unique<StbiParser>());
        ResourceRegistry::getDefaultRegistry().setImporter(ResourceImporter(std::move(parsers)));

        eventBus.addListener(*this);

        ren2d.renderClear(window->getRenderTarget(), ColorRGBA::black(), {},
                          window->getRenderTarget().getDescription().size);

        window->swapBuffers();
        window->update();
    }

    ~Foxtrot() override {
        eventBus.removeListener(*this);
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(LoadLevelEvent)) {
            auto &ev = event.as<LoadLevelEvent>();
            levelLoader.loadLevel(ev.name);
        } else if (event.getEventType() == typeid(InputEvent)) {
            auto &ev = event.as<InputEvent>();
            if (ev.deviceType == xng::InputEvent::DEVICE_KEYBOARD) {
                if (std::get<KeyboardEventData>(ev.data).type == xng::KeyboardEventData::KEYBOARD_KEY_DOWN
                    && std::get<KeyboardEventData>(ev.data).key == KEY_F5) {
                    ResourceRegistry::getDefaultRegistry().reloadAllResources();
                }
            }
        }
    }

protected:
    void start() override {
        levelLoader.loadLevel(LEVEL_MAIN_MENU);
    }

    void stop() override {}

    void update(DeltaTime deltaTime) override {
        ren2d.renderClear(window->getRenderTarget(),
                          ColorRGBA::yellow(),
                          {},
                          window->getRenderTarget().getDescription().size);
        levelLoader.update(deltaTime);
        Application::update(deltaTime);
    }

private:
    std::unique_ptr<FontDriver> fontDriver;
    std::unique_ptr<SPIRVCompiler> shaderCompiler;
    std::unique_ptr<SPIRVDecompiler> shaderDecompiler;

    DirectoryArchive archive;
    Renderer2D ren2d;
    EventBus eventBus;
    ECS ecs;

    LevelLoader levelLoader;
};

#endif //FOXTROT_FOXTROT_HPP

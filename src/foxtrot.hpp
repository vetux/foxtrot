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

#include "console/console.hpp"

#include "levelloader.hpp"

#include "events/loadlevelevent.hpp"

using namespace xng;

class Foxtrot : public Application, public EventListener, public ConsoleOutput, public ConsoleParser {
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
        REGISTER_COMPONENT(BackdropComponent)
        REGISTER_COMPONENT(CharacterControllerComponent)
        REGISTER_COMPONENT(FloorComponent)
        REGISTER_COMPONENT(FpsComponent)
        REGISTER_COMPONENT(HealthComponent)
        REGISTER_COMPONENT(InputComponent)
        REGISTER_COMPONENT(PlayerComponent)

        auto parsers = std::vector<std::unique_ptr<ResourceParser>>();
        parsers.emplace_back(std::make_unique<JsonParser>());
        parsers.emplace_back(std::make_unique<StbiParser>());

        ResourceRegistry::getDefaultRegistry().setImporter(ResourceImporter(std::move(parsers)));
        ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>(archive));
        ResourceRegistry::getDefaultRegistry().setDefaultScheme("file");

        ResourceHandle<RawAsset> fontAsset(Uri("fonts/Space_Mono/SpaceMono-Regular.ttf"));

        std::string str = std::string(fontAsset.get().bytes.begin(), fontAsset.get().bytes.end());
        std::stringstream stream(str);
        consoleFont = fontDriver->createFont(stream);

        consoleFont->setPixelSize(Vec2i(0, 25));

        consoleTextRenderer = std::make_unique<TextRenderer>(*consoleFont, ren2d);

        console.addOutput(*this);
        console.addParser(*this);

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
                auto kbev = std::get<KeyboardEventData>(ev.data);
                if (kbev.type == xng::KeyboardEventData::KEYBOARD_KEY_DOWN) {
                    auto key = kbev.key;
                    switch (key) {
                        case KEY_F5:
                            ResourceRegistry::getDefaultRegistry().reloadAllResources();
                            break;
                        case KEY_BACKSPACE:
                            if (!consoleInput.empty())
                                consoleInput.pop_back();
                            break;
                        case KEY_RETURN:
                            console.invokeCommand(consoleInput);
                            consoleInput = "";
                            break;
                        case KEY_F12:
                            consoleOpen = !consoleOpen;
                            break;
                    }
                } else if (kbev.type == xng::KeyboardEventData::KEYBOARD_CHAR_INPUT) {
                    if (consoleOpen) {
                        consoleInput += kbev.character;
                    }
                }
            }
        }
    }

    void print(const std::string &str) override {
        consoleOutput += str + "\n";
    }

    bool parseCommand(const ConsoleCommand &command, ConsoleOutput &printer) override {
        static const std::map<std::string, std::function<void()>> commands = {
                {"loadlevel", [this, command]() {
                    levelLoader.loadLevel(parseLevelID(command.arguments.at(0)));
                }},
                {"reloadlevel", [this, command]() {
                    levelLoader.loadLevel(levelLoader.getLevel().getID());
                }},
                {"fps", [this, &printer]() {
                    printer.print(std::to_string(fpsAverage));
                }},
        };
        auto it = commands.find(command.cmd);
        if (it != commands.end()) {
            it->second();
            return true;
        } else {
            return false;
        }
    }

protected:
    void start() override {
        levelLoader.loadLevel(LEVEL_MAIN_MENU);
    }

    void stop() override {}

    void update(DeltaTime deltaTime) override {
        fps = deltaTime > 0 ? 1.0f / deltaTime : 0;
        fpsAverage = fpsAlpha * fpsAverage + (1.0f - fpsAlpha) * fps;
        ren2d.renderClear(window->getRenderTarget(),
                          ColorRGBA::yellow(),
                          {},
                          window->getRenderTarget().getDescription().size);
        levelLoader.update(deltaTime);
        if (consoleOpen) {
            updateConsole(deltaTime);
        }
        Application::update(deltaTime);
    }

private:
    LevelID parseLevelID(const std::string &id) {
        return (LevelID) std::stoi(id);
    }

    void updateConsole(DeltaTime deltaTime) {
        const float inputHeight = 50;
        const float spacingBetweenInputOutput = 15;
        auto winSize = window->getRenderTarget().getDescription().size.convert<float>();
        auto conSize = Vec2f{winSize.x, winSize.y / 3 + spacingBetweenInputOutput};
        auto inputSize = Vec2f{winSize.x, inputHeight + spacingBetweenInputOutput};
        auto outputSize = Vec2f{winSize.x, conSize.y - inputSize.y};
        auto inputPos = Vec2f(0, outputSize.y + inputSize.y / 2 + spacingBetweenInputOutput);
        auto outputPos = Vec2f();

        ren2d.renderBegin(window->getRenderTarget(), false);
        ren2d.draw(Rectf(outputPos, conSize), ColorRGBA::grey(1, 225));
        ren2d.renderPresent();

        updateConsoleInput(inputPos, inputSize);
        updateConsoleOutput(deltaTime, outputPos, outputSize);
    }

    void updateConsoleInput(Vec2f inputPos, const Vec2f &inputSize) {
        auto inputText = consoleTextRenderer->render("> " + consoleInput, TextRenderProperties{.lineHeight = 20});
        auto textSize = inputText.getTexture().getDescription().size.convert<float>();

        const auto padding = 25;

        inputPos.x += padding;
        inputPos.y -= padding;

        ren2d.renderBegin(window->getRenderTarget(), false);

        if (textSize.x + padding * 2 > inputSize.x) {
            auto diff = textSize.x - inputSize.x;
            ren2d.draw(inputText,
                       Rectf({-(textSize.x * consoleInputScroll) + diff + padding, 0}, {inputSize.x, textSize.y}),
                       Rectf({inputPos.x - padding, inputPos.y}, {inputSize.x, textSize.y}), ColorRGBA::white());
        } else {
            ren2d.draw(inputText, Rectf({}, textSize),
                       Rectf(inputPos, textSize), ColorRGBA::white());
        }

        ren2d.renderPresent();
    }

    void updateConsoleOutput(DeltaTime deltaTime, Vec2f outputPos, const Vec2f &outputSize) {
        Vec2f textSize;
        if (!consoleOutput.empty()) {
            const auto padding = 15;

            auto outputText = consoleTextRenderer->render(consoleOutput,
                                                          TextRenderProperties{.lineHeight = 20,
                                                                  .lineWidth = (int) outputSize.x - padding * 2,
                                                                  .alignment = xng::ALIGN_LEFT});
            textSize = outputText.getTexture().getDescription().size.convert<float>();
            auto displaySize = outputSize;

            displaySize.x -= padding * 2;
            outputPos.x += padding;

            ren2d.renderBegin(window->getRenderTarget(), false);

            if (displaySize.y < outputSize.y) {
                auto diff = outputSize.y - displaySize.y;
                outputPos.y += diff;
                ren2d.draw(outputText,
                           Rectf({}, displaySize),
                           Rectf(outputPos, displaySize),
                           ColorRGBA::white());
            } else {
                auto diff = textSize.y - outputSize.y;
                ren2d.draw(outputText,
                           Rectf({0, -(textSize.y * consoleOutputScroll) + diff}, displaySize),
                           Rectf(outputPos, displaySize),
                           ColorRGBA::white());
            }

            ren2d.renderPresent();
        }

        if (window->getInput().getMouse().wheelDelta > 0) {
            consoleScrollUp(deltaTime, textSize, outputSize);
        } else if (window->getInput().getMouse().wheelDelta < 0) {
            consoleScrollDown(deltaTime, textSize);
        }
    }

    void consoleScrollDown(DeltaTime deltaTime, const Vec2f &textSize) {
        if (consoleOutputScroll <= 0)
            consoleOutputScroll = 0;
        else
            consoleOutputScroll -= (1000 / textSize.y) * deltaTime;
    }

    void consoleScrollUp(DeltaTime deltaTime, const Vec2f &textSize, const Vec2f &outputSize) {
        float diff = textSize.y > outputSize.y ? outputSize.y / textSize.y : textSize.y / outputSize.y;
        float maxScroll = 1 - diff;
        if (consoleOutputScroll >= maxScroll)
            consoleOutputScroll = maxScroll;
        else
            consoleOutputScroll += (1000 / textSize.y) * deltaTime;
    }

    std::unique_ptr<FontDriver> fontDriver;
    std::unique_ptr<SPIRVCompiler> shaderCompiler;
    std::unique_ptr<SPIRVDecompiler> shaderDecompiler;

    DirectoryArchive archive;
    Renderer2D ren2d;
    EventBus eventBus;
    ECS ecs;

    LevelLoader levelLoader;

    Console console;

    std::string consoleInput;
    std::string consoleOutput;

    std::unique_ptr<Font> consoleFont;
    std::unique_ptr<TextRenderer> consoleTextRenderer;

    float consoleInputScroll = 0;
    float consoleOutputScroll = 0;

    bool consoleOpen = false;

    float fps = 0;

    float fpsAverage = 0;
    float fpsAlpha = 0.9;
};

#endif //FOXTROT_FOXTROT_HPP

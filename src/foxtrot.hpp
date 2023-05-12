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

#include "xng/xng.hpp"

#include "levels/mainmenu.hpp"
#include "levels/level0.hpp"

#include "console/console.hpp"

#include "levelloader.hpp"

#include "events/loadlevelevent.hpp"

using namespace xng;

class Foxtrot : public Application, public EventListener, public ConsoleOutput, public ConsoleParser {
public:
    Foxtrot(int argc, char *argv[]) : Application(argc, argv),
                                      archive(std::filesystem::current_path().append("assets").string()),
                                      displayDriver(),
                                      gpuDriver(),
                                      window(displayDriver.createWindow(xng::OPENGL_4_6)),
                                      renderDevice(gpuDriver.createRenderDevice()),
                                      screenTarget(window->getRenderTarget(*renderDevice)),
                                      ren2d(*renderDevice,
                                            shaderCompiler,
                                            shaderDecompiler),
                                      eventBus(std::make_shared<EventBus>()),
                                      levelLoader(*screenTarget,
                                                  ren2d,
                                                  *window,
                                                  fontDriver,
                                                  physicsDriver,
                                                  *audioDevice,
                                                  eventBus) {
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
        parsers.emplace_back(std::make_unique<SndFileParser>());

        ResourceRegistry::getDefaultRegistry().setImporter(ResourceImporter(std::move(parsers)));
        ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>(archive));
        ResourceRegistry::getDefaultRegistry().setDefaultScheme("file");

        ResourceHandle<RawResource> fontAsset(Uri("fonts/Space_Mono/SpaceMono-Regular.ttf"));

        std::string str = std::string(fontAsset.get().bytes.begin(), fontAsset.get().bytes.end());
        std::stringstream stream(str);
        consoleFont = fontDriver.createFont(stream);

        consoleTextRenderer = std::make_unique<TextRenderer>(*consoleFont, ren2d, Vec2i(0, 25));

        console.addOutput(*this);
        console.addParser(*this);

        eventBus->addListener(*this);
        ren2d.renderClear(*screenTarget,
                          ColorRGBA::black(),
                          {},
                          screenTarget->getDescription().size);
        window->swapBuffers();
        window->update();

        frameLimiter.setTargetFrameRate(144);
    }

    ~Foxtrot() override {
        eventBus->removeListener(*this);
    }

    void onEvent(const Event &event) override {
        if (event.getEventType() == typeid(LoadLevelEvent)) {
            auto &ev = event.as<LoadLevelEvent>();
            currentLevel = ev.name;
            levelLoader.loadLevel(ev.name);
        } else if (event.getEventType() == typeid(KeyboardEvent)) {
            auto kbev = event.as<KeyboardEvent>();
            if (kbev.type == xng::KeyboardEvent::KEYBOARD_KEY_DOWN) {
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
            } else if (kbev.type == xng::KeyboardEvent::KEYBOARD_CHARACTER_INPUT) {
                if (consoleOpen) {
                    if (kbev.value < 128) {
                        consoleInput += static_cast<char>(kbev.value);
                    }
                }
            }
        }
    }

    void print(const std::string &str) override {
        consoleOutput += str + "\n";
    }

    bool parseCommand(const ConsoleCommand &command, ConsoleOutput &printer) override {
        const std::map<std::string, std::function<void()>> commands = {
                {"loadlevel",   [this, &command]() {
                    levelLoader.loadLevel(parseLevelID(command.arguments.at(0)));
                }},
                {"reloadlevel", [this]() {
                    levelLoader.loadLevel(currentLevel);
                }},
                {"fps",         [this, &printer]() {
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
        ren2d.renderClear(*screenTarget,
                          ColorRGBA::yellow(),
                          {},
                          screenTarget->getDescription().size);
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
        auto winSize = screenTarget->getDescription().size.convert<float>();
        auto conSize = Vec2f{winSize.x, winSize.y / 3 + spacingBetweenInputOutput};
        auto inputSize = Vec2f{winSize.x, inputHeight + spacingBetweenInputOutput};
        auto outputSize = Vec2f{winSize.x, conSize.y - inputSize.y};
        auto inputPos = Vec2f(0, outputSize.y + inputSize.y / 2 + spacingBetweenInputOutput);
        auto outputPos = Vec2f();

        ren2d.renderBegin(*screenTarget, false, {}, {}, screenTarget->getDescription().size, {});
        ren2d.draw(Rectf(outputPos, conSize), ColorRGBA::gray(1, 225));
        ren2d.renderPresent();

        updateConsoleInput(inputPos, inputSize);
        updateConsoleOutput(deltaTime, outputPos, outputSize);
    }

    void updateConsoleInput(Vec2f inputPos, const Vec2f &inputSize) {
        if (renderedConsoleInput != consoleInput) {
            renderedConsoleInput = consoleInput;
            auto inputText = consoleTextRenderer->render("> " + consoleInput, TextLayout{.lineHeight = 20});
            inputTextHandle = ren2d.createTexture(inputText.getImage());
        }

        const auto padding = 25;

        inputPos.x += padding;
        inputPos.y -= padding;

        ren2d.renderBegin(*screenTarget, false, {}, {}, screenTarget->getDescription().size, {});

        auto textSize = inputTextHandle.size.convert<float>();

        if (textSize.x + padding * 2 > inputSize.x) {
            auto diff = textSize.x - inputSize.x;
            ren2d.draw(Rectf({-(textSize.x * consoleInputScroll) + diff + padding, 0}, {inputSize.x, textSize.y}),
                       Rectf({inputPos.x - padding, inputPos.y}, {inputSize.x, textSize.y}),
                       inputTextHandle,
                       {},
                       0,
                       NEAREST,
                       ColorRGBA::white());
        } else {
            ren2d.draw(Rectf({}, textSize),
                       Rectf(inputPos, textSize),
                       inputTextHandle,
                       {},
                       0,
                       NEAREST,
                       ColorRGBA::white());
        }

        ren2d.renderPresent();
    }

    void updateConsoleOutput(DeltaTime deltaTime, Vec2f outputPos, const Vec2f &outputSize) {
        Vec2f textSize;
        if (!consoleOutput.empty()) {
            const auto padding = 15;

            if (renderedConsoleOutput != consoleOutput) {
                renderedConsoleOutput = consoleOutput;
                auto outputText = consoleTextRenderer->render(consoleOutput,
                                                              TextLayout{.lineHeight = 20,
                                                                      .lineWidth = (int) outputSize.x - padding * 2,
                                                                      .alignment = xng::TEXT_ALIGN_LEFT});
                outputTextHandle = ren2d.createTexture(outputText.getImage());
            }

            textSize = outputTextHandle.size.convert<float>();

            auto displaySize = outputSize;

            displaySize.x -= padding * 2;
            outputPos.x += padding;

            ren2d.renderBegin(*screenTarget, false, {}, {}, screenTarget->getDescription().size, {});

            if (displaySize.y < outputSize.y) {
                auto diff = outputSize.y - displaySize.y;
                outputPos.y += diff;
                ren2d.draw(Rectf({}, displaySize),
                           Rectf(outputPos, displaySize),
                           outputTextHandle,
                           {},
                           0,
                           NEAREST,
                           ColorRGBA::white());
            } else {
                auto diff = textSize.y - outputSize.y;
                ren2d.draw(Rectf({0, -(textSize.y * consoleOutputScroll) + diff}, displaySize),
                           Rectf(outputPos, displaySize),
                           outputTextHandle,
                           {},
                           0,
                           NEAREST,
                           ColorRGBA::white());
            }

            ren2d.renderPresent();
        }

        if (window->getInput().getMouse().wheelDelta.y > 0) {
            consoleScrollUp(deltaTime, textSize, outputSize);
        } else if (window->getInput().getMouse().wheelDelta.y < 0) {
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

    freetype::FtFontDriver fontDriver;
    shaderc::ShaderCCompiler shaderCompiler;
    spirv_cross::SpirvCrossDecompiler shaderDecompiler;
    opengl::OGLGpuDriver gpuDriver;
    glfw::GLFWDisplayDriver displayDriver;
    openal::OALAudioDriver audioDriver;
    box2d::PhysicsDriverBox2D physicsDriver;

    std::unique_ptr<Window> window;
    std::unique_ptr<RenderDevice> renderDevice;
    std::unique_ptr<RenderTarget> screenTarget;

    std::unique_ptr<AudioDevice> audioDevice;

    DirectoryArchive archive;
    Renderer2D ren2d;
    std::shared_ptr<EventBus> eventBus;

    LevelLoader levelLoader;

    Console console;

    std::string consoleInput;
    std::string consoleOutput;

    std::unique_ptr<Font> consoleFont;
    std::unique_ptr<TextRenderer> consoleTextRenderer;

    TextureAtlasHandle inputTextHandle;
    TextureAtlasHandle outputTextHandle;

    std::string renderedConsoleInput;
    std::string renderedConsoleOutput;

    float consoleInputScroll = 0;
    float consoleOutputScroll = 0;

    bool consoleOpen = false;

    float fps = 0;

    float fpsAverage = 0;
    float fpsAlpha = 0.9;

    LevelID currentLevel;
};

#endif //FOXTROT_FOXTROT_HPP

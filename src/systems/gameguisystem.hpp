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

#ifndef FOXTROT_GAMEGUISYSTEM_HPP
#define FOXTROT_GAMEGUISYSTEM_HPP

#include <sstream>

#include "xng/xng.hpp"

#include "components/playercomponent.hpp"
#include "components/fpscomponent.hpp"

using namespace xng;

class GameGuiSystem : public System, public EventListener {
public:
    explicit GameGuiSystem(Input &input)
            : input(input) {}

    void start(EntityScene &scene, EventBus &eventBus) override {
        eventBus.addListener(*this);

        toolbarEntity = scene.createEntity();
        auto rt = RectTransformComponent();
        rt.parent = "GuiCanvas";
        rt.rectTransform.position = {0, 50};
        rt.rectTransform.size = Vec2f{352, 64} * 2;
        rt.rectTransform.center = rt.rectTransform.size / 2;
        rt.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_CENTER_TOP;
        toolbarEntity.createComponent(rt);
        auto sprite = SpriteComponent();
        sprite.sprite = ResourceHandle<Sprite>(Uri("sprites/toolbar.json"));
        toolbarEntity.createComponent(sprite);

        createSlotEntities(scene);
    }

    void stop(EntityScene &scene, EventBus &eventBus) override {
        eventBus.removeListener(*this);
        scene.destroyEntity(toolbarEntity);
        for (auto &ent: slotEntities) {
            scene.destroyEntity(ent);
        }
        slotEntities.clear();
    }

    void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override {
        for (auto &pair: scene.getPool<InputComponent>()) {
            auto &health = scene.getComponent<HealthComponent>(pair.first);
            if (health.health > 0 && pair.second.aim) {
                input.setMouseCursorHidden(true);
            } else {
                input.setMouseCursorHidden(false);
            }
            break;
        }
        Entity player;
        for (auto &pair: scene.getPool<PlayerComponent>()) {
            player = Entity(pair.first, scene);
            break;
        }
        if (player.getHandle()) {
            auto &plc = player.getComponent<PlayerComponent>();
            auto &inv = plc.player.getInventory();
            auto &acc = plc.player.getAccount();

            auto ammoGui = scene.getEntity("AmmoGUI");
            auto ammoText = ammoGui.getComponent<TextComponent>();
            ammoText.text = std::to_string(plc.player.getWeapon().getClip()) + " / " +
                            std::to_string(plc.player.getWeapon().getClipSize());
            if (plc.player.getWeapon().getReloadTimer() > 0) {
                ammoText.textColor = ColorRGBA::yellow();
            } else {
                ammoText.textColor = ColorRGBA::gray();
            }
            ammoGui.updateComponent(ammoText);

            auto &health = player.getComponent<HealthComponent>();

            auto healthGui = scene.getEntity("HealthGUI");
            auto healthText = healthGui.getComponent<TextComponent>();

            healthText.textColor = ColorRGBA::red();

            healthText.text = std::to_string(floorf(health.health * 100) / 100);

            healthText.text.erase(healthText.text.find_last_not_of('0') + 1, std::string::npos);
            healthText.text.erase(healthText.text.find_last_not_of('.') + 1, std::string::npos);

            healthGui.updateComponent(healthText);
        }

        float fps = deltaTime > 0 ? std::round(1.0f / deltaTime) : 0;
        std::stringstream stream;
        stream << std::fixed << std::setprecision(0) << fps;
        std::string fpsStr = stream.str();
        for (auto &pair: scene.getPool<FpsComponent>()) {
            auto text = scene.getComponent<TextComponent>(pair.first);
            text.text = fpsStr;
            scene.updateComponent(pair.first, text);
        }
    }

    void onEvent(const Event &event) override {

    }

private:
    void createSlotEntities(EntityScene &scene) {
        for (int i = 0; i < 10; i++) {
            auto ent = scene.createEntity();
            auto rt = RectTransformComponent();
            rt.parent = "GuiCanvas";
            rt.rectTransform.position = getToolbarSlotPosition(i);
            rt.rectTransform.position.y = 50;
            rt.rectTransform.size = Vec2f{32, 32} * 2;
            rt.rectTransform.center = rt.rectTransform.size / 2;
            rt.rectTransform.alignment = xng::RectTransform::RECT_ALIGN_CENTER_TOP;
            ent.createComponent(rt);
            ent.createComponent(SpriteComponent());
            auto btn = ButtonComponent();
            btn.sprite = ResourceHandle<Sprite>(Uri("sprites/celltile.json/idle"));
            btn.spriteHover = ResourceHandle<Sprite>(Uri("sprites/celltile.json/hover"));
            btn.spritePressed = ResourceHandle<Sprite>(Uri("sprites/celltile.json/press"));
            btn.id = TOOLBAR_BUTTON + std::to_string(i);
            ent.createComponent(btn);
        }
    }

    void updateSlotEntity(int slot) {
        auto ent = slotEntities.at(slot);
        auto rt = ent.getComponent<RectTransformComponent>();
    }

    Vec2f getToolbarSlotPosition(int slot) {
        return Vec2f(64, 0) * (slot - 5) + Vec2f(32, 0);
    }

    Vec2f getInventorySlotPosition(int slot) {
        return {};
    }

    Input &input;

    Entity toolbarEntity;
    std::vector<Entity> slotEntities;

    const std::string TOOLBAR_BUTTON = "#btntoolbar_";

    Vec2f toolbarBase = {};
    RectTransform::Alignment toolbarAnchor;

    Vec2f inventoryBase = {};
    RectTransform::Alignment inventoryAnchor;
};

#endif //FOXTROT_GAMEGUISYSTEM_HPP

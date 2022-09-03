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

#include "xengine.hpp"

#include "components/playercomponent.hpp"

using namespace xng;

class GameGuiSystem : public System, public EventListener {
public:
    explicit GameGuiSystem(Input &input, EventBus &bus)
            : input(input), eventBus(bus) {}

    void start(EntityScene &scene) override {
        eventBus.addListener(*this);

        toolbarEntity = scene.createEntity();
        auto rt = CanvasTransformComponent();
        rt.canvas = "GuiCanvas";
        rt.rect.position = {0, 50};
        rt.rect.dimensions = Vec2f{352, 64} * 2;
        rt.center = rt.rect.dimensions / 2;
        rt.anchor = xng::CanvasTransformComponent::TOP_CENTER;
        toolbarEntity.createComponent(rt);
        auto sprite = SpriteComponent();
        sprite.sprite = ResourceHandle<Sprite>(Uri("sprites/toolbar.json"));
        toolbarEntity.createComponent(sprite);

        createSlotEntities(scene);
    }

    void stop(EntityScene &scene) override {
        eventBus.removeListener(*this);
        scene.destroyEntity(toolbarEntity);
        for (auto &ent : slotEntities){
            scene.destroyEntity(ent);
        }
        slotEntities.clear();
    }

    void update(DeltaTime deltaTime, EntityScene &scene) override {
        for (auto &pair: scene.getPool<InputComponent>()) {
            auto &health = scene.lookup<HealthComponent>(pair.first);
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
                ammoText.textColor = ColorRGBA::grey();
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
    }

    void onEvent(const Event &event) override {

    }

private:
    void createSlotEntities(EntityScene &scene) {
        for (int i = 0; i < 10; i++) {
            auto ent = scene.createEntity();
            auto rt = CanvasTransformComponent();
            rt.canvas = "GuiCanvas";
            rt.rect.position = getToolbarSlotPosition(i);
            rt.rect.position.y = 50;
            rt.rect.dimensions = Vec2f{32, 32} * 2;
            rt.center = rt.rect.dimensions / 2;
            rt.anchor = xng::CanvasTransformComponent::TOP_CENTER;
            ent.createComponent(rt);
            ent.createComponent(SpriteComponent());
            auto btn = ButtonComponent();
            btn.sprite = ResourceHandle<Sprite>(Uri("sprites/celltile.json$idle"));
            btn.spriteHover = ResourceHandle<Sprite>(Uri("sprites/celltile.json$hover"));
            btn.spritePressed = ResourceHandle<Sprite>(Uri("sprites/celltile.json$press"));
            btn.id = TOOLBAR_BUTTON + std::to_string(i);
            ent.createComponent(btn);
        }
    }

    void updateSlotEntity(int slot) {
        auto ent = slotEntities.at(slot);
        auto rt = ent.getComponent<CanvasTransformComponent>();
    }

    Vec2f getToolbarSlotPosition(int slot) {
        return Vec2f(64, 0) * (slot - 5) + Vec2f(32, 0);
    }

    Vec2f getInventorySlotPosition(int slot) {
        return {};
    }

    Input &input;
    EventBus &eventBus;

    Entity toolbarEntity;
    std::vector<Entity> slotEntities;

    const std::string TOOLBAR_BUTTON = "#btntoolbar_";

    Vec2f toolbarBase = {};
    CanvasTransformComponent::Anchor toolbarAnchor;

    Vec2f inventoryBase = {};
    CanvasTransformComponent::Anchor inventoryAnchor;
};

#endif //FOXTROT_GAMEGUISYSTEM_HPP

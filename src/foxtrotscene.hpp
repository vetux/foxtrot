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

#ifndef FOXTROT_FOXTROTSCENE_HPP
#define FOXTROT_FOXTROTSCENE_HPP

#include "xengine.hpp"

#include "components/playercontrollercomponent.hpp"
#include "components/floorcomponent.hpp"

using namespace xng;

class FoxtrotScene : public EntityScene {
public:
    void deserializeComponent(const EntityHandle &entity, const std::string &type, const Message &message) override {
        if (type == "player") {
            PlayerControllerComponent comp;
            comp << message;
            createComponent(entity, comp);
        } else if (type == "jump_reset") {
            createComponent(entity, FloorComponent());
        }else {
            EntityScene::deserializeComponent(entity, type, message);
        }
    }
};

#endif //FOXTROT_FOXTROTSCENE_HPP

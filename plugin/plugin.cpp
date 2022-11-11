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

#include "components/backdropcomponent.hpp"
#include "components/charactercontrollercomponent.hpp"
#include "components/floorcomponent.hpp"
#include "components/fpscomponent.hpp"
#include "components/healthcomponent.hpp"
#include "components/inputcomponent.hpp"
#include "components/playercomponent.hpp"

#include "xng/xng.hpp"


extern "C" {
EXPORT void load() {
    REGISTER_COMPONENT(BackdropComponent)
    REGISTER_COMPONENT(CharacterControllerComponent)
    REGISTER_COMPONENT(FloorComponent)
    REGISTER_COMPONENT(FpsComponent)
    REGISTER_COMPONENT(HealthComponent)
    REGISTER_COMPONENT(InputComponent)
    REGISTER_COMPONENT(PlayerComponent)
}
}

extern "C" {
EXPORT void unload() {
    UNREGISTER_COMPONENT(BackdropComponent)
    UNREGISTER_COMPONENT(CharacterControllerComponent)
    UNREGISTER_COMPONENT(FloorComponent)
    UNREGISTER_COMPONENT(FpsComponent)
    UNREGISTER_COMPONENT(HealthComponent)
    UNREGISTER_COMPONENT(InputComponent)
    UNREGISTER_COMPONENT(PlayerComponent)
}
}
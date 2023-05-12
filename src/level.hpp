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

#ifndef FOXTROT_LEVEL_HPP
#define FOXTROT_LEVEL_HPP

#include "xng/xng.hpp"

#include "levelname.hpp"

class Level {
public:
    class LoadListener {
    public:
        virtual void onLoadProgress(LevelID level, float progress) {}

        virtual void onLoadFinish(LevelID level) {}

        virtual void onLoadError(LevelID level, std::exception_ptr exception) {}
    };

    virtual ~Level() = default;

    virtual LevelID getID() = 0;

    // Loading interface
    virtual void startLoad(LoadListener &listener) { listener.onLoadFinish(getID()); };

    virtual void awaitLoad() {};

    virtual void unload() {};

    // Lifecycle interface
    virtual void onStart() {};

    virtual void onUpdate(xng::DeltaTime deltaTime) {};

    virtual void onStop() {};
};

#endif //FOXTROT_LEVEL_HPP

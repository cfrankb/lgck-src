/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <cstdio>
#include "mu_sdl.h"
#include "SDL2/SDL.h"
#include "../shared/LuaVM.h"

CMusicSDL::CMusicSDL()
{
    m_valid = true;
    if( SDL_Init(SDL_INIT_AUDIO) < 0 ) {
        CLuaVM::debugv("SDL_init failed: %s", SDL_GetError());
        m_valid = false;
    }
    if (Mix_OpenAudio(22050,AUDIO_S16SYS,2,640)<0) {
        CLuaVM::debugv("Mix_OpenAudio failed: %s", SDL_GetError());
        m_valid = false;
    }
    m_music = nullptr;
}

CMusicSDL::~CMusicSDL()
{
    close();
}

bool CMusicSDL::open(const char *file)
{
    close();
    if (m_valid) {
        m_music = Mix_LoadMUS(file);
    } else {
        CLuaVM::debugv("Mix_LoadMUS(\"%s\"): %s\n", file, Mix_GetError());
    }
    return m_music != nullptr;
}

bool CMusicSDL::play(int loop)
{
    if (m_music) {
        Mix_PlayMusic(m_music,loop);
        return true;
    }
    return false;
}

void CMusicSDL::stop()
{
    if (m_valid) {
        Mix_HaltMusic();
    }
}

void CMusicSDL::close()
{
    stop();
    if (m_music) {
        Mix_FreeMusic(m_music);
        m_music = nullptr;
    }
}

bool CMusicSDL::isValid()
{
    return m_valid;
}

const char *CMusicSDL::signature() const
{
    return "lgck-music-sdl";
}

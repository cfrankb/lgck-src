/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2014  Francois Blanchette

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

#include "stdafx.h"
#include <cstdio>
#include "../shared/interfaces/ISound.h"
#include "sn_sdl.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include "../shared/LuaVM.h"

CSndSDL::CSndSDL()
{   
    m_valid = false;
    // Initialize all SDL subsystems
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )  {
        CLuaVM::debugv("SDL_init failed: %s", SDL_GetError());
        return;
    }
    m_valid = true;
}

CSndSDL::~CSndSDL()
{
    forget();
    SDL_CloseAudio();
}

void CSndSDL::forget()
{
    for ( auto it = m_sounds.begin(); it != m_sounds.end(); ++it ) {
        unsigned int uid = it->first;
        stop(uid);
        SND * snd = it->second;
        // free chunk
        Mix_FreeChunk(snd->chunk);
        // delete snd object
        delete snd;
    }
    m_sounds.clear();
}

void CSndSDL::add(unsigned char *data, unsigned int size, unsigned int uid)
{
    if (m_sounds.find(uid) != m_sounds.end()) {
        CLuaVM::debugv("ADD: sound already added: %u", uid);
        return;
    }
    SND *snd = new SND;
    snd->channel = -1;
    snd->chunk = nullptr;
    bool fail = false;
    SDL_RWops *rw = SDL_RWFromConstMem((void*) data,  size);
    if (!rw) {
        fail = true;
        CLuaVM::debugv("SDL_RWFromConstMem Failed: %s", SDL_GetError());
    }
    if (!fail) {
        snd->chunk = Mix_LoadWAV_RW(rw, 1);
        if (!snd->chunk) {
            fail = true;
            CLuaVM::debugv("Mix_LoadWAV_RW Failed: %s", Mix_GetError());
        }
    }
    if (fail) {
        snd->chunk = nullptr;
    }
    m_sounds[uid] = snd;
}

void CSndSDL::replace(unsigned char *data, unsigned int size, unsigned int uid)
{
    // TODO: get rid of replace
    remove(uid);
    add(data,size,uid);
}

void CSndSDL::remove(unsigned int uid)
{
    if (m_sounds.find(uid) == m_sounds.end()) {
        CLuaVM::debugv("REMOVE: sound not found: %u", uid);
        return;
    }
    SND *snd = m_sounds[uid];
    if (snd->channel!=-1) {
        Mix_HaltChannel(snd->channel);
    }
    Mix_FreeChunk(snd->chunk);
    delete snd;
    m_sounds.erase(uid);
}

void CSndSDL::play(unsigned int uid)
{
    SND *snd = m_sounds[uid];
    if (snd->channel != -1
            && Mix_Playing(snd->channel)) {
        // already playing
        return;
    }
    snd->channel = Mix_PlayChannel(
                snd->channel, snd->chunk, 0);
    if (snd->channel==-1) {
        CLuaVM::debugv("Mix_PlayChannel: %s",Mix_GetError());
    }
}

void CSndSDL::stop(unsigned int uid)
{
    SND * snd = m_sounds[uid];
    if (snd->channel!=-1) {
        Mix_HaltChannel(snd->channel);
        snd->channel = -1;
    }
}

void CSndSDL::stopAll()
{
    Mix_HaltChannel(-1);
}

bool CSndSDL::isValid()
{
    return m_valid;
}

bool CSndSDL::has_sound(unsigned int uid)
{
    return m_sounds.find(uid) != m_sounds.end();
}

const char *CSndSDL::signature() const
{
    return "lgck-sdl-sound";
}

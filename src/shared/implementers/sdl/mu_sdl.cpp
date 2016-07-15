#include "stdafx.h"
#include <cstdio>
#include "mu_sdl.h"
#include "SDL2/SDL.h"

CMusicSDL::CMusicSDL()
{
    m_valid = true;
    if( SDL_Init(SDL_INIT_AUDIO) < 0 ) {
        qDebug("SDL_init failed: %s", SDL_GetError());
        m_valid = false;
    }
    if (Mix_OpenAudio(22050,AUDIO_S16SYS,2,640)<0) {
        qDebug("Mix_OpenAudio failed: %s", SDL_GetError());
        m_valid = false;
    }
    m_music = NULL;
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
        qDebug("Mix_LoadMUS(\"%s\"): %s\n", file, Mix_GetError());
    }
    return m_music != NULL;
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
        m_music = NULL;
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

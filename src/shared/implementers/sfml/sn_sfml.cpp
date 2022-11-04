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
#include "sn_sfml.h"
#include <SFML/Audio.hpp>
#include <cstring>

#define MAX_CHANNELS 32
sf::Sound m_channels[MAX_CHANNELS];

CSndSFML::CSndSFML()
{   
    m_valid = true;
}

CSndSFML::~CSndSFML()
{
    stopAll();
    forget();
}

void CSndSFML::forget()
{
    // sf::SoundBuffer
    for ( auto it = m_sounds.begin(); it != m_sounds.end(); ++it ) {
        unsigned int uid = it->first;
        stop(uid);
        SND * snd = it->second;
        // free chunk
      //  Mix_FreeChunk(snd->chunk);
        // delete snd object
        delete snd->buffer;
        delete [] snd->data;
        delete snd;
    }
    m_sounds.clear();
}

void CSndSFML::add(unsigned char *data, unsigned int size, unsigned int uid)
{
    if (m_sounds.find(uid) != m_sounds.end()) {
        qDebug("ADD: sound already added: %u", uid);
        return;
    }
    SND *snd = new SND;
    snd->data = new unsigned char[size];
    memcpy(snd->data, data, size);
    snd->buffer = new sf::SoundBuffer;
    snd->channel = -1;
    bool success = snd->buffer->loadFromMemory(snd->data, size);
    if (!success) {
        qDebug("failed to load sound: %d", uid);
        delete snd->buffer;
        delete [] snd->data;
        return;
    }
    m_sounds[uid] = snd;
}

void CSndSFML::replace(unsigned char *data, unsigned int size, unsigned int uid)
{\
    // TODO: get rid of replace
    remove(uid);
    add(data, size, uid);
}

void CSndSFML::remove(unsigned int uid)
{
    if (m_sounds.find(uid) == m_sounds.end()) {
        qDebug("REMOVE: sound not found: %u", uid);
        return;
    }
    SND *snd = m_sounds[uid];
    if (snd) {
        if (snd->channel != -1) {
            m_channels[snd->channel].stop();
        }
        delete snd->buffer;
        delete [] snd->data;
        delete snd;
    }
    m_sounds.erase(uid);
}

void CSndSFML::play(unsigned int uid)
{
    if (m_sounds.find(uid) == m_sounds.end()) {
        qDebug("PLAY: sound not found: %u", uid);
        return;
    }
    SND *snd = m_sounds[uid];

    if (snd->channel != -1 && m_channels[snd->channel].getStatus() == sf::SoundSource::Playing) {
        // already playing
        return;
    }
    snd->channel = findChannel();
    if (snd->channel == -1) {
        qDebug("No channel available");
    } else {
        m_channels[snd->channel].setBuffer(*(snd->buffer));
        m_channels[snd->channel].play();
    }
}

void CSndSFML::stop(unsigned int uid)
{
    if (m_sounds.find(uid) == m_sounds.end()) {
        qDebug("STOP: sound not found: %u", uid);
        return;
    }
    SND * snd = m_sounds[uid];
    if (snd->channel != -1) {
        m_channels[snd->channel].stop();
        snd->channel = -1;
    }
}

void CSndSFML::stopAll()
{
    for (int i=0; i<MAX_CHANNELS; ++i) {
        if (m_channels[i].getStatus() != sf::SoundSource::Stopped) {
            m_channels[i].stop();
        }
    }
}

bool CSndSFML::isValid()
{
    return m_valid;
}

bool CSndSFML::has_sound(unsigned int uid)
{
    return m_sounds.find(uid) != m_sounds.end();
}

const char *CSndSFML::signature() const
{
    return "lgck-sdl-sound";
}

int CSndSFML::findChannel()
{
    for (int i=0; i<MAX_CHANNELS; ++i) {
        if (m_channels[i].getStatus() == sf::SoundSource::Stopped) {
            return i;
        }
    }
    return -1;
}

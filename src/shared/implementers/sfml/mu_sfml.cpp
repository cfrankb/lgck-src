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
#include "stdafx.h"
#include <cstdio>
#include "mu_sfml.h"
#include <SFML/Audio.hpp>

CMusicSFML::CMusicSFML()
{
    qDebug("CMusicSFML");
    m_valid = true;
    m_music =  NULL;
    qDebug("CMusicSFML OUT");
}

CMusicSFML::~CMusicSFML()
{
    close();
}

bool CMusicSFML::open(const char *file)
{
    close();
    m_music = new sf::Music;
    if (m_music) {
        if (!m_music->openFromFile(file)) {
            delete m_music;
            m_music = NULL;
            qDebug("opening %s failed\n", file);
            return false;
        }
    }
    return true;
}

bool CMusicSFML::play(int loop)
{
    if (m_music) {
        m_music->play();
        m_music->setLoop((bool)loop);
        return true;
    }
    return false;
}

void CMusicSFML::stop()
{
    if (m_music) {
        m_music->stop();
    }
}

void CMusicSFML::close()
{
    stop();
    if (m_music) {
        delete m_music;
        m_music = NULL;
    }
}

bool CMusicSFML::isValid()
{
    return m_music;
}

const char *CMusicSFML::signature() const
{
    return "lgck-music-sfml";
}

#include "stdafx.h"
#include <cstdio>
#include "mu_sfml.h"
#include <SFML/Audio.hpp>

CMusicSFML::CMusicSFML()
{
   qDebug("CMusicSFML");
    m_valid = true;
    m_music =  new sf::Music;
     qDebug("CMusicSFML OUT");
}

CMusicSFML::~CMusicSFML()
{
    close();
}

bool CMusicSFML::open(const char *file)
{
    close();
    if (m_music) {
        if (!m_music->openFromFile(file)) {
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

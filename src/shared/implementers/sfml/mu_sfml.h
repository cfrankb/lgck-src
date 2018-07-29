#ifndef MU_SFML_H
#define MU_SFML_H

#include "../shared/interfaces/IMusic.h"
namespace sf {
    class Music;
}

class CMusicSFML : public IMusic
{
public:
    CMusicSFML();
    virtual ~CMusicSFML();
    virtual bool open(const char *file);
    virtual bool play(int loop=-1);
    virtual void stop();
    virtual void close();
    virtual bool isValid();
    virtual const char *signature() const;

protected:
    sf::Music *m_music;  // Background Music
    char *m_name;
};

#endif // MU_SFML_H

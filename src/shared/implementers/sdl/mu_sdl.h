#ifndef MU_SDL_H
#define MU_SDL_H

#include "../shared/interfaces/IMusic.h"
#include "SDL2/SDL_mixer.h"

class CMusicSDL : public IMusic
{
public:
    CMusicSDL();
    virtual ~CMusicSDL();
    virtual bool open(const char *file);
    virtual bool play(int loop=-1);
    virtual void stop();
    virtual void close();
    virtual bool isValid();
    virtual const char *signature() const;

protected:
    Mix_Music *m_music;  // Background Music
    char *m_name;
};

#endif // MU_SDL_H

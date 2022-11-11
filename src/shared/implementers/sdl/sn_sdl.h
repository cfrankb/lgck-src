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

#ifndef SN_SDL_H
#define SN_SDL_H

#include <unordered_map>
#include "../shared/interfaces/ISound.h"
class ISound;

struct Mix_Chunk;
struct SDL_RWops;

typedef struct
{
    Mix_Chunk *chunk;
    int channel;
} SND;

class CSndSDL : public ISound
{
public:
    CSndSDL();
    virtual ~CSndSDL();
    virtual void forget();
    virtual void add(unsigned char *data, unsigned int size, unsigned int uid);
    virtual void replace(unsigned char *data, unsigned int size, unsigned int uid);
    virtual void remove(unsigned int uid);
    virtual void play(unsigned int uid);
    virtual void stop(unsigned int uid);
    virtual void stopAll();
    virtual bool isValid();
    virtual bool has_sound(unsigned int uid);
    virtual const char *signature() const;

protected:
    bool m_valid;
    std::unordered_map <unsigned int, SND*>m_sounds;
};

#endif // SN_SDL_H

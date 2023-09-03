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
#ifdef EMS
#include <emscripten.h>
#endif
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include <SDL2/SDL.h>
#include "../shared/Const.h"
#include "../shared/Game.h"
#include "../shared/FileWrap.h"
#include "../shared/Level.h"
#include "../shared/interfaces/IMusic.h"
#include "../shared/interfaces/ISound.h"
#include "../shared/implementers/sdl/gr_sdl.h"
#include "../shared/implementers/sdl/im_sdl.h"
#include "../shared/implementers/sdl/mu_sdl.h"
#include "../shared/implementers/sdl/sn_sdl.h"
#include "../shared/inputs/sdl/kt_sdl.h"
#include "../shared/GameEvents.h"
#include "../shared/runtime.h"

CGame *g_game;
int g_currLevel = 0;
int g_skill = 0;

// extern functions
void add_lgck_res();

void updateScreen(void)
{
    // cache the images
    static bool image_cached;
    if (!image_cached)
    {
        g_game->cacheImages();
        image_cached = true;
        printf("image cached\n");
    }

    CGRSdl *gr = static_cast<CGRSdl *>(g_game->graphics());
    if (gr)
    {
        g_game->updateScreen();
        gr->update();
    }
}

void unfoldEvents(void)
{
    int result = g_game->runEngine();
    bool dead = false;
    switch (result)
    {
    case CGame::EVENT_NO_EVENT:
        // nothing happened
        break;

    case CGame::EVENT_QUIT:
        exit(EXIT_SUCCESS);
        break;

    case CGame::EVENT_LEVEL_COMPLETED:
        if (g_currLevel < g_game->getSize() - 1)
        {
            ++g_currLevel;
            g_game->var("level") = g_currLevel;
            g_game->setEngineState(CGame::ES_INTRO);
        }
        else
        {
            puts("You have sucessfully completed this game.");
            exit(EXIT_SUCCESS);
        }
        break;

    case CGame::EVENT_TIMEOUT:
        puts("You ran out of time.");
        dead = true;
        break;

    case CGame::EVENT_PLAYER_DIED:
        puts("You were killed.");
        dead = true;
        break;

    case CGame::EVENT_NO_PLAYER:
        puts("No player object.");
        exit(EXIT_FAILURE);
        break;

    default:
        printf("unknown event: %d\n", result);
        exit(EXIT_FAILURE);
        break;
    }

    if (dead)
    {
        --g_game->counter("lives");
        if (!g_game->counter("lives"))
        {
            puts("Game Over");
            exit(EXIT_SUCCESS);
        }
        else
        {
            g_game->setEngineState(CGame::ES_INTRO);
        }
    }

    // update screen
    updateScreen();
}

void notifyKeyCode(int keyCode, bool state)
{
    int lgckCode = CKeyTranslator::translate(keyCode);
    if (lgckCode != -1)
    {
        g_game->setKey(lgckCode, state);
        g_game->setLastKey(lgckCode);
        if (state)
        {
            g_game->callLvEvent(CLevel::EL_KEY_PRESSED);
        }
        else
        {
            g_game->callLvEvent(CLevel::EL_KEY_UP);
        }
    }
}

void loop_handler(void *arg)
{
    bool quit = false;
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            }
            notifyKeyCode(e.key.keysym.sym, true);
            break;
        case SDL_KEYUP:
            notifyKeyCode(e.key.keysym.sym, false);
        }
    }
    if (quit) {
        exit(EXIT_SUCCESS);
    }
    unfoldEvents();
}

void runGame()
{
    while (true)
    {
        loop_handler(nullptr);
    }
}

int main(int argc, char *args[])
{
    CGame game;
    g_game = &game;
    add_lgck_res();
    ARGS out;
#ifdef EMS
    out = ARGS {
        .skill=0,
        .level=0,
        .width=640,
        .height=480,
        .filename="data/vla3demo258.lgckdb"
    };
#else
    if (!parseCmdLine(argc, args, out))
    {
        return EXIT_FAILURE;
    }
#endif    
    game.setFileName(out.filename.c_str());
    printf("reading data...\n");
    if (!game.read())
    {
        printf("failed to read gamedata:%s [%s]\n", game.getFileName(), game.getLastError());
        return EXIT_FAILURE;
    }
    printf("attaching sound\n");
    CSndSDL *sn = new CSndSDL();
    game.attach((ISound *)sn);
    printf("attaching music\n");
    CMusicSDL *mu = new CMusicSDL();
    game.attach((IMusic *)mu);
    printf("attaching graphics\n");
    CGRSdl *gm = new CGRSdl();
    gm->init(&game, out.width, out.height, "LGCK / SDL runtime");
    game.attach(gm->cache());
    game.attach((IGraphics *)gm);
    game.initSounds();
    game.initSettings();
    game.initFonts();
    game.initLua();
    game.setLives(5);
    game.setHealth(32);
    g_skill = std::min(out.skill, 3);
    srand(static_cast<unsigned int>(time(NULL)));
    g_currLevel = std::min(out.level != -1 ? out.level : rand() % game.getSize(), game.getSize() - 1);
    g_game->setLevel(g_currLevel);
    g_game->setEngineState(CGame::ES_INTRO);
    g_game->callGameEvent(CGameEvents::EG_INIT_GAME);

#ifdef EMS
    emscripten_set_main_loop_arg(loop_handler, &game, -1, 1);
#else    
    runGame();
#endif    
    game.attach((IGraphics *)NULL);
    game.attach((IMusic *)NULL);
    game.attach((ISound *)NULL);
    return EXIT_SUCCESS;
}

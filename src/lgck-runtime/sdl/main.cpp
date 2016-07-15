#include "stdafx.h"
#undef INT8
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <SDL2/SDL.h>
#include "Const.h"
#include "../shared/Game.h"
#include "../shared/FileWrap.h"
#include "../shared/Level.h"
#include "../shared/interfaces/IMusic.h"
#include "../shared/interfaces/ISound.h"
//#include "../shared/implementers/dm_sdl.h"
#include "../shared/implementers/sdl/gr_sdl.h"
#include "../shared/implementers/sdl/im_sdl.h"
#include "../shared/implementers/sdl/mu_sdl.h"
#include "../shared/implementers/sdl/sn_sdl.h"
#include "../shared/inputs/sdl/kt_sdl.h"
#include "../shared/GameEvents.h"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

CGame *g_game;
int g_currLevel = 0;
int g_skill = 0;

//int g_state;
//unsigned long long g_timeTo;

// extern functions
void add_lgck_res();

void updateScreen(void)
{
    // cache the images
    static bool image_cached;
    if (!image_cached) {
        g_game->m_arrFrames.add(g_game->m_points);
        g_game->cacheImages();
        image_cached = true;
        printf("image cached\n");
    }

    CGRSdl * gr = static_cast<CGRSdl *>(g_game->graphics());
    if (gr) {
        g_game->updateScreen();
        gr->update();
    }
}

void unfoldEvents(void)
{
    int result = g_game->runEngine();
    bool dead = false;
    switch (result) {
        case CGame::EVENT_NO_EVENT:
            // nothing happened
        break;
            
        case CGame::EVENT_QUIT:
            exit(EXIT_SUCCESS);
        break;
        
        case CGame::EVENT_LEVEL_COMPLETED:
            if (g_currLevel < g_game->getSize() - 1) {
                ++g_currLevel;
                g_game->var("level") = g_currLevel;
                g_game->setEngineState(CGame::ES_INTRO);
            } else {
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

    if (dead) {
        --g_game->counter("lives");
        if (!g_game->counter("lives")) {
            puts("Game Over");
            exit(EXIT_SUCCESS);
        } else {
            g_game->setEngineState(CGame::ES_INTRO);
        }
    }
    
    // update screen
    updateScreen();
}

void notifyKeyCode(int keyCode, bool state)
{
    int lgckCode = CKeyTranslator::translate(keyCode);
    if (lgckCode != -1) {
        g_game->setKey(lgckCode, state);
        g_game->setLastKey(lgckCode);
        if (state) {
            g_game->callLvEvent(CLevel::EL_KEY_PRESSED);
        } else {
            g_game->callLvEvent(CLevel::EL_KEY_UP);
        }
    }
}

void runGame()
{
    bool quit =false;
    while (!quit) {
        SDL_Event e;
        while( SDL_PollEvent( &e ) != 0 ) {
            switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit=true;
                }
                notifyKeyCode(e.key.keysym.sym, true);
                break;
            case SDL_KEYUP:
                notifyKeyCode(e.key.keysym.sym, false);
            }
        }
        unfoldEvents();
    }
   // exit(EXIT_SUCCESS);
}

int main( int argc, char* args[] )
{
    CGame game;
    g_game = & game;
    add_lgck_res();
    if (argc < 2) {
        game.setFileName(" ../../data/vla3demo241.lgckdb");
    } else {
        game.setFileName(args[1]);
    }
    printf("reading data...\n");
    if (!game.read()) {
        printf("failed to read gamedata:%s [%s]\n", game.getFileName(), game.getLastError());
        return EXIT_FAILURE;
    }

    CSndSDL *sn = new CSndSDL();
    game.attach((ISound*)sn);
    CMusicSDL *mu = new CMusicSDL();
    game.attach((IMusic*)mu);
    CGRSdl * gm = new CGRSdl(&game);
    game.attach(gm->cache());
    game.attach((IGraphics *) gm);    
    game.initSounds();
    game.initSettings();
    game.setLives(5);
    game.setHealth(32);
    g_skill = 0;
    srand(static_cast<unsigned int>(time(NULL)));
    g_currLevel = rand() % game.getSize();
    g_game->setLevel(g_currLevel);
    g_game->setEngineState(CGame::ES_INTRO);
    g_game->callGameEvent(CGameEvents::EG_INIT_GAME);
    runGame();
    game.attach((IGraphics*)NULL);
    game.attach((IMusic*)NULL);
    game.attach((ISound*)NULL);
	return 0;
} 
    

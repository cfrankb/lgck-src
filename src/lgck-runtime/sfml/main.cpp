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
#undef INT8
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include "Const.h"
#include "../shared/Game.h"
#include "../shared/FileWrap.h"
#include "../shared/Level.h"
#include "../shared/interfaces/IMusic.h"
#include "../shared/interfaces/ISound.h"
#include "../shared/implementers/sfml/gr_sfml.h"
#include "../shared/implementers/sfml/im_sfml.h"
#include "../shared/implementers/sfml/mu_sfml.h"
#include "../shared/implementers/sfml/sn_sfml.h"
#include "../shared/inputs/sfml/kt_sfml.h"
#include "../shared/GameEvents.h"
#include "../shared/Snapshot.h"
#include "../shared/DisplayManager.h"
#include "../shared/runtime.h"

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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

    CGRSfml *gr = static_cast<CGRSfml *>(g_game->graphics());
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
        g_game->snapshot().clear();
        if (g_game->svar("WarpTo") == CGame::INVALID)
        {
            // we're not warping to another level
            ++g_currLevel;
        }
        else
        {
            g_currLevel = g_game->svar("WarpTo");
        }
        if (g_currLevel < g_game->getSize() - 1)
        {
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
            if (result == CGame::EVENT_TIMEOUT)
            {
                g_game->setEngineState(CGame::ES_TIMEOUT);
            }
            else
            {
                g_game->setEngineState(CGame::ES_INTRO);
            }
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

void runGame(sf::RenderWindow *window)
{
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window->close();
                break;
            // key pressed
            case sf::Event::Resized:
                break;
            case sf::Event::KeyPressed:
                notifyKeyCode(event.key.code, true);
                break;
            case sf::Event::KeyReleased:
                notifyKeyCode(event.key.code, false);
                break;
            case sf::Event::LostFocus:
                break;
            case sf::Event::GainedFocus:
                break;
            case sf::Event::TextEntered:
                // if (event.text.unicode < 128)
                // std::cout << "ASCII character typed: " << static_cast<char>(event.text.unicode) << std::endl;
                break;
            default:
                break;
            }
        }
        unfoldEvents();
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char *args[])
{
    CGame game;
    g_game = &game;
    add_lgck_res();
    ARGS out;
    if (!parseCmdLine(argc, args, out))
    {
        return EXIT_FAILURE;
    }
    game.setFileName(out.filename.c_str());
    printf("reading data...\n");
    if (!game.read())
    {
        printf("failed to read gamedata:%s [%s]\n", game.getFileName(), game.getLastError());
        return EXIT_FAILURE;
    }
    CSndSFML *sn = new CSndSFML();
    game.attach((ISound *)sn);
    CMusicSFML *mu = new CMusicSFML();
    game.attach((IMusic *)mu);
    CGRSfml *gm = new CGRSfml();
    gm->init(&game, out.width, out.height, "LGCK / SFML runtime");
    game.attach(gm->cache());
    game.attach((IGraphics *)gm);
    game.initSounds();
    game.initSettings();
    game.initLua();
    game.initFonts();
    game.setLives(5);
    game.setHealth(32);
    g_skill = std::min(out.skill, 3);
    srand(static_cast<unsigned int>(time(NULL)));
    g_currLevel = std::min(out.level != -1 ? out.level : rand() % game.getSize(), game.getSize() - 1);
    g_game->setLevel(g_currLevel);
    g_game->setEngineState(CGame::ES_INTRO);
    g_game->callGameEvent(CGameEvents::EG_INIT_GAME);
    runGame(gm->window());
    return 0;
}

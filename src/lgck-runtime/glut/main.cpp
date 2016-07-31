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
#undef INT8
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#ifdef _WIN32
    #include <GL/freeglut.h>
#else
    #include <GL/glut.h>
#endif
#include "Const.h"
#include "../shared/Game.h"
#include "../shared/FileWrap.h"
#include "../shared/Level.h"
#include "../shared/interfaces/IMusic.h"
#include "../shared/interfaces/ISound.h"
#include "../shared/implementers/opengl/gr_opengl.h"
#include "../shared/implementers/opengl/im_opengl.h"
#include "../shared/implementers/sdl/mu_sdl.h"
#include "../shared/implementers/sdl/sn_sdl.h"
#include "../shared/inputs/glut/kt_glut.h"
#include "../shared/runtime.h"

#define INIT_LEN 640
#define INIT_HEI 480
CGame *g_game;
int g_currLevel = 0;
int g_skill = 0;

int g_state;
unsigned long long g_timeTo;

// extern functions
void add_lgck_res();

// These variables set the dimensions of the rectanglar region we wish to view.
const double Xmin = 0.0, Xmax = 3.0;
const double Ymin = 0.0, Ymax = 3.0;

// Called when the window is resized
//		w, h - width and height of the window in pixels.
void resizeWindow(int w, int h)
{
    double scale, center;
    double windowXmin, windowXmax, windowYmin, windowYmax;

    // Define the portion of the window used for OpenGL rendering.
    glViewport( 0, 0, w, h );	// View port uses whole window

    // Set up the projection view matrix: orthographic projection
    // Determine the min and max values for x and y that should appear in the window.
    // The complication is that the aspect ratio of the window may not match the
    //		aspect ratio of the scene we want to view.
    w = (w==0) ? 1 : w;
    h = (h==0) ? 1 : h;
    if ( (Xmax-Xmin)/w < (Ymax-Ymin)/h ) {
        scale = ((Ymax-Ymin)/h)/((Xmax-Xmin)/w);
        center = (Xmax+Xmin)/2;
        windowXmin = center - (center-Xmin)*scale;
        windowXmax = center + (Xmax-center)*scale;
        windowYmin = Ymin;
        windowYmax = Ymax;
    }
    else {
        scale = ((Xmax-Xmin)/w)/((Ymax-Ymin)/h);
        center = (Ymax+Ymin)/2;
        windowYmin = center - (center-Ymin)*scale;
        windowYmax = center + (Ymax-center)*scale;
        windowXmin = Xmin;
        windowXmax = Xmax;
    }

    // Now that we know the max & min values for x & y that should be visible in the window,
    //		we set up the orthographic projection.
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( windowXmin, windowXmax, windowYmin, windowYmax, -1, 1 );
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
    glutPostRedisplay();
}

void updateScreen(void)
{
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    int len = viewport[2];
    int hei = viewport[3];

    // cache the images
    static bool image_cached;
    if (!image_cached) {
        g_game->m_arrFrames.add(g_game->m_points);
        g_game->cacheImages();
        image_cached = true;
        printf("image cached\n");
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, len,0, hei,-1,1);
    glViewport(
            0,          // lower left x position
            0,			// lower left y position
            len,	// viewport width
            hei	// viewport height
    );

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glShadeModel( GL_FLAT );
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(0,0,0);
        g_game->updateGeometry(len, hei);
        CGROpenGL * gr = (CGROpenGL *)g_game->graphics();
        if (gr) {
            g_game->updateScreen();
        }
    glPopMatrix();
    glutSwapBuffers();
}

void mouse_callback(int button, int state, int x, int y) 
{
    int lgck_button = -1; 
    if (state == GLUT_DOWN) {
       switch(button){
            case GLUT_LEFT_BUTTON:
                lgck_button = CGame::BUTTON_LEFT;
            break;
            case GLUT_RIGHT_BUTTON:
                lgck_button = CGame::BUTTON_RIGHT;
       }
    }
    if (lgck_button != -1) {
        g_game->triggerMouseEvent(x, y, lgck_button);
    }
}

// http://stackoverflow.com/questions/9266045/list-of-key-constants

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

void keyPressed(unsigned char key, int x, int y)
{
    //printf("KP: %d %d %d\n", key,x,y);
    switch ( key ) {
    case 27:									// "27" is theEscape key
        exit(1);
    }
    notifyKeyCode(key, true);
}

void keyReleased(unsigned char key, int x, int y)
{
    //printf("KR: %d %d %d\n", key,x,y);
    notifyKeyCode(key, false);
}

void specialKeyPressed(int key, int x, int y)
{
    //printf("SKP: %d %d %d\n", key,x,y);
    notifyKeyCode(GLSKEY + key, true);
}

void specialKeyReleased(int key, int x, int y)
{
    //printf("SKR: %d %d %d\n", key,x,y);
    notifyKeyCode(GLSKEY + key, false);
}

int main(int argc, char *argv[])
{
    int len = INIT_LEN;
    int hei = INIT_HEI;
    CGame game;
    g_game = & game;
    add_lgck_res();
    ARGS out;
    if (!parseCmdLine(argc, argv, out)){
        return EXIT_FAILURE;
    }
    game.setFileName(out.filename.c_str());
    printf("reading data...\n");
    if (!game.read()) {
        printf("failed to read gamedata:%s\n", game.getFileName());
        return EXIT_FAILURE;
    }

    CSndSDL *sn = new CSndSDL();
    game.attach((ISound*)sn);
    CMusicSDL *mu = new CMusicSDL();
    game.attach((IMusic*)mu);
    CGROpenGL * gm = new CGROpenGL(&game);
    game.attach(gm->cache());
    game.attach((IGraphics *) gm);    
    game.initSounds();
    game.initSettings();
    printf("initSettings() done\n");
    game.setLives(5);
    game.setHealth(32);
    g_skill = std::min(out.skill,3);
    srand(static_cast<unsigned int>(time(NULL)));
    g_currLevel = std::min(out.level != -1 ? out.level : rand() % game.getSize(), game.getSize()-1);
    glutInit(&argc,argv);
    // The image is not animated so single buffering is OK. 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    // Window position (from top corner), and size (width and hieght)
    glutInitWindowPosition( 20, 60 );
    glutInitWindowSize( len, hei );
    glutCreateWindow("LGCK Runtime");
    if (!game.initFonts()) {
        puts("giving up");
        exit(EXIT_FAILURE);
    }

    // Set up callback functions for key presses
    glutKeyboardFunc(keyPressed);			// Handles "normal" ascii symbols
    glutKeyboardUpFunc(keyReleased);
    //http://stackoverflow.com/questions/15435715/opengl-glut-buttons-and-keys
    // (for mouse, joystics)
    // http://www.opengl.org/resources/libraries/glut/spec3/node54.html
    glutSpecialFunc( specialKeyPressed );		// Handles "special" keyboard keys
    glutSpecialUpFunc( specialKeyReleased );

    // Set up the callback function for resizing windows
    glutReshapeFunc( resizeWindow );

    // Call this for background processing
    glutIdleFunc( unfoldEvents );

    // call this whenever window needs redrawing
    glutDisplayFunc( updateScreen );
    glutMouseFunc(mouse_callback);
    g_game->setLevel(g_currLevel);
    g_game->setEngineState(CGame::ES_INTRO);

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return(EXIT_SUCCESS);
}

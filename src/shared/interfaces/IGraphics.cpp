#include "IGraphics.h"
#include "stdafx.h"
#include "LevelEntry.h"
#include "Frame.h"
#include "Game.h"
#include "ISurface.h"
#include "Level.h"
#include "Layer.h"
#include "Scene.h"
#include "DisplayManager.h"

void IGraphics::drawSurface(ISurface * surface, int mx, int my)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);

    int offsetX = 0;
    int offsetY = 0;
    int maxX  = m_game->BUFFERLEN;
    if (maxX < screenLen)  {
        offsetX = (screenLen - maxX) / 2;
    }

    int maxY  = m_game->BUFFERHEI;
    if (maxY < screenHei)  {
        offsetY = (screenHei - maxY) / 2;
    }

    int entries = surface->getSize();
    int frameSet;
    for (int i=0; i< entries; ++i) {
        CLevelEntry & entry = surface->atIndex(i) ;
        if ((entry.m_nProto & m_game->PROTO_POINTS) == m_game->PROTO_POINTS ) {
            frameSet = m_game->var("pointsOBL_frameSet");
        } else {
            frameSet = entry.m_nFrameSet;
        }
        CFrame & frame = m_game->toFrame(frameSet, entry.m_nFrameNo);
        int x = entry.m_nX - mx;
        int y = entry.m_nY - my;
        if (!((entry.m_nTriggerKey & TRIGGER_HIDDEN) ||
            (x + frame.m_nLen <= 0) ||
            (x >= screenLen) ||
            (y + frame.m_nHei <= 0) ||
            (y >= screenHei) ||
            (entry.m_nFrameNo & 0x8000))) {
            x += offsetX;
            y += offsetY;
            paintImage(x, screenHei - y, frameSet, entry.m_nFrameNo);
        }
    }
}

void IGraphics::getOffset(int & offsetX, int & offsetY)
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    offsetX = 0;
    offsetY = 0;
    int maxX  = m_game->BUFFERLEN;
    if (maxX < screenLen)  {
        offsetX = (screenLen - maxX) / 2;
    }

    int maxY  = m_game->BUFFERHEI;
    if (maxY < screenHei)  {
        offsetY = (screenHei - maxY) / 2;
    }
}

void IGraphics::drawScreen()
{
    int screenLen;
    int screenHei;
    getScreenSize(screenLen, screenHei);
    int offsetX;
    int offsetY;
    getOffset(offsetX, offsetY);
    clear(m_game->var("borderColor"));
    paint(offsetX,
          offsetY,
          screenLen - offsetX,
          screenHei - offsetY,
          m_game->var("bkColor") | 0xff000000);
    int colorMod = m_game->var("colorMod") | 0xff000000;
    m_colorMod.blue = (colorMod & 0xff);
    m_colorMod.green = (colorMod & 0xff00) >> 8;
    m_colorMod.red = (colorMod >> 16) & 0xff;

    for (int i = 0; i < m_game->m_layers->getSize(); ++i) {
        int speeds[] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
        CLayer & layer = (*(m_game->m_layers))[i];
        if (layer.getType() != CLayer::LAYER_MAIN) {
            // draw background and foreground layers
            int mx = 0, my = 0;
            int h, v;
            layer.getSpeed(h, v);
            if (h == CLayer::SPEED_CUSTOM) {
                layer.getOffset(mx, my);
            } else {
                if (h) {
                    mx = m_game->m_mx / speeds[h];
                }
                if (v) {
                    my = m_game->m_my / speeds[v];
                }
            }
            drawSurface(dynamic_cast<ISurface*>(&layer), mx, my);
        } else {
            // draw main layer
            int mx = m_game->m_mx;
            int my = m_game->m_my;
            drawSurface(dynamic_cast<ISurface*>(m_game->m_sBK), mx, my);
            drawSurface(dynamic_cast<ISurface*>(m_game->m_sFW), mx, my);
        }
    };
    m_displayManager->draw();
}

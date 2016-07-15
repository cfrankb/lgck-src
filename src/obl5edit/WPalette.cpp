/*
    LGCK Builder GUI
    Copyright (C) 2005, 2012  Francois Blanchette

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

#include "WPalette.h"
#include "../shared/stdafx.h"
#include "../shared/Frame.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QBitmap>
#include <QColor>

CWPalette::CWPalette(QWidget *parent) :
    QLabel(parent)
{
    m_count = 0;
    m_palette = new uint[MAX_COLORS];
    //memset(m_palette, 0, sizeof(uint)*MAX_COLORS);
    for (int i=0; i < MAX_COLORS; ++i) {
        m_palette[i] = 0xff000000;
    }

    // enable mouse tracking
    setMouseTracking(true);

    m_mouse.lButton = false;
    m_mouse.rButton = false;
    m_mouse.mButton = false;
}

CWPalette::~CWPalette()
{
    if (m_palette) {
        delete [] m_palette;
    }
}

void CWPalette::init(uint *colors, int count)
{
    QPixmap pixmap;
    pixmap.load(":/images/cursors/eyedropper.png");
    pixmap.setMask( pixmap.createHeuristicMask() );
    setCursor(QCursor(pixmap, 2, 14));

    m_count = count;
    if (colors) {
        memcpy(m_palette, colors, count*sizeof(uint));
    }
    updateView();
}

void CWPalette::updateView()
{
    // draw the palette
    CFrame *frame = new CFrame(16 * GRID_SIZE, ROW_SIZE * GRID_SIZE);
    const UINT32 *pal = m_palette;//frame->dosPal();
    for (int y=0; y < 16; ++y) {
        for (int x=0; x < ROW_SIZE; ++x) {
            const UINT32 color = pal[y * 16 + x];
            for (int i=0; i < GRID_SIZE-1; ++i) {
                for (int j=0; j < GRID_SIZE-1; ++j) {
                    frame->at(x * GRID_SIZE + j,y * GRID_SIZE + i) = color;
                }
            }
        }
    }

    UINT8 *png;
    int size;
    frame->toPng(png, size);

    QImage img;
    if (!img.loadFromData( png, size )) {
        qDebug("failed to load png\n");
    }
    delete [] png;
    delete frame;

    QPixmap pm = QPixmap::fromImage(img);
    setPixmap(pm);
}

void CWPalette::mousePressEvent ( QMouseEvent * event )
{
    switch (event->button()) {
        case Qt::LeftButton:
            m_mouse.lButton = true;
        break;

        case Qt::RightButton:
            m_mouse.rButton = true;
        break;

        case Qt::MidButton:
            m_mouse.mButton = true;
        break;

        default:
        break;
    }

    int x = event->x() / GRID_SIZE;
    int y = event->y() / GRID_SIZE;
    if (m_mouse.lButton
            && x < ROW_SIZE
            && y < ROW_SIZE) {
        int iColor = x + ROW_SIZE * y;
        UINT32 color = m_palette[iColor];//CFrame::dosPal()[iColor];
        emit colorPicked( color );
        qDebug("color picked");
        QColor qcolor = QColor(
                    color & 0xff,
                    color >> 8 & 0xff,
                    color >> 16 & 0xff
                    );
        emit colorChanged( qcolor);
    }

    if (m_mouse.rButton) {
        int iColor = x + ROW_SIZE * y;
        UINT32 color = m_palette[iColor];//CFrame::dosPal()[iColor];
        emit bkColorPicked( color );
        qDebug("bkcolor picked");
    }
}

void CWPalette::mouseReleaseEvent ( QMouseEvent * event )
{
    switch (event->button()){
        case Qt::LeftButton:
            m_mouse.lButton = false;
        break;

        case Qt::RightButton:
            m_mouse.rButton = false;
        break;

        case Qt::MidButton:
            m_mouse.mButton = false;
        break;

        default:
        break;
    }
}

void CWPalette::addColor(const uint color)
{
    int i=0, j=0;
    for (; i< m_count; ++i) {
        if (i != j) {
            m_palette[j] = m_palette[i];
            //qDebug("%d = %d", j,i);
        }
        if (m_palette[i] != color){
            ++j;
        }
    }
    m_count -= (i - j);

    for (int i=std::min(m_count, MAX_COLORS-1); i > 0 ; --i) {
        m_palette[i] = m_palette[i-1];
    }

    ++m_count;
    m_count = std::min(m_count, (int)MAX_COLORS);
    m_palette[0] = color;
    updateView();
}

void CWPalette::mouseMoveEvent ( QMouseEvent * event )
{
    int x = event->x() / GRID_SIZE;
    int y = event->y() / GRID_SIZE;
    if (m_mouse.lButton
            && x < ROW_SIZE
            && y < ROW_SIZE) {
        int iColor = x + ROW_SIZE * y;
        UINT32 color = m_palette[iColor];//CFrame::dosPal()[iColor];
        emit colorPicked( color );
       // qDebug("color picked");
        QColor qcolor = QColor(
                    color & 0xff,
                    color >> 8 & 0xff,
                    color >> 16 & 0xff
                    );
        emit colorChanged( qcolor);
    }

    if (m_mouse.rButton) {
        int iColor = x + ROW_SIZE * y;
        UINT32 color = m_palette[iColor];//CFrame::dosPal()[iColor];
        emit bkColorPicked( color );
     //   qDebug("bkcolor picked");
    }
}

const uint * CWPalette::palette() const
{
    return m_palette;
}

void CWPalette::palChanged(uint *colors, int count)
{
    init(colors, count);
}

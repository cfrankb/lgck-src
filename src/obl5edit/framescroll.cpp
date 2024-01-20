/*
    LGCK Builder package
    Copyright (C) 2005, 2016  Francois Blanchette

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

#include "Frame.h"
#include "framescroll.h"
#include "framewidget.h"
#include "Tools.h"
#include <QScrollBar>
#include <QMouseEvent>

CFrameScroll::CFrameScroll(QWidget *parent):
     QAbstractScrollArea(parent)
{
    m_widget = new CFrameWidget(this);
    setViewport(m_widget);
    connect(m_widget, SIGNAL(scrollbarAdjusted()), this, SLOT(updateScrollbars()));
    connect(this, SIGNAL(cursorChanged()), m_widget, SLOT(changeCursor()));
    connect(this, SIGNAL(toolHandler(int,int,bool,bool)), m_widget, SLOT(handleTool(int,int,bool,bool)));
    connect(this, SIGNAL(mouseUp()), m_widget, SLOT(clearDotArray()));

    // set view attributes
    setAttribute(Qt::WA_MouseTracking);
    setMouseTracking(true);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFrameShape(QFrame::NoFrame);
    initMouse();

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), m_widget, SLOT(setMX(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), m_widget, SLOT(setMY(int)));
    update();
}

void CFrameScroll::resizeEvent(QResizeEvent * event)
{
    CFrameWidget * glw = dynamic_cast<CFrameWidget *>(viewport());
    glw->resizeEvent(event);
    updateScrollbars();
}

void CFrameScroll::paintEvent(QPaintEvent *event)
{
    CFrameWidget * glw = dynamic_cast<CFrameWidget *>(viewport());
    glw->paintEvent(event);
}

void CFrameScroll::updateScrollbars()
{
   // qDebug("CFrameScroll::updateScrollbars()");
    CFrameWidget *widget = m_widget;
    QSize sz = size();
    CFrame *frame = widget->m_frame;
    int zoom = widget->m_zoom;

    if (!frame || sz.width() >= frame->len() * zoom) {
        horizontalScrollBar()->setRange(0, 0);
    } else {
        horizontalScrollBar()->setRange(0, (frame->len()*zoom - sz.width())/ zoom + 1);
    }

    if (!frame || sz.height() >= frame->hei() *zoom) {
        verticalScrollBar()->setRange(0, 0);
    } else {
        verticalScrollBar()->setRange(0, (frame->hei()*zoom - sz.height()) / zoom + 1);
    }

    horizontalScrollBar()->setPageStep(zoom);
    verticalScrollBar()->setPageStep(zoom);
}

void CFrameScroll::initMouse()
{
    m_mouse.mButton = m_mouse.lButton = m_mouse.rButton = false;
    m_mouse.oldX = m_mouse.oldY = m_mouse.x = m_mouse.y = -1;
}

void CFrameScroll::mousePressEvent(QMouseEvent * event)
{
    QList<int> tools;
    tools.append(TOOL_PEN);
    tools.append(TOOL_ERASER);
    tools.append(TOOL_FLOODFILL);
    tools.append(TOOL_LINE);
    tools.append(TOOL_RECT);
    tools.append(TOOL_CIRCLE);

    CFrameWidget *widget = m_widget;// static_cast<CFrameWidget *>(viewport());
    //QSize sz = size();
    CFrame *frame = widget->m_frame;
    int & tool = widget->m_tool;
    int & mode = widget->m_mode;

    switch (event->button())
    {
    case Qt::LeftButton:
        m_mouse.lButton = true;
        if (frame && (mode != CFrameWidget::MODE_TILED_VIEW)
                && tools.indexOf(tool) != -1) {
            frame->push();
            emit modified();
        }
        break;

    case Qt::RightButton:
        m_mouse.rButton = true;
        if (frame && (mode != CFrameWidget::MODE_TILED_VIEW)
                && tools.indexOf(tool) != -1) {
            frame->push();
            emit modified();
        }
        break;

    default:
        break;
    }

    setFocus();
    if ((m_mouse.lButton | m_mouse.rButton) &&
         (m_mouse.x >= 0 && m_mouse.y >= 0
                && (m_widget->m_mode != CFrameWidget::MODE_TILED_VIEW))) {
//        handleTool();
        emit toolHandler(m_mouse.x, m_mouse.y, m_mouse.lButton, m_mouse.rButton);
    }
}

void CFrameScroll::mouseReleaseEvent(QMouseEvent * event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        m_mouse.lButton = false;
        break;
    case Qt::RightButton:
        m_mouse.rButton = false;
        break;

    default:
        break;
    }
    setFocus();

    /*if (m_dots->getSize()) {
        m_dots->flush();
    }*/
    emit mouseUp();
}

void CFrameScroll::mouseMoveEvent(QMouseEvent * event)
{

    // MouseMove isn't called unless a button is pressed
    CFrameWidget *widget = static_cast<CFrameWidget *>(viewport());
    int gridSize = widget->m_zoom;
    QSize sz = size();
    int xx = -1;
    int yy = -1;
    int x = -1;
    int y = -1;
    CFrame * frame = widget->m_frame;
    if (frame){
        int width = frame->len() * gridSize;
        int height = frame->hei() * gridSize;
        if (event->x()>=0 && event->y()>=0) {
            xx = (event->x() + gridSize * horizontalScrollBar()->value()) ;
            yy = (event->y() + gridSize * verticalScrollBar()->value());
            if (xx < width && yy < height) {
                x = xx / gridSize;
                y = yy / gridSize;
            }
        }

        // Test if mouse in within valid coordonates
        if (event->x() >= sz.width()
                 ||event->y() >= sz.height() ) {
            setCursor(Qt::ArrowCursor);
            return;
        }
        m_mouse.oldX = m_mouse.x;
        m_mouse.oldY = m_mouse.y;
        m_mouse.x = x;
        m_mouse.y = y;
    }

    QString s = "";
    if (x < 0 || y < 0) {
        setCursor(Qt::ForbiddenCursor);
    } else {
        if (widget->m_frame) {
            uint rgb = widget->m_frame->at(x,y);
            char hex[12];
            int red = rgb & 0xff;
            int green = (rgb >> 8) & 0xff;
            int blue = (rgb >> 16) & 0xff;
            int alpha = (rgb >> 24) & 0xff;
            sprintf(hex, "%.2X%.2X%.2X:%.2X", red, green, blue, alpha);
            s = QString("x=%1 y=%2 (#%3)").arg(x).arg(y).arg(hex);
        }
        emit cursorChanged();
        if ((m_mouse.lButton | m_mouse.rButton) &&
             (m_mouse.x >= 0 && m_mouse.y >= 0
                    && (m_widget->m_mode != CFrameWidget::MODE_TILED_VIEW))) {
    //        handleTool();
            emit toolHandler(m_mouse.x, m_mouse.y, m_mouse.lButton, m_mouse.rButton);
        }
    }
    emit statusUpdate(0,s);
}

void CFrameScroll::wheelEvent(QWheelEvent *)
{
  //  qDebug("wheelEvent");
}

void CFrameScroll::mouseDoubleClickEvent(QMouseEvent * )
{
//    qDebug("mouseDoubleClickEvent");
}


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

#include "previewscroll.h"
#include "previewwidget.h"
#include <QScrollBar>
#include <QMouseEvent>

CPreviewScroll::CPreviewScroll(QWidget *parent):
    QAbstractScrollArea(parent)
{
    m_widget = new CPreviewWidget(this);
    setViewport(m_widget);
    // set view attributes
    setAttribute(Qt::WA_MouseTracking);
    setMouseTracking(true);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFrameShape(QFrame::NoFrame);
    connect(m_widget, SIGNAL(scrollbarAdjusted()), this, SLOT(updateScrollbars()));
    connect(this, SIGNAL(zoomChanged(int)), m_widget, SLOT(setZoom(int)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), m_widget, SLOT(setMX(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), m_widget, SLOT(setMY(int)));
}

void CPreviewScroll::resizeEvent(QResizeEvent * event)
{
    CPreviewWidget * glw = dynamic_cast<CPreviewWidget *>(viewport());
    glw->resizeEvent(event);
    updateScrollbars();
}

void CPreviewScroll::paintEvent(QPaintEvent *event)
{
    CPreviewWidget * glw = dynamic_cast<CPreviewWidget *>(viewport());
    glw->paintEvent(event);
}

void CPreviewScroll::updateScrollbars()
{
    CPreviewWidget *widget = m_widget;
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

void CPreviewScroll::mouseReleaseEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    m_mouse.oldX = -1;
    m_mouse.oldY = -1;
    m_mouse.x = -1;
    m_mouse.y = -1;
}

void CPreviewScroll::mousePressEvent(QMouseEvent * event)
{
    m_mouse.oldX = m_mouse.x;
    m_mouse.oldY = m_mouse.y;
    m_mouse.x = event->x();
    m_mouse.y = event->y();
    m_mouse.lButton = false;
    m_mouse.rButton = false;
    switch (event->button())
    {
    case Qt::LeftButton:
        m_mouse.lButton = true;
        break;

    case Qt::RightButton:
        m_mouse.rButton = true;
        break;
    default:
        break;
    }

    if (m_mouse.oldX != m_mouse.x
            || m_mouse.oldY != m_mouse.y) {

        int zoom = m_widget->m_zoom;
        if (m_mouse.lButton) {
            ++zoom;
            zoom = std::min(10, zoom);
        }

        if (m_mouse.rButton) {
            --zoom;
            zoom = std::max(1, zoom);
        }
        emit zoomChanged(zoom);
        update();
    }
}


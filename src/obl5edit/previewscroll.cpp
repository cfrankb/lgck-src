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
   // qDebug("CFrameScroll::updateScrollbars()");
    CPreviewWidget *widget = m_widget;
    QSize sz = size();
    CFrame *frame = widget->m_frame;
    int zoom = widget->m_zoom;

    if (!frame || sz.width() >= frame->m_nLen * zoom) {
        horizontalScrollBar()->setRange(0, 0);
    } else {
        horizontalScrollBar()->setRange(0, (frame->m_nLen*zoom - sz.width())/ zoom + 1);
    }

    if (!frame || sz.height() >= frame->m_nHei *zoom) {
        verticalScrollBar()->setRange(0, 0);
    } else {
        verticalScrollBar()->setRange(0, (frame->m_nHei*zoom - sz.height()) / zoom + 1);
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
    case Qt::MidButton:
        m_mouse.rButton = true;
        break;
    default:
        break;
    }

    if (m_mouse.oldX != m_mouse.x
            || m_mouse.oldY != m_mouse.y) {

        int zoom = m_widget->m_zoom;
        if (m_mouse.lButton) {
            zoom = std::min(10, ++zoom);
        }

        if (m_mouse.rButton) {
            zoom = std::max(1, --zoom);
        }
        //qDebug("zoom:%d", m_zoom);
        //changeFrame(m_frame);
      //  m_scene->update();
        qDebug("dseewewwe");
        emit zoomChanged(zoom);
        update();
    }
}


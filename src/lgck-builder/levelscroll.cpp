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
#include "levelscroll.h"
#include "levelviewgl.h"
#include <QScrollBar>
#include "Game.h"
#include <QMouseEvent>
#include <algorithm>
#include <QKeyEvent>
#include <QMimeData>
#include <QTime>
#include <QCursor>
#include <QPixmap>
#include "Level.h"
#include "Layer.h"
#include "Const.h"
#include "Path.h"
#include "PathBlock.h"
#include "Frame.h"
#include "Selection.h"
#include "inputs/qt/kt_qt.h"

constexpr int MAX_PIXEL = 0x7ff8;
constexpr int MARGIN = 32;
constexpr int FAST_SCROLL = 32;
constexpr int OBJ_SCROLL = 16;
constexpr int PATH_SCROLL = 8;
constexpr int MOUSE_POS_MASK = 0xfffffff8;
constexpr int DEFAULT_GRID_SIZE = 32;

CLevelScroll::CLevelScroll(QWidget *parent, CGame *game) :
        QAbstractScrollArea(parent)
{
    // create viewport
    CLevelViewGL *widget = new CLevelViewGL(this, game);
    setViewport(widget);
    // connect with widget
    connect(widget, SIGNAL(gameModeStateResync(bool&)),
        this, SLOT(getGameMode(bool&)));
    connect(widget, SIGNAL(scrollStatusResync(int&,int&)),
        this, SLOT(scrollStatus(int&,int&)));
    connect(widget, SIGNAL(whereIsMouse(int&,int&,int&,int&)),
        this, SLOT(mousePos(int&,int&,int&,int&)));
    // set view attributes
    setAttribute(Qt::WA_MouseTracking);
    setMouseTracking(true);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFrameShape(QFrame::NoFrame);
    initMouse();
    m_editPath = false;
    m_game = game;
    m_zoom = 1;
    m_ticks = 0;
    m_gameMode = false;
    m_paintSprite = false;
    m_gridSize = DEFAULT_GRID_SIZE;
    m_bErase = false;
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(mxChanged(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(myChanged(int)));
    connect(this, SIGNAL(setMX(int)), horizontalScrollBar(), SLOT(setValue(int)));
    connect(this, SIGNAL(setMY(int)), verticalScrollBar(), SLOT(setValue(int)));
    connect(this, & CLevelScroll::triggerMouseEvent,
            widget, & CLevelViewGL::mouseClick);
}

void CLevelScroll::resizeEvent(QResizeEvent * event)
{
    CLevelViewGL * glw = dynamic_cast<CLevelViewGL *>(viewport());
    glw->resizeEvent(event);
}

void CLevelScroll::paintEvent(QPaintEvent *event)
{
    CLevelViewGL * glw = dynamic_cast<CLevelViewGL *>(viewport());
    glw->paintEvent(event);
}

void CLevelScroll::updateScrollbars(int w, int h)
{
    horizontalScrollBar()->setPageStep(m_zoom);
    verticalScrollBar()->setPageStep(m_zoom);

    QSize sz = viewport()->size();
    if (sz.width() >= w) {
        horizontalScrollBar()->setRange(0, 0);
    } else {
        horizontalScrollBar()->setRange(0, (w - sz.width())/m_zoom + 1);
    }
    if (sz.height() >= h) {
        verticalScrollBar()->setRange(0, 0);
    } else {
        verticalScrollBar()->setRange(0, (h - sz.height())/m_zoom + 1);
    }
}

int CLevelScroll::gridMask()
{
    return INT_MAX - m_gridSize + 1;
}

void CLevelScroll::insideTile(int x1, int y1, int x2, int y2, CSelection & selection)
{
    CLevel & level = m_game->getCurrentLevel();
    CLayer & layer = * level.getCurrentLayer();
    for (int i=0; i < layer.getSize(); ++i) {
        CLevelEntry & entry = layer[i];
        if ((entry.m_nX >= x1 && entry.m_nX < x2)
                && (entry.m_nY >= y1 && entry.m_nY < y2)
                && (entry.m_nActionMask & m_skillFilters)) {
            selection.addEntry(entry, i);
        }
    }
}

void CLevelScroll::paintSprite()
{
    CLevel & level = m_game->getCurrentLevel();
    CLayer & layer = * level.getCurrentLayer();
    CSelection selection;
    int x = (level.m_mx + m_mouse.x) & gridMask();
    int y = (level.m_my + m_mouse.y) & gridMask();
    insideTile(x, y, x + m_gridSize, y + m_gridSize, selection);
    layer.clearSelection();
    if (selection.getSize()) {
        layer.select(selection);
        layer.removeSelectedSprites();
    }
    if (m_proto > 0 && !m_bErase) {
        m_entry.m_nX = x;
        m_entry.m_nY = y;
        layer.add(m_entry);
    }
    m_game->setDirty( true );
}

void CLevelScroll::mousePressEvent(QMouseEvent * event)
{
    setCursor();
    if (!m_game || !m_game->getSize()) {
        return;
    }

    //qDebug("mousePressEvent");
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

    auto const pos = event->pos();
    m_mouse.x = pos.x() & MOUSE_POS_MASK;
    m_mouse.y = pos.y() & MOUSE_POS_MASK;
    const bool validLevel = m_game && m_game->getSize();
    if (this->isGameMode()) {
        if (m_game) {
            emit triggerMouseEvent(m_mouse.x, m_mouse.y, event->button());
        }
    } else if (validLevel && m_mouse.lButton && m_paintSprite) {
        paintSprite();
    } else if (validLevel && m_mouse.lButton && !m_editPath) {
        CGameFile & gf = *m_game;
        CLevel & level = m_game->getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        int oldCurrEntry = layer.getSelectionIndex(0);
        int selCount = layer.getSelectionSize();
        int index = gf.whoIs(level, m_mouse.x + level.m_mx, m_mouse.y + level.m_my, m_skillFilters);
        if (index != -1) {
            if (layer.isInSelection(index)) {
                if (m_game->testKey(lgck::Key::LControl)) {
                    layer.removeFromSelection( index );
                }
            } else {
                if (m_game->testKey(lgck::Key::LControl)) {
                    layer.select( index );
                } else {
                    layer.selectSingle( index );
                }
            }
        } else {
            if (!m_game->testKey(lgck::Key::LControl)) {
                layer.clearSelection();
            }
        }

        if (oldCurrEntry != layer.getSelectionIndex(0)
                || selCount != layer.getSelectionSize()) {
            //qDebug("menu changed");
            emit menuChanged();
        }

        // if no selection
        if (!layer.getSelectionSize()) {
            const auto pos = event->pos();
            m_mouse.destX = m_mouse.orgX = pos.x() ;
            m_mouse.destY = m_mouse.orgY = pos.y() ;
            return;
        }
        m_mouse.orgX = m_mouse.orgY = -1;
        if (m_bErase) {
            layer.removeSelectedSprites();
        }
    }
    //setFocus();
}

void CLevelScroll::mouseReleaseEvent(QMouseEvent * event)
{
    if (!m_game || !m_game->getSize()) {
        return;
    }

    setCursor();
    //qDebug("mouseReleaseEvent");
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
    //setFocus();
    if (!m_mouse.lButton) {
        m_mouse.drag = false;
    }
    if (!this->isGameMode() && m_game && m_game->getSize()
            && !m_editPath && !m_paintSprite) {
        CLevel & level = m_game->getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        if (!layer.getSelectionSize()
                && !m_mouse.lButton
                && m_mouse.orgX != -1
                && m_mouse.orgY != -1) {
            QPoint pointS (m_mouse.orgX, m_mouse.orgY);
            QPoint pointE (m_mouse.destX, m_mouse.destY);
            int x1 = std::min(pointS.x(), pointE.x()) + level.m_mx;
            int x2 = std::max(pointS.x(), pointE.x()) + level.m_mx;
            int y1 = std::min(pointS.y(), pointE.y()) + level.m_my;
            int y2 = std::max(pointS.y(), pointE.y()) + level.m_my;
            CSelection selection;
            select(x1, y1, x2, y2, selection);
            m_mouse.orgX = -1;
            m_mouse.orgY = -1;
            if (selection.getSize()) {
                emit menuChanged();
                layer.select(selection);
            }
            if (m_bErase) {
                layer.removeSelectedSprites();
            }
        }
    }
}

void CLevelScroll::mouseMoveEvent(QMouseEvent *event)
{
    setCursor();
    if (!m_game || !m_game->getSize()) {
        return;
    }
    //qDebug("mouseMoveEvent");
    QSize sz = size();
    m_mouse.oldX = m_mouse.x;
    m_mouse.oldY = m_mouse.y;
    const auto pos = event->pos();
    m_mouse.x = pos.x() & MOUSE_POS_MASK;
    m_mouse.y = pos.y() & MOUSE_POS_MASK;
    if (!this->isGameMode() && !m_editPath && m_paintSprite) {
        if (m_mouse.lButton &&
                ((m_mouse.oldX != m_mouse.x) || (m_mouse.oldY != m_mouse.y))) {
            paintSprite();
        }
    } else if (!this->isGameMode() && !m_editPath) {
        QString s = "";
        if (m_game->getSize()) {
            if (m_mouse.x >= 0 && m_mouse.y >= 0) {
               s = tr("x = %1 y = %2").arg(m_mouse.x).arg(m_mouse.y);
            }
        } else {
            s = tr("No level in this project. Go to Build, Create New Level to add one.");
        }
        emit statusChanged(0, s);
        if (m_game && m_game->getSize()) {
            CGame & gf = *m_game;
            CLevel & level = m_game->getCurrentLevel();
            CLayer & layer = * level.getCurrentLayer();
            if (layer.getSelectionSize()) {
                for (int i=0; i < layer.getSelectionSize(); ++i) {
                    CLevelEntry & entry = layer.getSelection(i);
                    if (m_mouse.drag && (m_mouse.x < 0 || m_mouse.y < 0
                       || m_mouse.x >= sz.width() || m_mouse.y >= sz.height())) {
                        m_mouse.drag = false;
                        layer.clearSelection();
                        break;
                    }
                    if (m_mouse.drag) {
                        int tX = m_mouse.x - m_mouse.oldX;
                        int tY = m_mouse.y - m_mouse.oldY;
                        if ( (tX | tY) ){
                            gf.setDirty( true );
                        }
                        entry.m_nX += tX;
                        entry.m_nY += tY;
                    }
                    if (m_mouse.lButton) {
                        m_mouse.drag = true;
                    }

                    if (entry.m_nX < 0) {
                        entry.m_nX = 0;
                        m_mouse.drag = false;
                    }

                    if (entry.m_nY < 0) {
                        entry.m_nY = 0;
                        m_mouse.drag = false;
                    }
                }
            } else {
                if (!layer.getSelectionSize()
                        && m_mouse.lButton
                        && m_mouse.orgX != -1
                        && m_mouse.orgY != -1) {
                    const auto pos = event->pos();
                    m_mouse.destX = pos.x();
                    m_mouse.destY = pos.y();
                }
            }
        }
    }
}

void CLevelScroll::editPath()
{
    QSize sz = size();
    emit statusChanged(0, tr("Editing Path: Use CURSORS to move, SPACE to Save or ESC to Cancel"));
    ++m_ticks;
    //setFocus();
    if (m_game && m_game->getSize()) {
        CGame & gf = *m_game;
        CLevel & level = m_game->getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        bool endPath = false;
        // if single selection
        if (layer.getSelectionSize() == 1) {
            CLevelEntry & entry = layer.getSelection(0);
            CFrame & frame = gf.toFrame(entry);
            CPathBlock & paths = *(gf.getPaths());
            CPath & path = paths[entry.m_path];
            int scroll = PATH_SCROLL;
            // move up
            if (m_game->testKey(lgck::Key::Up) && entry.m_nY) {
                if (entry.m_nY > scroll) {
                    entry.m_nY -= scroll;
                    path.add(CGame::UP);
                } else {
                    entry.m_nY = 0;
                }
                if (entry.m_nY < level.m_my) {
                    level.m_my = entry.m_nY;
                }
                gf.setDirty(true);
            }
            // move down
            if (m_game->testKey(lgck::Key::Down)) {
                entry.m_nY += scroll;
                gf.setDirty(true);
                path.add(CGame::DOWN);
            }
            if (entry.m_nY + frame.hei() > level.m_my + sz.height() - scroll) {
                level.m_my += scroll;
            }

            // move left
            if (m_game->testKey(lgck::Key::Left) && entry.m_nX) {
                if (entry.m_nX > scroll) {
                    entry.m_nX -= scroll;
                    path.add(CGame::LEFT);
                } else {
                    entry.m_nX = 0;
                }
                if (entry.m_nX < level.m_mx) {
                    level.m_mx = entry.m_nX;
                }
                gf.setDirty(true);
            }

            // move right
            if (m_game->testKey(lgck::Key::Right)) {
                entry.m_nX += scroll;
                path.add(CGame::RIGHT);
                gf.setDirty(true);
            }

            if (m_game->testKey(lgck::Key::Back)
                    && path.getSize()) {
                switch (path[path.getSize()-1]) {
                case CGame::UP:
                    entry.m_nY += scroll;
                    break;
                case CGame::DOWN:
                    entry.m_nY -= scroll;
                    break;
                case CGame::LEFT:
                    entry.m_nX += scroll;
                    break;
                case CGame::RIGHT:
                    entry.m_nX -= scroll;
                }
                path.removeAt(path.getSize()-1);
                gf.setDirty(true);
            }

            if (entry.m_nX + frame.len() > level.m_mx + sz.width()) {
                level.m_mx += scroll;
            }

            // de-select object
            if (m_game->testKey(lgck::Key::Space)
                    || m_game->testKey(lgck::Key::Escape)) {
                layer.clearSelection();
                emit pathEnded();
                emit menuChanged();
                endPath = true;
                emit statusChanged(0, "");
            }
            if (!endPath) {
                QString s;
                if (!layer.isMultiSelection()) {
                    s = QString(tr("Sprite: x = %1 y = %2")).arg(entry.m_nX).arg(entry.m_nY);
                } else {
                    s = tr("multiple selection");
                }
                emit statusChanged(1, s);
                s = QString(tr("moves: %1").arg(path.getSize()));
                emit statusChanged(2, s);
            }
        }
    }
}

void CLevelScroll::initMouse()
{
    m_mouse.drag = m_mouse.mButton = m_mouse.lButton = m_mouse.rButton = false;
    m_mouse.orgX = m_mouse.orgY = m_mouse.oldX = m_mouse.oldY = m_mouse.x = m_mouse.y = -1;
}

/////////////////////////////////////////////////////////////////////
// Drag & Drop

void CLevelScroll::entryFromProto(const int protoId, CLevelEntry &entry)
{
    CProto & proto = m_game->toProto(protoId);
    entry.m_nProto = protoId;
    entry.m_nFrameNo = proto.m_nFrameNo;
    entry.m_nFrameSet = proto.m_nFrameSet;

    if (proto.getOption(CProto::OPTION_NO_TRIGGER_FLIP)) {
        entry.m_nActionMask = DIFFICULTY_ALL;
    } else {
        entry.m_nActionMask = DIFFICULTY_ALL | TRIGGER_HIDDEN ;
    }

    if (proto.getOption(CProto::OPTION_AUTO_GOAL)) {
        entry.m_nTriggerKey = TRIGGER_GOAL;
    }
}

void CLevelScroll::dropEvent(QDropEvent *event)
{
    QString t = event->mimeData()->text();
    QPoint pos = event->position().toPoint();
    if (event->source() && m_game->getSize()
            && t.mid(0, 5) == "LGCK:") {

        int offsetX = 0x10;
        int offsetY = 0x10;
        QString s = t.mid(6);
        bool ok;
        int protoId = s.toInt( &ok, 10 );

        CLevel & level = m_game->getCurrentLevel();
        CLevelEntry entry;
        entryFromProto(protoId, entry);
        entry.m_nX = level.m_mx + (pos.x() & 0xfff8) + offsetX;
        entry.m_nY = level.m_my + (pos.y() & 0xfff8) + offsetY;

        CLayer & layer = * level.getCurrentLayer();
        int size = layer.getSize();
        layer.add(entry);
        layer.selectSingle(size);
        m_game->setDirty( true );
       // event->setDropAction(Qt::CopyAction);
        //setFocus();
        emit menuChanged();
    }
}

void CLevelScroll::dragEnterEvent(QDragEnterEvent *event)
{
    CGame & gf = *m_game;
    // the drag event comes from this application
    // and we got a level to drop it on
    if (event->source() && gf.getSize() && !isGameMode()
            && !m_editPath) {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
        event->setAccepted(true);
    } else {
         event->setAccepted(false);
    }
}

void CLevelScroll::dragMoveEvent(QDragMoveEvent *event)
{
    CGame & gf = *m_game;
    // the drag event comes from this application
    // and we got a level to drop it on
    if (event->source() && gf.getSize() && !isGameMode()
            && !m_editPath) {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
    }
}

void CLevelScroll::enterEditPath()
{
    m_editPath = true;
}

void CLevelScroll::leaveEditPath()
{
    m_editPath = false;
}

void CLevelScroll::scrollFastMargin(int & mx, int & my)
{
    const QSize sz = size();
    // this is conditional to the mouse
    // being inside the view's margin
    if (m_mouse.lButton) {
        if (m_mouse.x < MARGIN && m_mouse.x >= 0) {
            if (mx > FAST_SCROLL) {
                mx -= FAST_SCROLL;
            } else {
                mx = 0;
            }
        }

        if (m_mouse.x >= sz.width() - MARGIN
            && m_mouse.x < sz.width()) {
            mx += FAST_SCROLL;
        }

        if (m_mouse.y < MARGIN && m_mouse.y >= 0) {
            if (my > FAST_SCROLL) {
                my -= FAST_SCROLL;
            } else {
                my = 0;
            }
        }

        if (m_mouse.y >= sz.height() - MARGIN
            && m_mouse.y < sz.height()) {
            my += FAST_SCROLL;
        }
    }
}

void CLevelScroll::scrollFastByKeys(int & mx, int & my)
{
    // scroll using keyboard arrow keys
    if (m_game->testKey(lgck::Key::Up) && my) {
        if (my > FAST_SCROLL) {
            my -= FAST_SCROLL;
        } else {
            my = 0;
        }
    }

    if (m_game->testKey(lgck::Key::Down)) {
        my += FAST_SCROLL;
    }

    if (m_game->testKey(lgck::Key::Left) && mx) {
        if (mx > FAST_SCROLL) {
            mx -= FAST_SCROLL;
        } else {
            mx = 0;
        }
    }

    if (m_game->testKey(lgck::Key::Right)) {
        mx += FAST_SCROLL;
    }
}

void CLevelScroll::slowPrecisionPlacement(int & mx, int & my, CLevelEntry & entry)
{
    const QSize sz = size();
    CGame & gf = *m_game;
    CFrameSet & filter = gf.toFrameSet(entry.m_nFrameSet);
    CFrame & frame = gf.toFrame(entry);

    // change frame ++
    if (m_game->testKey(lgck::Key::RBracket) ) {
        if (entry.m_nFrameNo < filter.getSize() -1){
            ++entry.m_nFrameNo;
            gf.setDirty(true);
        }
    }

    // change frame --
    if (m_game->testKey(lgck::Key::LBracket)) {
        if (entry.m_nFrameNo >0){
            --entry.m_nFrameNo;
            gf.setDirty(true);
        }
    }

    // precision scrolling
    int scroll = OBJ_SCROLL;
    if (m_game->testKey(lgck::Key::LShift)) {
        scroll /= 2;
    }

    // move up
    if (m_game->testKey(lgck::Key::Up) && entry.m_nY) {
        if (entry.m_nY > scroll)
            entry.m_nY -= scroll;
        else
            entry.m_nY = 0;

        if (entry.m_nY < my) {
            my = entry.m_nY;
        }
        gf.setDirty(true);
    }

    // move down
    if (m_game->testKey(lgck::Key::Down)) {
        entry.m_nY += scroll;
        gf.setDirty(true);
    }

    if (entry.m_nY + frame.hei() > my + sz.height() - scroll) {
        my += scroll;
    }

    // move left
    if (m_game->testKey(lgck::Key::Left) && entry.m_nX) {
        if (entry.m_nX > scroll)
            entry.m_nX -= scroll;
        else
            entry.m_nX = 0;

        if (entry.m_nX < mx) {
            mx = entry.m_nX;
        }
        gf.setDirty(true);
    }

    // move right
    if (m_game->testKey(lgck::Key::Right)) {
        entry.m_nX += scroll;
        gf.setDirty(true);
    }

    if (entry.m_nX + frame.len() > mx + sz.width()) {
        mx += scroll;
    }
}

void CLevelScroll::alignEntry(CLevelEntry & entry)
{
    CGame & gf = *m_game;
    if (entry.m_nX & 15 ||
            entry.m_nY & 15) {
        gf.setDirty(true);
    }

    entry.m_nX &= 0xfff0;
    entry.m_nY &= 0xfff0;
}

void CLevelScroll::changeOriginFromEntry(int & mx, int & my, const CLevelEntry& entry)
{
    if (entry.m_nY < my) {
        if (my < FAST_SCROLL) {
            my = 0;
        } else {
            my -= FAST_SCROLL;
        }
    }

    if (entry.m_nX < mx) {
        if (mx < FAST_SCROLL) {
            mx = 0;
        } else {
            mx -= FAST_SCROLL;
        }
    }
}

void CLevelScroll::eventHandler()
{
    int mx = horizontalScrollBar()->value();
    int my = verticalScrollBar()->value();
    int oldMX = mx;
    int oldMY = my;
    ++ m_ticks;
    //setFocus();
    if (m_game && m_game->getSize()) {
        CLevel & level = m_game->getCurrentLevel();
        CLayer & layer = * level.getCurrentLayer();
        // scroll the screen when the mouse is
        // within the page margin return home
        if (m_game->testKey(lgck::Key::Home)) {
            layer.clearSelection();
            my = mx = 0;
        }
        scrollFastMargin(mx, my);

        // compute screen translations for the selected
        // sprites
        int tX = mx - oldMX;
        int tY = my - oldMY;

        // if no sprite is selected
        if (layer.getSelectionSize() == 0) {
            scrollFastByKeys(mx, my);
            QString s = QString(tr("topX = %1 topY = %2")).arg(mx).arg(my);
            emit statusChanged(1, s);
        } else {
            // manage the selection
            CLayer & layer = * level.getCurrentLayer();
            for (int z = 0; z < layer.getSelectionSize(); ++z) {
                CLevelEntry & entry = layer.getSelection(z);
                changeOriginFromEntry(mx, my, entry);

                // align sprite
                if (m_game->testKey(lgck::Key::LControl)
                        && m_game->testKey(lgck::Key::A)) {
                    alignEntry(entry);
                }

                // slowdown editor for precision placement
                if (m_ticks % 2 == 0) {
                    slowPrecisionPlacement(mx, my, entry);
                }

                // de-select object
                if (m_game->testKey(lgck::Key::Space)
                        || m_game->testKey(lgck::Key::Escape)) {
                    layer.clearSelection();
                    emit menuChanged();
                }
            }
            QString s;
            if (!layer.isMultiSelection()) {
                CLevelEntry & entry = layer.getSelection(0);
                s = QString(tr("Sprite: x = %1 y = %2")).arg(entry.m_nX).arg(entry.m_nY);
            } else {
                s = tr("Multiple selections");
            }
            emit statusChanged(1, s);
        }

        // ensures that screen is within valid
        // worldview coordonates

        const QSize sz = size();
        int maxMX = MAX_PIXEL - sz.width();
        int maxMY = MAX_PIXEL - sz.height();

        if (mx > maxMX) {
            mx = maxMX ;
        }

        if (my > maxMY) {
            my = maxMY ;
        }

        if (layer.getSelectionSize()
            && m_mouse.drag) {
            //TODO: revisit this code
            for (int i = 0; i < layer.getSelectionSize(); ++i) {
                CLevelEntry & entry = layer.getSelection(i);
                entry.m_nX = std::max(entry.m_nX + tX, 0);
                entry.m_nY = std::max(entry.m_nY + tY, 0);
            }
        }

        level.m_mx = mx;
        level.m_my = my;
        if (mx != oldMX) {
            emit setMX(mx);
        }
        if (my != oldMY) {
            emit setMY(my);
        }
    }
}

void CLevelScroll::select(int x1, int y1, int x2, int y2, CSelection & selection)
{
    CLevel & level = m_game->getCurrentLevel();
    CLayer & layer = * level.getCurrentLayer();
    for (int i=0; i < layer.getSize(); ++i) {
        CLevelEntry & entry = layer[i];
        CFrame & frame = m_game->toFrame(entry);
        if ( (entry.m_nActionMask & m_skillFilters)
             && ((entry.m_nX >= x1 && entry.m_nX <= x2)
              || (entry.m_nX <= x1 && entry.m_nX + frame.len() >= x1)
              || (entry.m_nX <= x2 && entry.m_nX + frame.len() >= x2))
                &&
                ((entry.m_nY >= y1 && entry.m_nY <= y2)
                 || (entry.m_nY <= y1 && entry.m_nY + frame.hei() >= y1)
                 || (entry.m_nY <= y2 && entry.m_nY + frame.hei() >= y2))
                ) {
            selection.addEntry(entry, i);
        }
    }
}

void CLevelScroll::changeLevel(int i)
{
    qDebug("IN  changeLevel(int) %d **************", i);
    CLevel * level = nullptr;
    if (m_game && m_game->getSize()) {
        level = & m_game->getCurrentLevel();
    }
    updateScrollbars(level ? MAX_PIXEL : 1, level ? MAX_PIXEL : 1);
    update();
    if (level) {
        emit setMX(level->m_mx);
        emit setMY(level->m_my);
    }
    qDebug("OUT  changeLevel(int) **************");
}

void CLevelScroll::showScrollbars(bool show)
{
    updateScrollbars(show ? MAX_PIXEL : 1, show ? MAX_PIXEL : 1);
    update();
}

void CLevelScroll::mousePos(int &orgX, int &orgY, int &destX, int &destY)
{
    orgX = m_mouse.orgX;
    orgY = m_mouse.orgY;
    destX = m_mouse.destX;
    destY = m_mouse.destY;
}

void CLevelScroll::mxChanged(int mx)
{
    if (m_game && m_game->getSize()) {
        CLevel & level = m_game->getCurrentLevel();
        level.m_mx = mx;
    }
}

void CLevelScroll::myChanged(int my)
{
    if (m_game && m_game->getSize()) {
        CLevel & level = m_game->getCurrentLevel();
        level.m_my = my;
    }
}

void CLevelScroll::scrollStatus(int &mx, int &my)
{
    mx = horizontalScrollBar()->value();
    my = verticalScrollBar()->value();
}

void CLevelScroll::setGameMode(bool gm)
{
    m_gameMode = gm;
}

void CLevelScroll::getGameMode(bool & gm)
{
    gm = m_gameMode;
}

void CLevelScroll::keyReflector(const QKeyEvent *event, bool pressed)
{
    emit keyChanged(CKeyTranslator::translate(event->key()), (int) pressed);
}

void CLevelScroll::keyPressEvent ( QKeyEvent * event )
{
    keyReflector(event, true);
}

void CLevelScroll::keyReleaseEvent ( QKeyEvent * event )
{
    keyReflector(event, false);
}

void CLevelScroll::getGLInfo(QString &vendor, QString &renderer, QString &version, QString &extensions)
{
    vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
}

void CLevelScroll::setPaintState(bool state)
{
    m_paintSprite = state;
    qDebug("paintState %d", state);
}

void CLevelScroll::changeProto(int proto)
{
    m_proto = proto;
    entryFromProto(m_proto, m_entry);
    qDebug("changeProto: %d", m_proto);
}

void CLevelScroll::changeProtoFrame(int proto, int frameId)
{
    m_proto = proto;
    entryFromProto(m_proto, m_entry);
    m_entry.m_nFrameNo = frameId;
    qDebug("changeProto: %d - frameId: %d", m_proto, frameId);
}

void CLevelScroll::setGridSize(int size)
{
    m_gridSize = size;
}

void CLevelScroll::setEraserState(bool state)
{
    qDebug("erase state: %d", state);
    m_bErase = state;
}

void CLevelScroll::setCursor()
{
    if (m_gameMode) {
        viewport()->setCursor(Qt::ArrowCursor);
    } else if (!m_game || !m_game->getSize()) {
        viewport()->setCursor(Qt::ForbiddenCursor);
    } else if (m_editPath) {
        QCursor c = QCursor(QPixmap(":/images/pd/footprint.png"));
        viewport()->setCursor(c);
    } else if (m_bErase) {
        QCursor c = QCursor(QPixmap(":/images/icons8-eraser-50.png"), 20, 45);
        viewport()->setCursor(c);
    } else if (m_paintSprite) {
        QCursor c = QCursor(QPixmap(":/images/sketchpntbrush.png"), 10, 31);
        viewport()->setCursor(c);
    } else {
        viewport()->setCursor(Qt::ArrowCursor);
    }
}

void CLevelScroll::setSkillFilters(int flags)
{
    m_skillFilters = flags;
}

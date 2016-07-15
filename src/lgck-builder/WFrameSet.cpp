/*
    LGCK Builder GUI
    Copyright (C) 1999, 2012  Francois Blanchette

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

#include "WFrameSet.h"
#include "../shared/stdafx.h"
#include "../shared/FrameSet.h"
#include "../shared/Frame.h"
#include "../shared/FileWrap.h"
#include "../shared/qtgui/cheat.h"
#include "DlgGotoLevel.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>

CWFrameSet::CWFrameSet(QWidget *parent)
    : QTreeWidget(parent)
{
}

void CWFrameSet::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_startPos = event->pos();
    }

    if (event->button()) {
        emit clicked();
    }

    QTreeWidget::mousePressEvent(event);
}

void CWFrameSet::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void CWFrameSet::contextMenuEvent(QContextMenuEvent* event)
{
    const QPoint pos = event->pos();
    QTreeWidgetItem * item = itemAt(pos);
    if (item) {
        // create pop-up
        QMenu menu(this);

        // Make sure this item is selected
        setCurrentItem( item );
        int index = currentIndex().row();
        CFrame *frame = (*m_frameSet)[index];

        QAction *actionDelete = new QAction(tr("Remove"), &menu);
        menu.addAction(actionDelete);

        QAction *actionEnlarge = new QAction(tr("Enlarge x2"), &menu);
        menu.addAction(actionEnlarge);

        if (frame->m_nLen > 16 || frame->m_nHei > 16) {
            QAction *actionSlice = new QAction(tr("Slice Image"), &menu);
            menu.addAction(actionSlice);
            connect(actionSlice, SIGNAL(triggered()),
                    this, SLOT(sliceImage()));
        }

        if (!frame->hasTransparency()) {
            QAction *actionTransparent = new QAction(tr("Set Transparent"), &menu);
            menu.addAction(actionTransparent);
            connect(actionTransparent, SIGNAL(triggered()),
                    this, SLOT(setTransparency()));
        }

        QAction *actionRemoveAll = new QAction(tr("Remove All"), &menu);
        menu.addAction(actionRemoveAll);

        connect(actionDelete, SIGNAL(triggered()),
                this, SLOT(deleteImage()));

        connect(actionRemoveAll, SIGNAL(triggered()),
                this, SLOT(removeAll()));

        connect(actionEnlarge, SIGNAL(triggered()),
                this, SLOT(enlargeImage()));

        menu.exec(mapToGlobal(pos));
    } else {
        QMenu menu(this);

        QAction *actionAdd = new QAction(tr("Add Images"), &menu);
        menu.addAction(actionAdd);
        connect(actionAdd, SIGNAL(triggered()), this, SLOT(addImages()));

        QAction *actionRemoveAll = new QAction(tr("Remove All"), &menu);
        menu.addAction(actionRemoveAll);
        connect(actionRemoveAll, SIGNAL(triggered()), this, SLOT(removeAll()));

        menu.exec(mapToGlobal(pos));
    }
}

/////////////////////////////////////////////////////////////////////
// Drag & Drop

void CWFrameSet::dropEvent(QDropEvent *event)
{
    QList<QUrl> urlList;
    QString fName;
    QFileInfo info;

    if (event->mimeData()->hasUrls())
    {
        urlList = event->mimeData()->urls(); // returns list of QUrls

        // if just text was dropped, urlList is empty (size == 0)
        if ( urlList.size() > 0) // if at least one QUrl is present in list
        {
            for (int i=0; i < urlList.size(); ++i) {
                fName = urlList[i].toLocalFile(); // convert QUrl to local path
                info.setFile( fName ); // information about file
                if ( info.isFile() ) {
                    addFile( fName );
                    event->acceptProposedAction();
                }
            }
        }
    }

}

void CWFrameSet::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
    }
}

void CWFrameSet::dragEnterEvent(QDragEnterEvent *event)
{
    // accept just text/uri-list mime format
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void CWFrameSet::setFrameSet(CFrameSet *frameSet)
{
    m_frameSet = frameSet;
}

CFrameSet * CWFrameSet::getFrameSet()
{
   return (CFrameSet *)m_frameSet;
}

void CWFrameSet::addFrame(int index)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(0);
    updateIcon(item, index);
    addTopLevelItem(item);
}

void CWFrameSet::updateIcon(QTreeWidgetItem *item,  int j)
{
    UINT8 *png;
    int pngSize;
    (*m_frameSet)[j]->toPng(png, pngSize);

    QImage img;
    if (!img.loadFromData( png, pngSize )) {
        qDebug("failed to load png\n");
    }
    delete [] png;

    QPixmap pm = QPixmap::fromImage(img);
    QIcon icon;
    icon.addPixmap(pm, QIcon::Normal, QIcon::On);
    icon.actualSize(QSize(32,32));

    QString s = QString(tr("%1 x %2")) .arg( (*m_frameSet)[j]->m_nLen) . arg((*m_frameSet)[j]->m_nHei);
    item->setIcon(0, icon);
    item->setText(1, s);
}

void CWFrameSet::addFile( QString fileName )
{
    CFileWrap file;
    if (file.open( q2c(fileName) )) {
        CFrameSet images;
        char format[5];
        if (images.extract(file, format)) {
            for (int i = 0; i < images.getSize(); ++i) {
                int j = m_frameSet->getSize();
                m_frameSet->add(images[i]);
                addFrame(j);
            }

            if (images.getSize()) {
                emit countChanged();
            }

            images.removeAll();
        } else {
            QMessageBox::warning(this,"", tr("Unsupported file: %1").arg(fileName), QMessageBox::Ok );
        }

        file.close();

    } else {
        QMessageBox::warning(this,"", QString(tr("can't open `%1`\n")).arg(fileName), QMessageBox::Ok );
    }
}

void CWFrameSet::deleteImage()
{
    QModelIndex index = currentIndex();
    if (index.row() < 0) {
        QMessageBox::warning((QWidget*) parent(), "",
                             tr("This image cannot be deleted"),
                             QMessageBox::Ok);
        return;
    }

    QMessageBox::StandardButton
            ret = QMessageBox::warning((QWidget*) parent(), "", tr("Are you sure that you want to delete\n"
                                          "this image?"),
                                       QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        // remove object
        m_frameSet->removeAt( index.row() );

        // remove visual
        model()->removeRow( index.row() );        
        emit countChanged();
    }
}

void CWFrameSet::addImages()
{
    QString pngFilter = tr("PNG images (*.png)");
    QString oblFilter = tr("Object Blocks (*.obl5 *.obl)");
    QString allFilter = tr("All supported images (*.obl *.obl5 *.png)");
    static QString fileFilter = allFilter + ";;" + pngFilter + ";;" + oblFilter ;

    QString selectedFilter;

    QFileDialog dialog(this);
    //dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(fileFilter);
    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        selectedFilter = dialog.selectedNameFilter();
        if (fileNames.count()>0) {
            if (selectedFilter == allFilter) {
                fileFilter = allFilter + ";;" + oblFilter + ";;" + pngFilter;
            } else if (selectedFilter == oblFilter) {
                fileFilter = oblFilter + ";;" + pngFilter + ";;" + allFilter ;
            } else {
                fileFilter = pngFilter + ";;" + oblFilter + ";;" + allFilter;
            }

            for (int i=0; i < fileNames.count(); ++i) {
                addFile(fileNames.at(i));
            }
        }
    }
}

void CWFrameSet::setTransparency()
{
    int index = currentIndex().row();
    QMessageBox::StandardButton result =
            QMessageBox::warning((QWidget*)parent(),  "",  tr("Image #%1 doesn't have any transparent pixels.\n"
                                                              "Some system may simply mark the transparent color as the first pixel.\n"
                                                              "Do you want to use the top left pixel as the transparent color key?") .arg(index + 1),
                                 QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        (*m_frameSet)[index]->setTopPixelAsTranparency();
        QTreeWidgetItem * item = topLevelItem(index);
        updateIcon(item, index);
    }
}

void CWFrameSet::enlargeImage()
{
    int index = currentIndex().row();
    if (index != -1) {
        (*m_frameSet)[index]->enlarge();
        QTreeWidgetItem * item = topLevelItem(index);
        updateIcon(item, index);
    }
}

void CWFrameSet::removeAll()
{
    int count = model()->rowCount();
    model()->removeRows(0, count);
    m_frameSet->removeAll();
}

void CWFrameSet::sliceImage()
{
    int index = currentIndex().row();
    if (index != -1) {
        CDlgGotoLevel *dlg = new CDlgGotoLevel(this);
        dlg->setWindowTitle(tr("Split into pieces..."));
        int j = index;
        CFrame *frame = (*m_frameSet)[j];
        dlg->initSizes(std::max(frame->m_nLen, frame->m_nHei));
        dlg->setLevel( 0 );
        if (dlg->exec() == QDialog::Accepted) {
            int sizes[] = {
                16, 24, 32, 48, 64, 96, 128, 256
            };
            int pxSize = sizes[dlg->getLevel()];
            CFrameSet *frames = frame->split(pxSize);
            model()->removeRow(j);
            m_frameSet->removeAt(j);
            for (int i=0; i < frames->getSize();++i) {
                if (!(*frames)[i]->isEmpty()) {
                    m_frameSet->insertAt(j, (*frames)[i]);
                    QTreeWidgetItem *item = new QTreeWidgetItem(0);
                    updateIcon(item, j);
                    insertTopLevelItem(j, item);
                    ++j;
                } else {
                    delete (*frames)[i];
                }
            }
            frames->removeAll();
        }
        delete dlg;
    }
}

void CWFrameSet::reload()
{
    setAcceptDrops(true);
    setColumnCount( 2 );
    setEditTriggers(0);
    setWordWrap(false);
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    setColumnWidth(0, 32);
    setColumnWidth(1, 64);
    setIconSize(QSize(32,32));
    setLineWidth(48);
    setAllColumnsShowFocus(true);
    setBaseSize(32,32);
    setUniformRowHeights(false);
    setAlternatingRowColors(true);
    for (int i = 0; i < m_frameSet->getSize(); ++i){
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        updateIcon(item, i);
        addTopLevelItem(item);
    }
}

/*
    LGCK Builder GUI
    Copyright (C) 1999, 2016  Francois Blanchette

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

#include "ToolBoxDock.h"
#include "ui_ToolBoxDock.h"
#include "../shared/stdafx.h"
#include "../shared/Game.h"
#include "../shared/FileWrap.h"
#include "../shared/Frame.h"
#include "../shared/interfaces/IImageManager.h"
#include "../shared/Level.h"
#include "../shared/Layer.h"
#include "../shared/GameEvents.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/qtgui/qthelper.h"
#include "../shared/interfaces/ISound.h"
#include "../shared/Proto.h"
#include "../shared/ProtoIndex.h"
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QtCore/QVariant>
#include "DlgObject.h"
#include "DlgSource.h"
#include "DlgEditLevel.h"
#include "DlgFrameSet.h"
#include "WizSprite.h"
#include "Snd.h"
#include "WFileDialog.h"
#include "OBL5File.h"
#include "displayconfig.h"
#include "Display.h"
#include "dlgdisplay.h"
#include "fontmanager.h"
#include "Font.h"
#include "WizFont.h"

CToolBoxDock::CToolBoxDock(QWidget *parent) :
    QDockWidget(parent),
    m_ui(new Ui::CToolBoxDock)
{
    m_ui->setupUi(this);
    m_index = nullptr;
    m_ui->tabWidget->setCurrentIndex(0);
    setWidget(m_ui->tabWidget);
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            this, SLOT(docked(Qt::DockWidgetArea)));
}

CToolBoxDock::~CToolBoxDock()
{
    delete m_ui;
    if (m_index) {
        delete (CProtoIndex*)m_index;
        m_index = nullptr;
    }
    clearSprites();
}

void CToolBoxDock::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CToolBoxDock::docked(Qt::DockWidgetArea area)
{
    setMinimumWidth(TOOLBAR_WIDTH);
    switch (area) {
    case Qt::DockWidgetArea::TopDockWidgetArea:
    case Qt::DockWidgetArea::BottomDockWidgetArea:
        m_ui->tabWidget->setTabPosition(QTabWidget::North);
        setMaximumWidth(640);
        break;
    default:
         m_ui->tabWidget->setTabPosition(QTabWidget::West);
         setMaximumWidth(TOOLBAR_WIDTH);
    }
}

/////////////////////////////////////////////////////////////////////////////
// page 2

void CToolBoxDock::copySprite()
{
    QTreeWidgetItem * item = m_ui->treeObjects->currentItem();
    if (!item) {
        return;
    }

    ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
    int protoId = data->protoId;

    CGameFile & gf = *m_gameFile;

    // create dialog

    CDlgObject *d = new CDlgObject ((QWidget*)parent());
    d->setGameDB(m_gameFile);

    int j = gf.m_arrProto.getSize();
    CProto proto;
    gf.m_arrProto.add( proto );

    d->load( protoId );
    d->save( j );

    CProto & newProto = gf.m_arrProto[ j ];
    sprintf(newProto.m_szName, q2c(tr("sprite_%d")), j);
    newProto.resetUUID();

    CProtoIndex *protoIndex = (CProtoIndex *)m_index;
    protoIndex->resizeIndex( protoIndex->getSize() + 1);
    int pos = protoIndex->insert( gf.m_arrProto.getSize() - 1 );

    QAbstractItemModel * model =  m_ui->treeObjects->model();
    if (!model) {
        qCritical("model is null\n");
    }
    model->insertRow(pos);
    item = m_ui->treeObjects->topLevelItem( pos );
    updateIcon( item, gf.m_arrProto.getSize() - 1 );
    m_ui->treeObjects->setCurrentItem( item );
    gf.setDirty(true);
    delete d;
}

void CToolBoxDock::editEvents()
{
    qDebug("editEvents");

    CProtoIndex * indexProto = (CProtoIndex*) m_index;
    QTreeWidgetItem * item = m_ui->treeObjects->currentItem();
    if (!item || !indexProto) {
        return;
    }

    ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
    int protoId = data->protoId;

    qDebug("emit spriteChanged(protoId);");
    emit spriteChanged(protoId);
}

void CToolBoxDock::editSprite()
{
    CProtoIndex * indexProto = (CProtoIndex*) m_index;
    QTreeWidgetItem * item = m_ui->treeObjects->currentItem();
    if (!item || !indexProto) {
        return;
    }

    //indexProto->debug();

    ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
    int protoId = data->protoId;

    CGameFile & gf = *m_gameFile;
    int row = indexProto->findProto(protoId);

    CProto proto = gf.m_arrProto[protoId];
    CDlgObject *d = new CDlgObject ((QWidget*)parent());
    d->setGameDB(m_gameFile);
    d->load( protoId );
    if (d->exec() == QDialog::Accepted) {
        d->save( protoId );

        if (proto != gf.m_arrProto[ protoId ]) {
            qDebug("proto was changed\n");

            indexProto->removeIndex(row);
            int pos = indexProto->insert(protoId);

            QAbstractItemModel * model =  m_ui->treeObjects->model();
            if (!model) {
                qCritical("model is null\n");
            }
            if (pos != row) {
                model->removeRow(row);
                model->insertRow(pos);
            }
            //indexProto->debug();
            QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( pos );
            updateIcon( item, protoId );
            m_ui->treeObjects->setCurrentItem( item );
            gf.setDirty(true);
        }
    }

   delete d;

}

void CToolBoxDock::on_treeObjects_doubleClicked(QModelIndex index)
{
    if (index.row()!=-1)  {
        editSprite();
    }
}

void CToolBoxDock::on_btnAddSprite_clicked()
{
    createSprite();
}

void CToolBoxDock::createSprite()
{
    CGameFile & gf = *m_gameFile;
    int j = gf.m_arrProto.getSize();
    CProto proto;
    sprintf(proto.m_szName, q2c(tr("sprite_%d")), j);
    gf.m_arrProto.add( proto );

    CWizSprite *wiz = new CWizSprite( (QWidget*)parent() );
    wiz->setGameFile(m_gameFile);
    wiz->load(j);
    if (wiz->exec()) {
        wiz->save(j);
        gf.m_arrProto[j].resetUUID();
        // if we create a new frameset
        if (gf.m_arrProto[j].m_nFrameSet == gf.frames().getSize()) {
            qDebug("new frameSet created");
            int frameSet = gf.frames().getSize();
            gf.frames().add(wiz->getFrameSet());
            wiz->getFrameSet()->debug();
            char name[256];
            sprintf(name,"img%d",frameSet);
            gf.frames()[frameSet]->setName(name);
            gf.cache()->add( wiz->getFrameSet() );
            gf.m_arrProto[j].m_nFrameSet = frameSet;
            gf.m_arrProto[j].m_nFrameNo = 0;
        } else {
            // delete the frameSet if we don't need it
            delete wiz->getFrameSet();
        }
        CProtoIndex *protoIndex = m_index;
        protoIndex->resizeIndex( protoIndex->getSize() + 1);
        int pos = protoIndex->insert( gf.m_arrProto.getSize() - 1 );
        QAbstractItemModel * model =  m_ui->treeObjects->model();
        if (!model) {
            qCritical("model is null\n");
        }
        model->insertRow(pos);
        QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( pos );
        updateIcon(item, gf.m_arrProto.getSize() - 1);
        m_ui->treeObjects->setCurrentItem( item );
        gf.setDirty(true);
        if (QMessageBox::question(this, "", tr("Do you want to edit your new sprite?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            editSprite();
        }
    } else {
        gf.m_arrProto.removeAt( j );
    }
    delete wiz;
}

void CToolBoxDock::checkFrameSetUses(int frameSet)
{
    CGameFile & gf = *m_gameFile;
    int *uses = gf.countFrameSetUses();
    CFrameSet *fs = gf.frames()[frameSet];
    if (!uses[frameSet]) {
        QMessageBox::StandardButton
                ret = QMessageBox::warning(
                    dynamic_cast<QWidget*>(parent()),
                    "", tr("ImageSet `%1` appears to no longer being used.\n"
                           "Do you want to remove it?").arg(fs->getName()),
                    QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            gf.frames().removeAt(frameSet);
            gf.killFrameSet(frameSet);
        }
    }
    delete [] uses;
}

void CToolBoxDock::on_btnDeleteSprite_clicked()
{
    CGameFile & gf = *m_gameFile;
    CProtoIndex *protoIndex = (CProtoIndex *)m_index;
    QModelIndex index = m_ui->treeObjects->currentIndex();
    if (!protoIndex) {
        return;
    }

    int j = (*protoIndex) [ index.row() ];
    CProto & proto = gf.m_arrProto[ j ];
    QMessageBox::StandardButton
            ret = QMessageBox::warning( (QWidget*)parent(), "", tr("Are you sure that you want to delete\n"
                                          "#%1 `%2`?").arg( j ) . arg( proto.getName() ),
                                       QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        // get frameset
        int frameSet = proto.m_nFrameSet;

        // remove visual
        QAbstractItemModel * model =  m_ui->treeObjects->model();
        model->removeRow( index.row() );

        // remove object from index
        protoIndex->removeFromIndex ( j );

        // remove object & delete all references
        gf.killProto( j );
        gf.setDirty( true );

        // resync the item data
        for (int i=0; i < protoIndex->getSize(); ++i) {
            QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( i );
            ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
            if (!data) {
                data = new ITEM_DATA;
            }
            data->protoId = (*protoIndex)[i];
            // this is essentially what happens when you call setData
            QVariant v;
            v.setValue( data );
            item->setData(0, Qt::UserRole, v);
        }
        // let other interfaces that we deleted a sprite
        emit spriteDeleted(j);

        // check if this frameSet is still being used
        checkFrameSetUses(frameSet);
    }
}

void CToolBoxDock::updateIcon(QTreeWidgetItem *itm, int protoId)
{
    QTreeWidgetItem * item = itm;
    // TODO: make sure to delete these pointers to prevent memory leaks

    ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
    if (!data) {
        data = new ITEM_DATA;
    }
    data->protoId = protoId;

    // this is essentially what happens when you call setData
    QVariant v;
    v.setValue( data );
    item->setData(0, Qt::UserRole, v);

    CGameFile & gf = *m_gameFile;
    CProto & proto = gf.m_arrProto[ protoId ];

    CFrameSet & filter = *gf.frames()[proto.m_nFrameSet];
    QIcon icon = frame2icon(* filter[proto.m_nFrameNo]);
    icon.actualSize(QSize(32,32));

    QString className;
    if (gf.m_className[proto.m_nClass].empty()) {
        className = QString(tr("Unknown class %1").arg( proto.m_nClass ));
    } else {
        className = gf.m_className[proto.m_nClass].c_str();
    }

    QString label = QString("%1\n%2").arg(proto.m_szName).arg(className);
    item->setIcon(0, icon);
    item->setText(1, label);
}

void CToolBoxDock::updateObjects (int OldframeSet, int newFrameSet)
{
    CGameFile & gf = *m_gameFile;
    for (int i = 0; i < gf.m_arrProto.getSize(); ++i) {
        CProtoIndex * indexProto = (CProtoIndex*) m_index;

        int protoId = (*indexProto)[ i ];
        CProto & proto = gf.m_arrProto[ protoId ];
        if (proto.m_nFrameSet == OldframeSet) {
            if (newFrameSet != -1) {
                proto.m_nFrameSet = newFrameSet;
                proto.m_nFrameNo = 0;
            }
            qDebug("image for obl #%d replaced\n", i);
            QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( i );
            updateIcon(item, protoId);
        }
    }
}

void CToolBoxDock::on_treeObjects_itemSelectionChanged()
{
    QModelIndex indexObject = m_ui->treeObjects->currentIndex();
    QTreeWidgetItem * item = m_ui->treeObjects->currentItem();
    int protoId = CGame::INVALID;
    if (item && indexObject.row() != -1) {
        ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
        protoId = data->protoId;
        const CProto & proto = m_gameFile->m_arrProto.get(protoId);
        qDebug("spriteSelected: proto: [%d] at index [%d] [%s]",
               protoId,
               indexObject.row(),
               proto.getName());
    } else {
        qDebug("spriteSelected: proto: [%d] at index [%d] [%s]",
               protoId,
               indexObject.row(),
               "invalid");
    }
    emit currentProtoChanged(protoId);
    updateButtons();
}

void CToolBoxDock::on_btnCopySprite_clicked()
{
    copySprite();
}

/////////////////////////////////////////////////////////////////////////////
// shared

void CToolBoxDock::updateButtons()
{
    // page 2
    QModelIndex indexObject = m_ui->treeObjects->currentIndex();
    m_ui->btnDeleteSprite->setEnabled( indexObject.row() != -1 );
    m_ui->btnCopySprite->setEnabled( indexObject.row() != -1 );

    // page 3
    QModelIndex indexSound = m_ui->treeSounds->currentIndex();
    m_ui->btnDeleteSound->setEnabled( indexSound.row() > 0);

    // page 4
    //QModelIndex indexFrameSet = m_ui->treeFrameSets->currentIndex();
    //m_ui->btnDeleteFrameSet->setEnabled( indexFrameSet.row() > 0);

    // page 5
    QModelIndex indexLevel = m_ui->treeLevels->currentIndex();
    m_ui->btnDeleteLevel->setEnabled( indexLevel.row() >= 0);
}

void CToolBoxDock::clearSprites()
{
    int count = m_ui->treeObjects->model()->rowCount();
    for (int i=0; i < count; ++i) {
        QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( i );
        ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
        if (data) {
            delete data;
        }
    }

    m_ui->treeObjects->model()->removeRows(0, count);
}

void CToolBoxDock::reloadSprites()
{
    // page 2: Sprites

    if (m_index) {
        delete (CProtoIndex*) m_index;
    }

    CGameFile & gf = *m_gameFile;
    int filter = m_ui->cbFilterSprites->currentIndex();
    m_index = gf.m_arrProto.createIndex(filter, q2c(m_ui->eSearchSprite->text().trimmed()));
    CProtoIndex *index = (CProtoIndex*) m_index;

    clearSprites();
    m_ui->treeObjects->setColumnCount(2);
    int x,y,w,h;
    m_ui->treeObjects->geometry().getRect(&x, &y, &w, &h);
    m_ui->treeObjects->setColumnWidth(0, 32);
    m_ui->treeObjects->setColumnWidth(1, 96);
    m_ui->treeObjects->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeObjects->setWordWrap(true);
    m_ui->treeObjects->setRootIsDecorated(false);
    m_ui->treeObjects->setIconSize(QSize(32,32));
    m_ui->treeObjects->setLineWidth(48);
    m_ui->treeObjects->setAllColumnsShowFocus(true);
    m_ui->treeObjects->setBaseSize(32,32);
    m_ui->treeObjects->setUniformRowHeights(false);
    m_ui->treeObjects->setAlternatingRowColors(true);

    for (int i = 0; i < index->getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        updateIcon(item, (*index)[i] );
        m_ui->treeObjects->addTopLevelItem(item);
    }
}

void CToolBoxDock::reloadSounds()
{
    CGameFile & gf = *m_gameFile;
    int count = m_ui->treeSounds->model()->rowCount();
    m_ui->treeSounds->model()->removeRows(0, count);
    m_ui->treeSounds->setColumnCount(2);
    m_ui->treeSounds->setColumnWidth(0, 128);
    m_ui->treeSounds->setColumnWidth(1, 64);
    m_ui->treeSounds->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeSounds->setWordWrap(false);
    m_ui->treeSounds->setRootIsDecorated(false);
    m_ui->treeSounds->setAlternatingRowColors(true);

    for (int i = 0; i < gf.m_arrSounds.getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, gf.m_arrSounds[i]->getName());
        QString s;
        if (gf.m_arrSounds[i]->getSize() > 2) {
            s = QString( tr("%1 bytes") ).arg(gf.m_arrSounds[i]->getSize());
        } else {
            s = QString( tr("%1 byte") ).arg(gf.m_arrSounds[i]->getSize());
        }

        item->setText(1, s);
        m_ui->treeSounds->addTopLevelItem(item);
    }
}

void CToolBoxDock::reloadLevels()
{
    CGameFile & gf = *m_gameFile;
    int count = m_ui->treeLevels->model()->rowCount();
    m_ui->treeLevels->model()->removeRows(0, count);
    m_ui->treeLevels->setColumnCount(2);
    m_ui->treeLevels->setColumnWidth(0, 32);
    m_ui->treeLevels->setColumnWidth(1, 70);
    m_ui->treeLevels->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeLevels->setWordWrap(false);
    m_ui->treeLevels->setRootIsDecorated(false);
    m_ui->treeLevels->setAlternatingRowColors(true);

    for (int i = 0; i < gf.getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        QString s = QString::asprintf("%.3d", i + 1);
        item->setText(0, s);
        item->setText(1, gf[i]->getSetting("title"));
        m_ui->treeLevels->addTopLevelItem(item);
    }
}

void CToolBoxDock::reloadEvents()
{
    // page 6: events

    CGameFile & gf = *m_gameFile;
    initTree(m_ui->treeEvents);

    QIcon iconBlank;
    iconBlank.addFile(":/images/blank.png");

    QIcon iconCheck;
    iconCheck.addFile(":/images/check.png");

    CGameEvents & events = *(gf.getEvents());
    for (int i = 0; i < events.getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, events.getName(i));
        if (events.getEvent(i)[0]) {
            item->setIcon(0, iconCheck);
        } else {
            item->setIcon(0, iconBlank);
        }
        m_ui->treeEvents->addTopLevelItem(item);
    }
}

void CToolBoxDock::reloadDisplays()
{
    // page x: displays
    qDebug("reload displays()");

    CGameFile & gf = *m_gameFile;
    initTree(m_ui->treeDisplays);

    QIcon iconBlank;
    iconBlank.addFile(":/images/blank.png");

    QIcon iconCheck;
    iconCheck.addFile(":/images/pd/gesloten_slot.png");

    CDisplayConfig & conf = *(gf.getDisplayConfig());
    for (int i = 0; i < conf.getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        CDisplay *display = conf[i];
        item->setText(0, display->name());
        if (display->isProtected()) {
            item->setIcon(0, iconCheck);
        } else {
            item->setIcon(0, iconBlank);
        }
        m_ui->treeDisplays->addTopLevelItem(item);
    }
    m_ui->btnDeleteDisplay->setEnabled(false);
}

void CToolBoxDock::reload()
{
    reloadSprites();
    reloadSounds();
    reloadLevels();
    reloadEvents();
    reloadDisplays();
    reloadFonts();
    updateButtons();
}

void CToolBoxDock::init()
{
    m_ui->treeObjects->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ui->treeLevels->setContextMenuPolicy(Qt::CustomContextMenu);
    m_ui->treeSounds->setContextMenuPolicy(Qt::CustomContextMenu);

    if (!m_ui->cbFilterSprites->count()) {
        for (int i=0; i < CProtoIndex::getFilterCount(); ++i) {
            m_ui->cbFilterSprites->addItem(CProtoIndex::getFilterName(i));
        }
    }

    reload();
}

void CToolBoxDock::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
    emit visibilityChanged(false);
}

void CToolBoxDock::on_treeObjects_customContextMenuRequested(QPoint pos)
{
    QTreeWidgetItem * item = this->m_ui->treeObjects->itemAt(pos);
    if (item) {
        // create pop-up
        QMenu menu(this->m_ui->treeObjects);
        QAction *actionEdit = new QAction(tr("Edit Sprite"), &menu);
        menu.addAction(actionEdit);

        QAction *actionEvents = new QAction(tr("Edit Events"), &menu);
        menu.addAction(actionEvents);

        QAction *actionEditFrames= new QAction(tr("Edit Images"), &menu);
        menu.addAction(actionEditFrames);

        QAction *actionSeparator1 = new QAction("", &menu);
        actionSeparator1->setSeparator(true);
        menu.addAction(actionSeparator1);

        QAction *actionDelete = new QAction(tr("Delete Sprite"), &menu);
        menu.addAction(actionDelete);

        QAction *actionCopy= new QAction(tr("Copy Sprite"), &menu);
        menu.addAction(actionCopy);

        QAction *actionSeparator2 = new QAction("", &menu);
        actionSeparator2->setSeparator(true);
        menu.addAction(actionSeparator2);

        QAction *actionExport= new QAction(tr("Export Sprite"), &menu);
        menu.addAction(actionExport);

        QAction *actionAdd = new QAction(tr("New Sprite"), &menu);
        menu.addAction(actionAdd);

        // Make sure this item is selected
        m_ui->treeObjects->setCurrentItem( item );

        // create an add action and connect it to a signal
        connect(actionEvents, SIGNAL(triggered()), this, SLOT(editEvents()));
        connect(actionEdit, SIGNAL(triggered()), this, SLOT(editSprite()));
        connect(actionCopy, SIGNAL(triggered()), this, SLOT(copySprite()));
        connect(actionDelete, SIGNAL(triggered()), this, SLOT(on_btnDeleteSprite_clicked()));
        connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAddSprite_clicked()));
        connect(actionEditFrames, SIGNAL(triggered()), this, SLOT(editFrames()));
        connect(actionExport, SIGNAL(triggered()), this, SLOT(exportSprite()));

        //emit menuSeekingItems(& menu, MENU_ITEM::POPUP_MENU_SPRITE);
        menu.exec(this->m_ui->treeObjects->mapToGlobal(pos));
       // nodeMenu->exec(event->globalPos());
    } else {

        QMenu menu(this->m_ui->treeObjects);
        QAction *actionAdd = new QAction(tr("New Sprite"), &menu);
        menu.addAction(actionAdd);

        connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAddSprite_clicked()));
        //emit menuSeekingItems(& menu, MENU_ITEM::POPUP_MENU_NO_SPRITE);
        menu.exec(this->m_ui->treeObjects->mapToGlobal(pos));
    }
}

void CToolBoxDock::on_cbFilterSprites_currentIndexChanged(int index)
{
    if (index != -1) {
        reloadSprites();
    }
}

/////////////////////////////////////////////////////////////////////
// Sounds

void CToolBoxDock::on_btnDeleteSound_clicked()
{
    CGame & gf = *(static_cast<CGame*>(m_gameFile));
    QModelIndex index = m_ui->treeSounds->currentIndex();
    if (index.row() < 1) {
        QMessageBox::warning(this, "", tr("This Sound cannot be deleted."), QMessageBox::Ok);
        return;
    }

    CSnd * snd = gf.m_arrSounds[index.row()];

    QMessageBox::StandardButton
            ret = QMessageBox::warning(this,  "",  tr("Are you sure that you want to delete\n"
                                          "`%1` ?") .arg(snd->getName()),
                                       QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        // remove visual
        QAbstractItemModel * model =  m_ui->treeSounds->model();
        model->removeRow( index.row() );
        // delete all references
        gf.m_arrProto.killSound ( index.row() );     
        CSnd* snd = gf.m_arrSounds[index.row()];
        gf.sound()->remove(snd->getUID());
        gf.m_arrSounds.removeAt( index.row() );
        delete snd;
        gf.setDirty( true );
    }
}

void CToolBoxDock::on_treeSounds_itemSelectionChanged()
{
    updateButtons();
}


void CToolBoxDock::on_treeSounds_doubleClicked(QModelIndex index)
{
    if (index.row() != -1) {
        editSound();
    }
}

void CToolBoxDock::on_btnAddSound_clicked()
{
    CGame & gf = *(static_cast<CGame*>(m_gameFile));
    CSnd *snd = new CSnd();
    QString wavFilter = tr("Wav sounds (*.wav)");
    QString oggFilter = tr("OggSound (*.ogg)");
    QString allFilter = tr("Popular sound files (*.wav *.ogg *.flac *.aiff *.au)");
    static QString fileFilter = allFilter + ";;" + wavFilter + ";;" + oggFilter ;

    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import sound..."), "", fileFilter, &selectedFilter);
    if (!fileName.isEmpty()) {
        if (selectedFilter == allFilter) {
            fileFilter = allFilter + ";;" + oggFilter + ";;" + wavFilter;
        } else if (selectedFilter == oggFilter) {
            fileFilter = oggFilter + ";;" + wavFilter + ";;" + allFilter ;
        } else {
            fileFilter = wavFilter + ";;" + oggFilter + ";;" + allFilter;
        }

        CFileWrap file;
        if (file.open( q2c(fileName) )) {
            int size = file.getSize();
            char *data = new char[size];
            file.read(data, size);
            file.close();

            // replace the sound data
            snd->replace(data, size);
            // rename the snd to the filename
            snd->setName(q2c(QFileInfo(fileName).fileName().mid(0, 31)));

            // add the sound to the array
            int i  = gf.m_arrSounds.getSize();
            gf.m_arrSounds.add( snd );
            // add sound to ISound
            gf.sound()->add((unsigned char*)data, size, gf.m_arrSounds[i]->getUID());

            // update visual
            QTreeWidgetItem *item = new QTreeWidgetItem(0);
            item->setText(0, gf.m_arrSounds[i]->getName());
            QString s;
            if (gf.m_arrSounds[i]->getSize() > 2) {
                s = QString( tr("%1 bytes") ).arg(gf.m_arrSounds[i]->getSize());
            } else {
                s = QString( tr("%1 byte") ).arg(gf.m_arrSounds[i]->getSize());
            }

            item->setText(1, s);
            m_ui->treeSounds->addTopLevelItem(item);

            // let the application know that the document was modified
            gf.setDirty( true );

        } else {
            QMessageBox::warning(this,"", QString(tr("can't open `%1`\n")).arg(fileName), QMessageBox::Ok );
        }
    }
}

/////////////////////////////////////////////////////////////////////
// Levels

void CToolBoxDock::on_treeLevels_doubleClicked(QModelIndex index)
{
    if (index.row() != -1) {
        emit levelEdited(index.row());
    }
}

void CToolBoxDock::on_treeLevels_itemSelectionChanged()
{
}

void CToolBoxDock::on_treeLevels_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);
    Q_UNUSED(current);

    QModelIndex index = m_ui->treeLevels->currentIndex();
    updateButtons();
    if (index.row() != -1) {
        emit levelSelection(index.row());
    }
}

void CToolBoxDock::on_btnAddLevel_clicked()
{
    emit levelAdded();
}

void CToolBoxDock::on_btnDeleteLevel_clicked()
{
    QModelIndex index = m_ui->treeLevels->currentIndex();
    emit levelDeleted(index.row());
}

/////////////////////////////////////////////////////////////////////
// Events

void CToolBoxDock::on_treeEvents_doubleClicked(QModelIndex index)
{
    CGameFile & gf = *m_gameFile;
    CGameEvents & events = *(gf.getEvents());
    CDlgSource *d = new CDlgSource ( (QWidget*) parent());
    d->init(m_gameFile);
    d->setWindowTitle(tr("Edit Event `%1`").arg(CGameEvents::getName(index.row())));
    d->setText(events[index.row()]);
    if (d->exec()) {

        QString text = d->getText().trimmed();
        if (events.getEvent(index.row()) != text) {
            events.setEvent(index.row(), q2c(text));
            gf.setDirty( true );
        }

        QTreeWidgetItem * item = m_ui->treeEvents->topLevelItem( index.row() );
        if (text.isEmpty()) {
            QIcon iconBlank;
            iconBlank.addFile(":/images/blank.png");
            item->setIcon(0, iconBlank);
        } else {
            QIcon iconCheck;
            iconCheck.addFile(":/images/check.png");
            item->setIcon(0, iconCheck);
        }
    }

    delete d;
}

void CToolBoxDock::on_btnClearAll_clicked()
{
    CGameFile & gf = *m_gameFile;
    if (QMessageBox::warning(this, "", tr("Are you sure that you want to clear all the events"),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
        QIcon iconBlank;
        iconBlank.addFile(":/images/blank.png");
        CGameEvents & events = *(gf.getEvents());
        for (int i = 0; i < events.getSize(); ++i) {
            if (events.getEvent(i)[0]) {
                QTreeWidgetItem * item = m_ui->treeEvents->topLevelItem( i );
                item->setIcon(0, iconBlank);
                gf.setDirty( true );
                events.setEvent(i, "");
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////
// slots

void CToolBoxDock::deleteLevel(int index)
{
    m_ui->treeLevels->model()->removeRow(index);
    if ( index > m_gameFile->getSize() - 1) {
        -- index ;
    }

    // renumber dealigned entries
    for (int i = index; i > 0  && i < m_gameFile->getSize(); ++i) {
        QTreeWidgetItem * item = m_ui->treeLevels->topLevelItem( i );
        if (item) {
            QString s = QString::asprintf("%.3d", i + 1);
            item->setText(0, s);
        }
    }

    if (m_gameFile->getSize()) {
        selectLevel(index);
    }

}

void CToolBoxDock::addLevel()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(0);
    int index = m_gameFile->getSize() - 1;
    CLevel & level = m_gameFile->getLevelObject(index);
    QString s = QString::asprintf("%.3d", index + 1);
    item->setText(0, s);
    item->setText(1, level.getSetting("title"));
    m_ui->treeLevels->addTopLevelItem(item);
    selectLevel( index );
}

void CToolBoxDock::selectLevel(int index)
{
    if (index != -1) {
        QTreeWidgetItem * item = m_ui->treeLevels->topLevelItem(index);
        m_ui->treeLevels->setCurrentItem(item);
    }
}

void CToolBoxDock::editLevel(int index)
{
    QTreeWidgetItem * item = m_ui->treeLevels->topLevelItem( index );
    if (item) {
        CLevel & levelObj = m_gameFile->getLevelObject(index);
        QString s = QString::asprintf("%.3d", index + 1);
        item->setText(0, s);
        item->setText(1, levelObj.getSetting("title"));
        selectLevel(index);
    }
}

void CToolBoxDock::moveLevel(int fromIndex, int toIndex)
{
    m_ui->treeLevels->model()->removeRow(fromIndex);
    m_ui->treeLevels->model()->insertRow(toIndex);
    QTreeWidgetItem * item = m_ui->treeLevels->topLevelItem( toIndex );
    CLevel & levelObj = m_gameFile->getLevelObject(toIndex);
    QString s = QString::asprintf("%.3d", toIndex + 1);
    item->setText(0, s);
    item->setText(1, levelObj.getSetting("title"));
    for (int i = 0; i < m_gameFile->getSize(); ++i) {
        QTreeWidgetItem * item = m_ui->treeLevels->topLevelItem( i );
        QString s = QString::asprintf("%.3d", i + 1);
        item->setText(0, s);
    }
    selectLevel(toIndex);
}

void CToolBoxDock::on_treeLevels_customContextMenuRequested(QPoint pos)
{
    QTreeWidgetItem * item = this->m_ui->treeLevels->itemAt(pos);
    if (item) {
        // create pop-up
        QMenu menu(this->m_ui->treeLevels);
        QAction *actionEdit = new QAction(tr("Edit Level"), &menu);
        menu.addAction(actionEdit);

        QAction *actionDelete = new QAction(tr("Delete Level"), &menu);
        menu.addAction(actionDelete);

        QAction *actionAdd = new QAction(tr("Add Level"), &menu);
        menu.addAction(actionAdd);

        QAction *actionMove= new QAction(tr("Move Level"), &menu);
        menu.addAction(actionMove);

        // Make sure this item is selected
        m_ui->treeLevels->setCurrentItem( item );

        // create an add action and connect it to a signal
        connect(actionEdit, SIGNAL(triggered()), parent(), SLOT( on_actionEdit_Level_triggered()));
        connect(actionMove, SIGNAL(triggered()), parent(), SLOT( on_actionMove_Level_triggered()));
        connect(actionDelete, SIGNAL(triggered()), parent(), SLOT( on_actionDelete_Level_triggered()));
        connect(actionAdd, SIGNAL(triggered()), parent(), SLOT(addLevel()));
        menu.exec(this->m_ui->treeLevels->mapToGlobal(pos));        
    } else {
        QMenu menu(this->m_ui->treeLevels);
        QAction *actionAdd = new QAction(tr("Add Level"), &menu);
        menu.addAction(actionAdd);
        connect(actionAdd, SIGNAL(triggered()), parent(), SLOT(addLevel()));
        menu.exec(this->m_ui->treeLevels->mapToGlobal(pos));
    }
}

void CToolBoxDock::on_treeSounds_customContextMenuRequested(QPoint pos)
{
    QTreeWidgetItem * item = this->m_ui->treeSounds->itemAt(pos);
    if (item) {
        // create pop-up
        QMenu menu(this->m_ui->treeSounds);
        QAction *actionEdit = new QAction(tr("Edit Sound"), &menu);
        menu.addAction(actionEdit);

        QAction *actionPlay = new QAction(tr("Play Sound"), &menu);
        menu.addAction(actionPlay);

        QAction *actionDelete = new QAction(tr("Delete Sound"), &menu);
        menu.addAction(actionDelete);

        QAction *actionAdd = new QAction(tr("Add Sound"), &menu);
        menu.addAction(actionAdd);

        QAction *actionRename = new QAction(tr("Rename Sound"), &menu);
        menu.addAction(actionRename);

        // Make sure this item is selected
        m_ui->treeSounds->setCurrentItem( item );

        // create an add action and connect it to a signal
        connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAddSound_clicked()));
        connect(actionEdit, SIGNAL(triggered()), this, SLOT( editSound()));
        connect(actionPlay, SIGNAL(triggered()), this, SLOT( playSound()));
        connect(actionDelete, SIGNAL(triggered()), this, SLOT( on_btnDeleteSound_clicked()));
        connect(actionRename, SIGNAL(triggered()), this, SLOT( renameSound()));
        menu.exec(this->m_ui->treeSounds->mapToGlobal(pos));
    } else {

        QMenu menu(this->m_ui->treeSounds);
        QAction *actionAdd = new QAction(tr("Add Sounds"), &menu);
        menu.addAction(actionAdd);

        connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAddSound_clicked()));
        menu.exec(this->m_ui->treeSounds->mapToGlobal(pos));
    }
}

void CToolBoxDock::editSound()
{
    QModelIndex index = m_ui->treeSounds->currentIndex();
    CGame & gf = *(static_cast<CGame*>(m_gameFile));

    if (index.row()<0) {
        return;
    }

    if (!index.row()) {
        QMessageBox::warning(this, "", tr("This sound cannot be edited."),  QMessageBox::Ok);
        return;
    }

    CSnd snd ( (* gf.m_arrSounds[ index.row() ]) );
    QString wavFilter = tr("Wav sounds (*.wav)");
    QString oggFilter = tr("Ogg sounds (*.ogg)");
    QString allFilter = tr("Popular sound files (*.wav *.ogg *.flac *.aiff *.au)");
    static QString fileFilter = allFilter + ";;" + wavFilter + ";;" + oggFilter ;

    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName((QWidget*)parent(), tr("Replace sound..."), "", fileFilter, &selectedFilter);
    if (!fileName.isEmpty()) {
        if (selectedFilter == allFilter) {
            fileFilter = allFilter + ";;" + oggFilter + ";;" + wavFilter;
        } else if (selectedFilter == oggFilter) {
            fileFilter = oggFilter + ";;" + wavFilter + ";;" + allFilter ;
        } else {
            fileFilter = wavFilter + ";;" + oggFilter + ";;" + allFilter;
        }

        CFileWrap file;
        if (file.open( q2c(fileName) )) {
            int size = file.getSize();
            char *data = new char[size];
            file.read(data, size);
            file.close();

            // replace the sound data
            snd.replace(data, size);
            // rename the snd to the filename
            snd.setName(q2c(QFileInfo(fileName).fileName().mid(0, 31)));

            if ((*gf.m_arrSounds[ index.row()]) != snd) {
                CSnd *oldSnd = gf.m_arrSounds[ index.row()];
                // remove old sound from ISound
                gf.sound()->remove(oldSnd->getUID());
                // copy sound data
                CSnd *newSnd = gf.m_arrSounds[ index.row()];
                newSnd->copy( snd );
                // add sound to ISound
                gf.sound()->add((unsigned char*)newSnd->getData(),
                                newSnd->getSize(),
                                newSnd->getUID());
                newSnd->debug();
                qDebug("snd was changed\n");

                QTreeWidgetItem * item = m_ui->treeSounds->topLevelItem( index.row() );
                item->setText(0, gf.m_arrSounds[index.row()]->getName());
                QString s;
                if (gf.m_arrSounds[index.row()]->getSize() > 2) {
                    s = QString( tr("%1 bytes") ).arg(gf.m_arrSounds[index.row()]->getSize());
                } else {
                    s = QString( tr("%1 byte") ).arg(gf.m_arrSounds[index.row()]->getSize());
                }

                item->setText(1, s);
                gf.setDirty(true);
            }
        } else {
            QMessageBox::warning(this,"", QString(tr("can't open `%1`\n")).arg(fileName), QMessageBox::Ok );
        }
    }
}

void CToolBoxDock::playSound()
{
    QModelIndex index = m_ui->treeSounds->currentIndex();
    if (index.row()<0) {
        return;
    }
    if (!index.row()) {
        QMessageBox::warning(this, "", tr("This sound cannot be played."),  QMessageBox::Ok);
        return;
    }
    CGame *game = (CGame*) m_gameFile;
    game->initSounds();
    game->playSound(index.row());
}

void CToolBoxDock::renameSound()
{
    QModelIndex index = m_ui->treeSounds->currentIndex();
    if (index.row()<0) {
        return;
    }
    if (!index.row()) {
        QMessageBox::warning(this, "", tr("This sound cannot be renamed."),  QMessageBox::Ok);
        return;
    }

    CGame *gf = (CGame*) m_gameFile;
    CSnd * snd = gf->m_arrSounds[ index.row()];
    bool ok;
    QString text = QInputDialog::getText(static_cast<QWidget*>(parent()), tr("Rename Sound"),
                                         tr("Sound name:"), QLineEdit::Normal,
                                         snd->getName(), &ok);
    if (ok && !text.isEmpty()) {
        snd->setName(q2c(text));
        QTreeWidgetItem * item = m_ui->treeSounds->topLevelItem( index.row() );
        item->setText(0, text);
    }
}

void CToolBoxDock::editFrames()
{
    CGameFile & gf = *m_gameFile;
    QModelIndex index = m_ui->treeObjects->currentIndex();
    if (!m_index) {
        return;
    }
    CProtoIndex & protoIndex = *m_index;
    int currSprite = index.row();

    int j = protoIndex[ currSprite ];
    CProto & proto = gf.m_arrProto[ j ];
    int fs = proto.m_nFrameSet;

    CDlgFrameSet * d = new CDlgFrameSet (this);
    d->setWindowTitle ( QString(tr("Edit Image Set `%1`")).arg(gf.frames()[fs]->getName()) );
    CFrameSet * frameSet = new CFrameSet (gf.frames()[fs]);
    d->init(frameSet);
    if (d->exec() == QDialog::Accepted) {
        qDebug("on_treeFrameSets_doubleClicked(QModelIndex index) save\n");
        gf.setDirty( true );
        // replace frameSet
        delete gf.frames()[ fs ];
        gf.frames().setAt( fs, frameSet);
        // update the imageCache
        gf.cache()->replace(fs, frameSet);
        updateFrameSet(fs);
        QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( currSprite );
        m_ui->treeObjects->setCurrentItem( item );

    } else {
        delete frameSet;
    }
    delete d;
}

void CToolBoxDock::updateFrameSet(int frameSet)
{
    CGame & gf = * static_cast<CGame*>(m_gameFile);
    // update any sprite using this frameSet
    CProtoIndex & protoIndex = *m_index;
    for (int i=0; i < protoIndex.getSize();++i) {
        int k = protoIndex[ i ];
        CProto & proto = gf.m_arrProto[ k ];
        if (proto.m_nFrameSet == frameSet) {
            // replace
            QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( i );
            updateIcon( item, k );
        }
    }
}

void CToolBoxDock::exportSprite()
{
    const QString oblFilter = QObject::tr("Object Blocks (*.obl *.obl5)");
    const QString pngFilter = QObject::tr("PNG Images (*.png)");
    const QString metaFilter = QObject::tr("Metadata (*.proto)");

    CProtoIndex * indexProto = (CProtoIndex*) m_index;
    QTreeWidgetItem * item = m_ui->treeObjects->currentItem();
    if (!item || !indexProto) {
        return;
    }

    ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
    int protoId = data->protoId;

    CGame & gf = *((CGame*)m_gameFile);
    CProto & proto = gf.m_arrProto[protoId];
    CFrameSet * frameSet = gf.frames()[proto.m_nFrameSet];

    QString selected = pngFilter + "\n" + oblFilter + "\n" + metaFilter;
    QString suffix = "png";
    QString fileName = proto.getName();
    QStringList filters;
    filters.append(pngFilter);
    filters.append(oblFilter);
    filters.append(metaFilter);

    CWFileDialog * dlg = new CWFileDialog(this,tr("Save As"), QDir::homePath(), selected);
    dlg->setNameFilters(filters);
    dlg->setAcceptMode(QFileDialog::AcceptSave);
    dlg->setDefaultSuffix(suffix);
    dlg->selectNameFilter(pngFilter); // workarround for default suffix not being set
    if (dlg->exec()) {
        QStringList fileNames = dlg->selectedFiles();
        if (fileNames.count()>0) {
            fileName = fileNames[0];
        }
    }

    std::string outFormat;
    if (!fileName.isEmpty()) {
        selected = dlg->selectedNameFilter();
        bool isObl = true;
        if (selected == oblFilter) {
            outFormat = "OBL5";
        } else if(selected == pngFilter) {
            outFormat = "PNG";
        } else if(selected == metaFilter) {
            outFormat = "PRTO";
            isObl = false;
        }
        bool result = false;
        if (isObl) {
            COBL5File oblDoc;
            oblDoc.setFormat(outFormat.c_str());
            oblDoc.getImageSet() = *frameSet;
            oblDoc.setFileName(fileName);
            result = oblDoc.write();
        } else {
            CFileWrap file;
            if (file.open(q2c(fileName), "wb")) {
                result = m_gameFile->m_arrProto.exportMeta(file, protoId);
                file.close();
            }
        }
        if (!result) {
            QMessageBox::warning(static_cast<QWidget*>(parent()), "", tr("Failed to export."));
        }
    }
    delete dlg;
}

void CToolBoxDock::on_treeDisplays_doubleClicked(const QModelIndex &index)
{
    CGame & gf = *((CGame*)m_gameFile);
    int i = index.row();
    if (i !=-1)  {
        CDisplayConfig & conf = *(m_gameFile->getDisplayConfig());
        CDlgDisplay dlg;
        CDisplay & d = *(conf[i]);
        dlg.setWindowTitle(tr("Edit Overlay ``%1`` %2").arg(d.name()).arg(d.isProtected() ? tr("[locked]") : ""));
        dlg.setGameFile(m_gameFile);
        dlg.setDisplayID(i);
        dlg.load(d);
        if (dlg.exec() == QDialog::Accepted) {
            dlg.save(d);
            gf.setDirty(true);
            QTreeWidgetItem * item = m_ui->treeDisplays->topLevelItem( i );
            m_ui->treeDisplays->setCurrentItem( item );
            item->setText(0, d.name());
        }
    }
}

void CToolBoxDock::on_btnDeleteDisplay_clicked()
{
    CGame & gf = *((CGame*)m_gameFile);
    QModelIndex index = m_ui->treeDisplays->currentIndex();
    int i = index.row();
    if (i == -1) {
        QMessageBox::warning(this, "", tr("No Overlay selected."), QMessageBox::Ok);
        return;
    }
    CDisplayConfig & conf = *(m_gameFile->getDisplayConfig());
    CDisplay *d = conf[i];
    if (d->isProtected()) {
        QMessageBox::warning(this, "", tr("This Overlay is owned by the engine and cannot be deleted."), QMessageBox::Ok);
        return;
    }
    QMessageBox::StandardButton
            ret = QMessageBox::warning(
                this,  "",  tr("Are you sure that you want to delete\n"
                               "`%1` ?") .arg(d->name()),
                QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        // remove visual
        QAbstractItemModel * model =  m_ui->treeDisplays->model();
        model->removeRow( index.row() );
        conf.removeAt(i);
        gf.setDirty( true );
        m_ui->btnDeleteDisplay->setEnabled(false);
    }
}

void CToolBoxDock::on_btnAddDisplay_clicked()
{
    CGameFile & gf = *m_gameFile;
    CDisplayConfig & conf = *(m_gameFile->getDisplayConfig());
    CDlgDisplay dlg;
    CDisplay d = CDisplay("", 0, 0, 0);
    d.setTemplate("");
    d.setText("");
    d.setType(CDisplay::DISPLAY_MESSAGE);
    dlg.setWindowTitle(tr("New overlay"));
    dlg.setGameFile(m_gameFile);
    dlg.load(d);
    if (dlg.exec() == QDialog::Accepted) {
        dlg.save(d);
        int pos = conf.getSize();
        conf.add(d);
        QAbstractItemModel * model =  m_ui->treeDisplays->model();
        if (!model) {
            qCritical("model is null\n");
        }
        model->insertRow(pos);
        QTreeWidgetItem * item = m_ui->treeDisplays->topLevelItem( pos );
        m_ui->treeDisplays->setCurrentItem( item );
        item->setText(0, d.name());
        QIcon iconBlank;
        iconBlank.addFile(":/images/blank.png");
        item->setIcon(0, iconBlank);
        gf.setDirty(true);
    }
}

void CToolBoxDock::on_treeDisplays_clicked(const QModelIndex &index)
{
    CDisplayConfig & conf = *(m_gameFile->getDisplayConfig());
    int i = index.row();
    m_ui->btnDeleteDisplay->setEnabled(i != -1 && !conf[i]->isProtected());
}

void CToolBoxDock::on_treeDisplays_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem * item = this->m_ui->treeDisplays->itemAt(pos);
    if (item) {
        // Make sure this item is selected
        m_ui->treeDisplays->setCurrentItem( item );

        CDisplayConfig & conf = *(m_gameFile->getDisplayConfig());
        QModelIndex index = this->m_ui->treeDisplays->currentIndex();
        int i = index.row();

        // create pop-up
        QMenu menu(this->m_ui->treeDisplays);
        QAction *actionEdit = new QAction(tr("Edit Overlay"), &menu);
        menu.addAction(actionEdit);

        QAction *actionAdd = new QAction(tr("New Overlay"), &menu);
        menu.addAction(actionAdd);
        if (i != -1 && !conf[i]->isProtected()) {
            QAction *actionDelete = new QAction(tr("Delete Overlay"), &menu);
            menu.addAction(actionDelete);
            connect(actionDelete, SIGNAL(triggered()), this, SLOT(on_btnDeleteDisplay_clicked()));
        }

        // create an action and connect it to a signal
        connect(actionEdit, SIGNAL(triggered()), this, SLOT(editDisplay()));
        connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAddDisplay_clicked()));
        menu.exec(this->m_ui->treeDisplays->mapToGlobal(pos));
    } else {
        QMenu menu(this->m_ui->treeDisplays);
        QAction *actionAdd = new QAction(tr("New Overlay"), &menu);
        menu.addAction(actionAdd);

        connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAddDisplay_clicked()));
        menu.exec(this->m_ui->treeDisplays->mapToGlobal(pos));
    }
}

void CToolBoxDock::editDisplay()
{
    QModelIndex index = m_ui->treeDisplays->currentIndex();
    on_treeDisplays_doubleClicked(index);
}

void CToolBoxDock::on_treeEvents_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem * item = this->m_ui->treeEvents->itemAt(pos);
    if (item) {
        // Make sure this item is selected
        m_ui->treeEvents->setCurrentItem( item );
        // create pop-up
        QMenu menu(this->m_ui->treeEvents);
        QAction *actionEdit = new QAction(tr("Edit Event"), &menu);
        menu.addAction(actionEdit);

        // create an action and connect it to a signal
        connect(actionEdit, SIGNAL(triggered()), this, SLOT(editEvent()));
        menu.exec(this->m_ui->treeEvents->mapToGlobal(pos));
    } else {
        QMenu menu(this->m_ui->treeEvents);
        QAction *actionClear = new QAction(tr("Clear all events"), &menu);
        menu.addAction(actionClear);

        connect(actionClear, SIGNAL(triggered()), this, SLOT(on_btnClearAll_clicked()));
        menu.exec(this->m_ui->treeEvents->mapToGlobal(pos));
    }
}

void CToolBoxDock::editEvent()
{
    QModelIndex index = m_ui->treeEvents->currentIndex();
    on_treeEvents_doubleClicked(index);
}

void CToolBoxDock::updateIcon(int protoId)
{
    int i = m_index->findProto(protoId);
    if (i !=  -1) {
        QTreeWidgetItem * item =  m_ui->treeObjects->topLevelItem(i);
        updateIcon(item, protoId);
    }
}

void CToolBoxDock::initTree(QTreeWidget *tree)
{
    int count = tree->model()->rowCount();
    tree->model()->removeRows(0, count);
    tree->setColumnCount(1);
    tree->setColumnWidth(0, 128);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setWordWrap(false);
    tree->setRootIsDecorated(false);
    tree->setAlternatingRowColors(true);
}

void CToolBoxDock::reloadFonts()
{
    // page x: fonts
    qDebug("reload fonts()");

    CGameFile & gf = *m_gameFile;
    QTreeWidget *tree = m_ui->treeFonts;
    initTree(tree);

    QIcon iconBlank;
    iconBlank.addFile(":/images/blank.png");

    QIcon iconCheck;
    iconCheck.addFile(":/images/pd/gesloten_slot.png");

    CFontManager & fonts = *(gf.getFonts());
    for (int i = 0; i < fonts.getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, fonts.nameAt(i));
        if (!i) {
            item->setIcon(0, iconCheck);
        } else {
            item->setIcon(0, iconBlank);
        }
        tree->addTopLevelItem(item);
    }
    m_ui->btnDeleteFont->setEnabled(false);
}

void CToolBoxDock::on_treeFonts_doubleClicked(const QModelIndex &index)
{
    if (index.row() > 0) {
        editFont();
    }
}

void CToolBoxDock::on_treeFonts_clicked(const QModelIndex &index)
{
    int i = index.row();
    m_ui->btnDeleteFont->setEnabled(i != 0);
}

void CToolBoxDock::on_treeFonts_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidget *tree = m_ui->treeFonts;
    QTreeWidgetItem * item = tree->itemAt(pos);
    if (item) {
        // Make sure this item is selected
        tree->setCurrentItem( item );

        QModelIndex index = tree->currentIndex();
        int i = index.row();

        // create pop-up
        QMenu menu(tree);
        QAction *actionAdd = new QAction(tr("New Font"), &menu);
        menu.addAction(actionAdd);
        if (i != 0) {
            QAction *actionEdit = new QAction(tr("Rename Font"), &menu);
            menu.addAction(actionEdit);
            QAction *actionDelete = new QAction(tr("Delete Font"), &menu);
            menu.addAction(actionDelete);
            connect(actionDelete, SIGNAL(triggered()), this, SLOT(on_btnDeleteFont_clicked()));
            // create an action and connect it to a signal
            connect(actionEdit, SIGNAL(triggered()), this, SLOT(editFont()));
        }
        connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAddFont_clicked()));
        menu.exec(tree->mapToGlobal(pos));
    } else {
        QMenu menu(tree);
        QAction *actionAdd = new QAction(tr("New Font"), &menu);
        menu.addAction(actionAdd);

        connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_btnAddFont_clicked()));
        menu.exec(tree->mapToGlobal(pos));
    }
}

void CToolBoxDock::on_btnAddFont_clicked()
{
    CWizFont *wiz = new CWizFont(static_cast<QWidget*>(parent()));
    if (wiz->exec()) {
        CFont font;
        QString name;
        wiz->importFont(font, name);
        CGameFile & gf = *m_gameFile;
        CFontManager & fonts = *(gf.getFonts());
        int pos = fonts.getSize();
        int x = 0;
        QString newName = name;
        while (fonts.indexOf(q2c(newName)) != CFontManager::NOT_FOUND) {
            ++x;
            newName = QString("%1_%2").arg(name).arg(x);
        }
        fonts.add(font, q2c(newName));

        QTreeWidget * tree = m_ui->treeFonts;
        QAbstractItemModel * model =  tree->model();
        if (!model) {
            qCritical("model is null\n");
        }
        model->insertRow(pos);
        QTreeWidgetItem * item = tree->topLevelItem( pos );
        tree->setCurrentItem( item );
        item->setText(0, newName);
        QIcon iconBlank;
        iconBlank.addFile(":/images/blank.png");
        item->setIcon(0, iconBlank);
        gf.setDirty(true);
        m_ui->btnDeleteFont->setEnabled(true);
    }
}

void CToolBoxDock::on_btnDeleteFont_clicked()
{
    CGameFile & gf = *m_gameFile;
    CDisplayConfig & conf = *(m_gameFile->getDisplayConfig());
    QTreeWidget * tree = m_ui->treeFonts;
    QModelIndex index = tree->currentIndex();
    int i = index.row();
    if (i == -1) {
        QMessageBox::warning(this, "", tr("No Font selected."), QMessageBox::Ok);
        return;
    }
    if (i == 0) {
        QMessageBox::warning(this, "", tr("This Font is owned by the engine and cannot be deleted."), QMessageBox::Ok);
        return;
    }
    CFontManager & fonts = *(gf.getFonts());
    QMessageBox::StandardButton
            ret = QMessageBox::warning(
                this,  "",  tr("Are you sure that you want to delete\n"
                               "`%1` ?") .arg(fonts.nameAt(i)),
                QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        // remove visual
        QAbstractItemModel * model =  m_ui->treeFonts->model();
        model->removeRow( index.row() );
        fonts.removeAt(i);
        gf.setDirty( true );
        conf.killFont(i);
        m_ui->btnDeleteFont->setEnabled(false);
    }
}

void CToolBoxDock::editFont()
{
    QTreeWidget *tree = m_ui->treeFonts;
    QModelIndex index = tree->currentIndex();
    int i = index.row();
    if (i < 0) {
        return;
    }
    if (!i) {
        QMessageBox::warning(this, "", tr("This font cannot be renamed."),  QMessageBox::Ok);
        return;
    }
    bool ok;
    QString text = QInputDialog::getText(static_cast<QWidget*>(parent()), tr("Rename Font"),
                                         tr("Font name:"), QLineEdit::Normal,
                                         m_gameFile->getFonts()->nameAt(i), &ok);
    if (ok && !text.isEmpty()) {
        CFontManager & fonts = *(m_gameFile->getFonts());
        int x = 0;
        QString newName = text;
        while (fonts.indexOf(q2c(newName)) != i && fonts.indexOf(q2c(newName)) != CFontManager::NOT_FOUND) {
            ++x;
            newName = QString("%1_%2").arg(text).arg(x);
        }
        m_gameFile->getFonts()->setName(i, q2c(newName));
        QTreeWidgetItem * item = tree->topLevelItem( index.row() );
        item->setText(0, text);
        m_gameFile->setDirty(true);
    }
}

void CToolBoxDock::on_eSearchSprite_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    reloadSprites();
}

/*
    LGCK Builder GUI
    Copyright (C) 1999, 2011  Francois Blanchette

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

#include "DlgGame.h"
#include "ui_DlgGame.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QStandardItemModel>
#include "DlgObject.h"
#include "DlgFrameSet.h"
#include "DlgEditLevel.h"
#include "DlgSource.h"
#include "WizFrameSet.h"
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/Frame.h"
#include "../shared/Snd.h"
#include "../shared/FileWrap.h"
#include "../shared/Level.h"
#include "../shared/Layer.h"
#include "../shared/interfaces/IImageManager.h"
#include "../shared/interfaces/ISound.h"
#include "../shared/GameEvents.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/Game.h"

CDlgGame::CDlgGame(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgGame)
{
    m_ui->setupUi(this);
    m_indexSettings = NULL;
    m_countFrameSetUses = NULL;
}

CDlgGame::~CDlgGame()
{
    delete m_ui;
    if (m_indexSettings) {
        delete [] m_indexSettings;
        m_indexSettings = NULL;
    }
    if (m_countFrameSetUses) {
        delete [] m_countFrameSetUses;
    }
}

void CDlgGame::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// page 1

void CDlgGame::on_treeSettings_doubleClicked(QModelIndex index)
{
    CGameFile &gf = *((CGameFile*)m_gameFile);
    bool ok;

    int i = m_indexSettings[ index.row() ];
    CSettings::SETTING & setting = (* gf.getSettings())[ i ];
    QString result = QInputDialog::getText(this, "",
            setting.param.c_str(), QLineEdit::Normal,
            setting.value.c_str(), &ok);

    if (ok && (result != setting.value.c_str())) {
        setting.value = q2c(result);
        setting.valueInt = strtol(setting.value.c_str(), NULL, 10);
        QTreeWidgetItem * item = m_ui->treeSettings->topLevelItem( index.row() );
        item->setText(1, result);
        gf.setDirty( true );
    }
}

void CDlgGame::on_btnDefaults_clicked()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    if (QMessageBox::warning(this, "", tr("Restoring the default settings will replace any changes you have made\nAre you sure that you want to continue?"),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {

        gf.restoreDefaults();
        QAbstractItemModel * model =  m_ui->treeSettings->model();
        int totalRows = model->rowCount();
        //qDebug("totalRows: %d\n", totalRows);
        model->removeRows(0, totalRows);
        fillTreeSettings();
        gf.setDirty( true );
    }
}

void CDlgGame::fillTreeSettings()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    int settingsCount = gf.getSettings()->getSize();
    m_indexSettings = new int [ settingsCount ];
    for (int i=0, j=0; i < settingsCount ; ++i) {
        CSettings::SETTING & setting = (* gf.getSettings())[ i ];
        if (setting.param[0] != '#') {
            m_indexSettings[j] = i;
            QTreeWidgetItem *item = new QTreeWidgetItem(0);
            item->setText(0, setting.param.c_str());
            item->setText(1, setting.value.c_str());
            m_ui->treeSettings->addTopLevelItem(item);
            ++j;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// page 2

void CDlgGame::on_treeObjects_doubleClicked(QModelIndex index)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);

    int j = gf.m_arrProto.getAtIndex( index.row() );
    if (!j) {
        QMessageBox::warning(this, "", tr("This object cannot be edited."),  QMessageBox::Ok);
        return;
    }

    CProto proto = gf.m_arrProto[j];
    CDlgObject *d = new CDlgObject (this);
    d->setGameDB(m_gameFile);
    d->load( j );
    if (d->exec() == QDialog::Accepted) {
        d->save( j );

        if (proto != gf.m_arrProto[ j ]) {
            qDebug("proto was changed\n");

            gf.m_arrProto.removeIndex( index.row() );
            int pos = gf.m_arrProto.insertIndex( j );

            QAbstractItemModel * model =  m_ui->treeObjects->model();
            if (!model) {
                qDebug("model is null\n");
            }

            if (pos != index.row()) {
                model->removeRow( index.row() );
                model->insertRow(pos);
            }

            QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( pos );
            m_ui->treeObjects->setCurrentItem( item );
            updateIcon( item, j );

            gf.setDirty(true);
        }
    }

   delete d;
}

void CDlgGame::on_btnAddObject_clicked()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);

    CDlgObject *d = new CDlgObject (this);
    d->setGameDB(m_gameFile);

    int j = gf.m_arrProto.getSize();
    CProto proto;
    sprintf(proto.m_szName, q2c(tr("object_%d")), j);
    gf.m_arrProto.add( proto );

    d->load( j );
    if (d->exec() == QDialog::Accepted) {
        qDebug ( "j= %d\n ", j);
        d->save( j );
        gf.m_arrProto.resizeIndex( gf.m_arrProto.getSize() );
        int pos = gf.m_arrProto.insertIndex( gf.m_arrProto.getSize() - 1 );

        QAbstractItemModel * model =  m_ui->treeObjects->model();
        if (!model) {
            qDebug("model is null\n");
        }

        model->insertRow(pos);

        QTreeWidgetItem * item = m_ui->treeObjects->topLevelItem( pos );
        m_ui->treeObjects->setCurrentItem( item );
        updateIcon( item, gf.m_arrProto.getSize() - 1 );

        countFrameSetUses();

        gf.setDirty(true);
    } else {
        gf.m_arrProto.removeAt( j );
    }

    delete d;
}

void CDlgGame::on_btnDeleteObject_clicked()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    QModelIndex index = m_ui->treeObjects->currentIndex();
    int j = gf.m_arrProto.getAtIndex( index.row() );
    if (!j) {
        QMessageBox::warning(this, "", tr("This object cannot be deleted."), QMessageBox::Ok);
        return;
    }

    CProto & proto = gf.m_arrProto[ j ];

    QMessageBox::StandardButton
            ret = QMessageBox::warning(this, "", tr("Are you sure that you want to delete\n"
                                          "#%1 `%2`?").arg( j ) . arg( proto.getName() ),
                                       QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {

        // remove visual
        QAbstractItemModel * model =  m_ui->treeObjects->model();
        model->removeRow( index.row() );

        // remove object from index
        gf.m_arrProto.removeFromIndex ( j );

        // remove object & delete all references
        gf.killProto( j );
        gf.setDirty( true );
        countFrameSetUses();
    }
}

void CDlgGame::updateIcon(QTreeWidgetItem * itm, int protoId)
{
    QTreeWidgetItem * item = (QTreeWidgetItem *) itm;
    CGameFile & gf = *m_gameFile;
    CProto & proto = gf.m_arrProto[ protoId ];

    UINT8 *png;
    int size;
    gf.toFrame(proto.m_nFrameSet, proto.m_nFrameNo).toPng(png, size);

    QImage img;
    if (!img.loadFromData( png, size )) {
        qDebug("failed to load png\n");
    }
    delete [] png;

    QPixmap pm = QPixmap::fromImage(img);
    QIcon icon;
    icon.addPixmap(pm, QIcon::Normal, QIcon::On);

    item->setText(0, proto.m_szName);
    if (gf.m_className[proto.m_nClass].empty()) {
        item->setText(1, tr("Unknown class %1").arg( proto.m_nClass ));
    } else {
        item->setText(1, gf.m_className[proto.m_nClass].c_str());
    }
    item->setIcon(0, icon);
}

void CDlgGame::updateObjects (int OldframeSet, int newFrameSet)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    for (int i = 0; i < gf.m_arrProto.getSize(); ++i) {
        int protoId = gf.m_arrProto.getAtIndex( i );
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
    countFrameSetUses();
}

void CDlgGame::on_treeObjects_itemSelectionChanged()
{
    updateButtons();
}


/////////////////////////////////////////////////////////////////////////////
// page 3: sounds

void CDlgGame::on_treeSounds_doubleClicked(QModelIndex index)
{
    CGame & gf = *((CGame*)m_gameFile);

    if (!index.row()) {
        QMessageBox::warning(this, "", tr("This sound cannot be edited."),  QMessageBox::Ok);
        return;
    }

    QString wavFilter = tr("Wav sounds (*.wav)");
    QString oggFilter = tr("OggSound (*.ogg)");
    QString allFilter = tr("Popular sound files (*.wav *.ogg *.flac *.aiff *.au)");
    static QString fileFilter = allFilter + ";;" + wavFilter + ";;" + oggFilter ;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Replace sound..."), "", fileFilter, &selectedFilter);
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

            CSnd snd ( (* gf.m_arrSounds[ index.row() ]) );
            // replace the sound data
            snd.replace(data, size);
            // rename the snd to the filename
            snd.setName(q2c(QFileInfo(fileName).fileName().mid(0, 31)));
            if ((*gf.m_arrSounds[ index.row()]) != snd) {
                CSnd *oldSnd = gf.m_arrSounds[ index.row()];
                // remove old sound from ISound
                gf.sound()->remove(oldSnd->getUID());
                (gf.m_arrSounds[ index.row()] )->copy( snd );
                // add sound to ISound
                CSnd *newSnd = gf.m_arrSounds[ index.row()];
                gf.sound()->add((unsigned char*)newSnd->getData(), newSnd->getSize(), newSnd->getUID());
                (*gf.m_arrSounds[ index.row()]).debug();
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

void CDlgGame::on_btnAddSound_clicked()
{
    CGame & gf = *((CGame*)m_gameFile);
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

void CDlgGame::on_btnDeleteSound_clicked()
{
    CGame & gf = *((CGame*)m_gameFile);
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
        // remove sound from ISound
        gf.sound()->remove(snd->getUID());

        gf.m_arrSounds.removeAt( index.row() );
        delete snd;

        gf.setDirty( true );
    }
}

void CDlgGame::on_treeSounds_itemSelectionChanged()
{
    updateButtons();
}

/////////////////////////////////////////////////////////////////////////////
// page 4

void CDlgGame::on_treeFrameSets_doubleClicked(QModelIndex index)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    if (index.row() < 1) {
        QMessageBox::warning(this, "", tr("This ImageSet cannot be edited."), QMessageBox::Ok);
        return;
    }

    CDlgFrameSet * d = new CDlgFrameSet (this);
    d->setWindowTitle ( QString(tr("Edit Image Set `%1`")).arg(gf.frames()[index.row()]->getName()) );
    CFrameSet * frameSet = new CFrameSet (gf.frames()[index.row()]);
    d->init(frameSet);    
    if (d->exec() == QDialog::Accepted) {
        d->save();

        qDebug("on_treeFrameSets_doubleClicked(QModelIndex index) save\n");
        gf.setDirty( true );

        delete gf.frames()[ index.row() ];
        gf.frames().setAt( index.row(), frameSet);

        // update the imageCache
        gf.cache()->replace(index.row(), frameSet);

        QTreeWidgetItem * item = m_ui->treeFrameSets->topLevelItem( index.row() );
        countFrameSetUses();
        updateIconFrameSet( item, index.row() );
        updateObjects (index.row(), -1);

    } else {
        delete frameSet;
    }

    delete d;
}

void CDlgGame::on_btnAddFrameSet_clicked()
{
    CWizFrameSet *wiz = new CWizFrameSet( this );
    CGameFile & gf = *((CGameFile*)m_gameFile);
    int i = gf.frames().getSize();
    wiz->init(i);
    if (wiz->exec()) {
        CFrameSet *frameSet = new CFrameSet (wiz->getFrameSet());
        char name[32];
        strcpy(name, wiz->getName());
        frameSet->setName(name);
        // add new FrameSet
        gf.frames().add(frameSet);
        // add this new imageSet to the cache
        gf.cache()->add(frameSet);
        // update GUI
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        countFrameSetUses();
        updateIconFrameSet( item, i );
        m_ui->treeFrameSets->addTopLevelItem( item );
        m_ui->treeFrameSets->setCurrentItem( item );
        // set the doc to dirty
        gf.setDirty( true );        
    }
    delete wiz;
}

void CDlgGame::on_btnDeleteFrameSet_clicked()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    QModelIndex index = m_ui->treeFrameSets->currentIndex();
    if (index.row() < 1) {
        QMessageBox::warning(this, "", tr("This ImageSet cannot be deleted."), QMessageBox::Ok);
        return;
    }

    const CFrameSet & frameSet = gf.toFrameSet(index.row());

    QMessageBox::StandardButton ret = QMessageBox::warning(this,  "",  tr("Are you sure that you want to delete\n"
                                                                          "`%1` ?") .arg(frameSet.getName()),
                                                           QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        // remove visual
        QAbstractItemModel * model =  m_ui->treeFrameSets->model();
        model->removeRow( index.row() );
        // update ObjectList
        updateObjects ( index.row(), 0 );
        // delete all references
        gf.killFrameSet ( index.row() );
        // remove frameSet
        gf.frames().removeAt( index.row() );
        // remove imageSet from cache
        gf.cache()->removeAt( index.row() );

        gf.setDirty( true );
        countFrameSetUses();
    }
}

void CDlgGame::updateIconFrameSet(QTreeWidgetItem * itm, int fs)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    QTreeWidgetItem * item = (QTreeWidgetItem *) itm;

    CFrameSet & frameSet = gf.toFrameSet(fs);
    UINT8 *png;
    int size;
    gf.toFrame(fs, 0).toPng(png, size);

    QImage img;
    if (!img.loadFromData( png, size )) {
        qDebug("failed to load png\n");
    }
    delete [] png;

    QPixmap pm = QPixmap::fromImage(img);
    QIcon icon;
    icon.addPixmap(pm, QIcon::Normal, QIcon::On);

    QString s;
    if (frameSet.getSize() < 2) {
        s = QString (tr("%1 image")).arg(frameSet.getSize());
    } else {
        s = QString (tr("%1 images")).arg(frameSet.getSize());
    }
    item->setText(0, frameSet.getName());
    item->setText(1, s);
    item->setText(2, m_countFrameSetUses[fs] ? "used": "unused");
    item->setIcon(0, icon);
}

void CDlgGame::on_treeFrameSets_itemSelectionChanged()
{
    updateButtons();
}

/////////////////////////////////////////////////////////////////////////////
// page 5

void CDlgGame::on_treeLevels_doubleClicked(QModelIndex index)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    int level = index.row();
    CLevel & levelObj = *gf.m_arrLevels[ level ];
    CDlgEditLevel *dlg = new CDlgEditLevel(this);
    dlg->setGameDB(m_gameFile);
    QString s;
    s.sprintf(q2c(tr("Edit level %.3d")), level + 1);
    dlg->setWindowTitle(s);
    dlg->load(& levelObj);
    if (dlg->exec() == QDialog::Accepted) {
        dlg->save(& levelObj);
        gf.setDirty( true );
        QTreeWidgetItem * item = m_ui->treeLevels->topLevelItem( level );
        s.sprintf("%.3d", level + 1);
        item->setText(0, s);
        item->setText(1, levelObj.getSetting("title"));
    }

    delete dlg;
}

void CDlgGame::on_treeLevels_itemSelectionChanged()
{
    updateButtons();
}

void CDlgGame::on_btnAddLevel_clicked()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    CDlgEditLevel *dlg = new CDlgEditLevel(this);
    dlg->setGameDB(m_gameFile);
    dlg->setWindowTitle(tr("Create New Level"));
    CLevel * level = new CLevel;
    level->setSetting("title", q2c(tr("untitled %1").arg(gf.getSize() + 1)));
    level->setSetting("hint", q2c(tr("put your hints here...")));
    dlg->load(level);
    if (dlg->exec() == QDialog::Accepted) {
        dlg->save( level );
        gf.addLevel( level );
        gf.setDirty( true );
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        QString s;
        s.sprintf("%.3d", gf.getSize());
        item->setText(0, s);
        item->setText(1, level->getSetting("title"));
        m_ui->treeLevels->addTopLevelItem(item);
        updateButtons();
    } else {
        delete level;
    }

    delete dlg;
}

void CDlgGame::on_btnDeleteLevel_clicked()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    QModelIndex index = m_ui->treeLevels->currentIndex();
    int level = index.row();

    if (QMessageBox::warning(this, "", tr("Are you sure that you want to delete this level?"),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
        gf.setDirty( true );
        delete gf[ level ] ;
        gf.removeLevel( level );
        if ( gf.m_nCurrLevel > gf.getSize() - 1) {
            -- gf.m_nCurrLevel ;
        }

        m_ui->treeLevels->model()->removeRow(level);

        for (int i = level; i < gf.getSize(); ++i) {
            QTreeWidgetItem * item = m_ui->treeLevels->topLevelItem( i );
            QString s;
            s.sprintf("%.3d", i + 1);
            item->setText(0, s);
            item->setText(1, gf[i]->getSetting("title"));
        }

        updateButtons();
    }    
}

/////////////////////////////////////////////////////////////////////////////
// page 6

void CDlgGame::on_treeEvents_doubleClicked(QModelIndex index)
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    CGameEvents & events = *(gf.getEvents());
    CDlgSource *d = new CDlgSource ( (QWidget*) parent());
    d->init(m_gameFile);
    d->setWindowTitle(tr("Edit Event `%1`").arg(CGameEvents::getName(index.row())));
    d->setText(gf.getEvents()->getEvent(index.row()));
    if (d->exec()) {

        if (strcmp(events.getEvent(index.row()), q2c(d->getText()))) {
            events.setEvent(index.row(), q2c(d->getText()));
            gf.setDirty( true );
        }

        QTreeWidgetItem * item = m_ui->treeEvents->topLevelItem( index.row() );

        if (d->getText().isEmpty()) {
            item->setText(1, "");
        } else {
            item->setText(1, tr("[event procedure]"));
        }
    }

    delete d;
}

void CDlgGame::on_btnClearAll_clicked()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);
    CGameEvents & events = *(gf.getEvents());
    if (QMessageBox::warning(this, "", tr("Are you sure that you want to clear all the events"),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
        for (int i = 0; i < events.getSize(); ++i) {
            if (events.getEvent(i)[0]) {
                QTreeWidgetItem * item = m_ui->treeEvents->topLevelItem( i );
                item->setText(1, "");
                gf.setDirty( true );
                events.setEvent(i,"");
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// shared

void CDlgGame::updateButtons()
{
    CGameFile & gf = *((CGameFile*)m_gameFile);

    // page 2
    QModelIndex indexObject = m_ui->treeObjects->currentIndex();
    int j = gf.m_arrProto.getAtIndex( indexObject.row() );
    m_ui->btnDeleteObject->setEnabled( j );

    // page 3
    QModelIndex indexSound = m_ui->treeSounds->currentIndex();
    m_ui->btnDeleteSound->setEnabled( indexSound.row() > 0);

    // page 4
    QModelIndex indexFrameSet = m_ui->treeFrameSets->currentIndex();
    m_ui->btnDeleteFrameSet->setEnabled( indexFrameSet.row() > 0);

    // page 5
    QModelIndex indexLevel = m_ui->treeLevels->currentIndex();
    m_ui->btnDeleteLevel->setEnabled( indexLevel.row() >= 0);
}

void CDlgGame::countFrameSetUses()
{
    if (m_countFrameSetUses) {
        delete [] m_countFrameSetUses;
    }
    m_countFrameSetUses = m_gameFile->countFrameSetUses();
}

void CDlgGame::init()
{
    CGameFile &gf = *((CGameFile*)m_gameFile);

    // page 1: settings

    m_ui->treeSettings->setColumnCount(2);
    m_ui->treeSettings->setColumnWidth(0, 128);
    m_ui->treeSettings->setEditTriggers(0);
    m_ui->treeSettings->setWordWrap(false);
    m_ui->treeSettings->setRootIsDecorated(false);
    m_ui->treeSettings->setAlternatingRowColors(true);

    fillTreeSettings();

    // page 2: objects

    m_ui->treeObjects->setColumnCount(2);
    m_ui->treeObjects->setColumnWidth(0, 128);
    m_ui->treeObjects->setEditTriggers(0);
    m_ui->treeObjects->setWordWrap(false);
    m_ui->treeObjects->setRootIsDecorated(false);
    m_ui->treeObjects->setAlternatingRowColors(true);

    if (!gf.m_arrProto.isIndexed()) {
        gf.m_arrProto.indexArray();
    }

    for (int i = 0; i < gf.m_arrProto.getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        updateIcon(item, gf.m_arrProto.getAtIndex( i ) );
        m_ui->treeObjects->addTopLevelItem(item);
    }

    // page 3 : sounds

    m_ui->treeSounds->setColumnCount(2);
    m_ui->treeSounds->setColumnWidth(0, 128);
    m_ui->treeSounds->setColumnWidth(1, 64);
    m_ui->treeSounds->setEditTriggers(0);
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


    // page 4: frame sets

    m_ui->treeFrameSets->setColumnCount(3);
    m_ui->treeFrameSets->setColumnWidth(0, 128);
    m_ui->treeFrameSets->setColumnWidth(1, 96);
    m_ui->treeFrameSets->setEditTriggers(0);
    m_ui->treeFrameSets->setWordWrap(false);
    m_ui->treeFrameSets->setRootIsDecorated(false);
    m_ui->treeFrameSets->setAlternatingRowColors(true);
    countFrameSetUses();

    for (int i = 0; i < gf.frames().getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        updateIconFrameSet( item, i );
        m_ui->treeFrameSets->addTopLevelItem(item);
    }

    // page 5: levels

    m_ui->treeLevels->setColumnCount(2);
    m_ui->treeLevels->setColumnWidth(0, 48);
    m_ui->treeLevels->setColumnWidth(1, 128);
    m_ui->treeLevels->setEditTriggers(0);
    m_ui->treeLevels->setWordWrap(false);
    m_ui->treeLevels->setRootIsDecorated(false);
    m_ui->treeLevels->setAlternatingRowColors(true);

    for (int i = 0; i < gf.getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        QString s;
        s.sprintf("%.3d", i + 1);
        item->setText(0, s);
        item->setText(1, gf[i]->getSetting("title"));
        m_ui->treeLevels->addTopLevelItem(item);
    }

    // page 6: events

    m_ui->treeEvents->setColumnCount(2);
    m_ui->treeEvents->setColumnWidth(0, 128);
    m_ui->treeEvents->setColumnWidth(1, 96);
    m_ui->treeEvents->setEditTriggers(0);
    m_ui->treeEvents->setWordWrap(false);
    m_ui->treeEvents->setRootIsDecorated(false);
    m_ui->treeEvents->setAlternatingRowColors(true);

    CGameEvents & events = *(gf.getEvents());
    for (int i = 0; i < events.getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, events.getName(i));
        if (strcmp(events.getEvent(i),"")) {
            item->setText(1, tr("[event procedure]"));
        }
        m_ui->treeEvents->addTopLevelItem(item);
    }

    updateButtons();
}

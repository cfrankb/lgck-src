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

#include "DlgEntry.h"
#include "DlgObject.h"
#include "ui_DlgEntry.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/Frame.h"
#include "../shared/StringTable.h"
#include "../shared/Layer.h"
#include "../shared/Level.h"
#include "../shared/PathBlock.h"
#include "../shared/Path.h"
#include "../shared/qtgui/cheat.h"
#include <stdio.h>

CDlgEntry::CDlgEntry(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgEntry)
{
    m_ui->setupUi(this);
    m_currImage = 0;
    m_deletePath = false;
    m_path = new CPath;
}

CDlgEntry::~CDlgEntry()
{
    delete m_ui;
    delete m_path;
}

void CDlgEntry::changeEvent(QEvent *e)
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

void CDlgEntry::init(const int *triggers)
{
    QIcon iconBlank;
    iconBlank.addFile(":/images/blank.png");
    QIcon iconCheck;
    iconCheck.addFile(":/images/check.png");

    for (int n = 0; n <= TRIGGER_KEYS; ++n) {
        if (n) {
            QString s = QString(tr("%1")).arg(n);
            if (triggers[n]) {
                m_ui->cbTriggerKey->addItem(iconCheck, s);
            } else {
                m_ui->cbTriggerKey->addItem(iconBlank, s);
            }
        }
        else {
            m_ui->cbTriggerKey->addItem(tr("(none)"));
        }
    }

    m_ui->tabWidget->setCurrentIndex(0);
}

void CDlgEntry::load(const int entryPos)
{
    m_entry = entryPos;
    CGameFile &gf = *((CGameFile*)m_gameFile);
    CLevel & level = (* gf[gf.m_nCurrLevel]);
    CLayer & layer = * level.getCurrentLayer();
    CLevelEntry & entry = layer[entryPos];
    m_ui->cbTriggerKey->setCurrentIndex( entry.m_nTriggerKey & TRIGGER_KEYS );

    // attributs

    if (entry.m_nTriggerKey & 0x80) {
       m_ui->cHidden->setChecked( true );
    }

    if (entry.m_nTriggerKey & 0x40) {
       m_ui->cFrozen->setChecked( true );
    }

    if (entry.m_nTriggerKey & 0x20) {
       m_ui->cExtra2->setChecked( true );
    }

    //if (entry.m_nTriggerKey & 0x10) {
       //m_ui->cExtra1->setChecked( true );
    //}

    // difficulty levels

    if (entry.m_nActionMask & 0x1) {
       m_ui->cD01->setChecked( true );
    }

    if (entry.m_nActionMask & 0x2) {
       m_ui->cD02->setChecked( true );
    }

    if (entry.m_nActionMask & 0x4) {
       m_ui->cD04->setChecked( true );
    }

    if (entry.m_nActionMask & 0x8) {
       m_ui->cD08->setChecked( true );
    }

    // trigger flip

    if (entry.m_nActionMask & 0x20) {
       m_ui->cF20->setChecked( true );
    }

    if (entry.m_nActionMask & 0x40) {
       m_ui->cF40->setChecked( true );
    }

    if (entry.m_nActionMask & 0x80) {
       m_ui->cF80->setChecked( true );
    }

    // setImage
    setImage(entry.m_nFrameSet, entry.m_nFrameNo);
    m_currImage = entry.m_nFrameNo;
    m_currSet = entry.m_nFrameSet;
    m_proto = entry.m_nProto;
    updateText();
    enableCheckboxes();
    if (entry.m_string) {
       CStringTable *table = gf.getStringTable();
       m_ui->eHint->setPlainText(table->get(entry.m_string));
    }

    QString pathId = QString::asprintf("0x%.4x", entry.m_path);
    QString s = QString(tr("path: %1").arg(pathId));
    m_ui->sPath->setText(s);
    CPathBlock & paths = *(gf.getPaths());
    bool playback = false;
    m_ui->cPlayback->setChecked(entry.m_playback & CPathBlock::PB_PLAYBACK);
    m_ui->cApplyActive->setChecked(entry.m_playback & CPathBlock::PB_APPLY_ACTIVE);
    m_ui->cAttackPlayer->setChecked(entry.m_playback & CPathBlock::PB_ATTACK_PLAYER);
    m_ui->cIgnoreGravity->setChecked(entry.m_playback & CPathBlock::PB_IGNORE_GRAVITY);
    m_ui->cIgnoreSolid->setChecked(entry.m_playback & CPathBlock::PB_IGNORE_SOLID);
    m_ui->cLoop->setChecked(entry.m_playback & CPathBlock::PB_LOOP);
    m_ui->cReturnOrigin->setChecked(entry.m_playback & CPathBlock::PB_RETURN_ORIGIN);
    m_ui->cCustomHandler->setChecked(entry.m_playback & CPathBlock::PB_CUSTOM_HANDLER);
    if (entry.m_path) {
        CPath & path = paths[entry.m_path];
        s = QString(tr("path size: %1 move(s)").arg(path.getSize()));
        playback = m_ui->cPlayback->isChecked();
        *m_path = path;
    } else {
        s = QString(tr("path size: %1 move").arg(0));
        m_ui->btnDeletePath->setEnabled(false);
        m_ui->cPlayback->setEnabled(false);
        m_ui->btnRawPath->setEnabled(false);
    }
    m_ui->sPathSize->setText(s);
    enablePlaybackOptions(playback);
}

void CDlgEntry::enablePlaybackOptions(bool enable)
{
    m_ui->cApplyActive->setEnabled(enable);
    m_ui->cAttackPlayer->setEnabled(enable);
    m_ui->cCustomHandler->setEnabled(enable);
    m_ui->cIgnoreGravity->setEnabled(enable);
    m_ui->cLoop->setEnabled(enable);
    m_ui->cReturnOrigin->setEnabled(enable);
    m_ui->cIgnoreSolid->setEnabled(enable);
    m_ui->cbCustomHandler->setEnabled(enable
        && m_ui->cCustomHandler->isChecked());
}

void CDlgEntry::updateText()
{   
    CGameFile &gf = *((CGameFile*)m_gameFile);
    CProto & proto = gf.m_arrProto[ m_proto ];
    QString sName = QString(tr("%1 - %2")).arg(m_entry).arg(proto.getName());
    m_ui->sObject->setText( sName );
    m_ui->sClass->setText( gf.m_className [ proto.m_nClass ].c_str() );
    this->setWindowTitle(QString(tr("Custom instance of `%1`")).arg(proto.getName()));
}

void CDlgEntry::enableCheckboxes()
{
   CGameFile &gf = *((CGameFile*)m_gameFile);
   CProto & proto = gf.m_arrProto[ m_proto ];
   bool result = proto.m_nClass >= 0x10;

   // trigger key
   m_ui->cbTriggerKey->setEnabled( result );

   // attributs
   m_ui->cHidden->setEnabled( result ) ;
   m_ui->cFrozen->setEnabled( result ) ;
   m_ui->cExtra2->setEnabled( result ) ;
   //m_ui->cExtra1->setEnabled( result ) ;

   // trigger flip
   m_ui->cF20->setEnabled( result ) ;
   m_ui->cF40->setEnabled( result ) ;
   m_ui->cF80->setEnabled( result ) ;
}

void CDlgEntry::save(const int entryPos)
{
    CGameFile &gf = *((CGameFile*)m_gameFile);

    CLevel & level = (* gf[gf.m_nCurrLevel]);
    CLayer & layer = * level.getCurrentLayer();
    CLevelEntry & entry = layer[entryPos];
    entry.m_nTriggerKey = m_ui->cbTriggerKey->currentIndex();

    // attributs

    if (m_ui->cHidden->isChecked()) {
       entry.m_nTriggerKey += 0x80;
    }

    if (m_ui->cFrozen->isChecked()) {
       entry.m_nTriggerKey += 0x40;
    }

    if (m_ui->cExtra2->isChecked()) {
       entry.m_nTriggerKey += 0x20;
    }

    // difficulty levels

    entry.m_nActionMask = 0;

    if (m_ui->cD01->isChecked()) {
       entry.m_nActionMask += 0x1;
    }

    if (m_ui->cD02->isChecked()) {
       entry.m_nActionMask += 0x2;
    }

    if (m_ui->cD04->isChecked()) {
       entry.m_nActionMask += 0x4;
    }

    if (m_ui->cD08->isChecked()) {
       entry.m_nActionMask += 0x8;
    }

    // trigger flip

    if (m_ui->cF20->isChecked()) {
       entry.m_nActionMask += 0x20;
    }

    if (m_ui->cF40->isChecked()) {
       entry.m_nActionMask += 0x40;
    }

    if (m_ui->cF80->isChecked()) {
       entry.m_nActionMask += 0x80;
    }

    entry.m_nFrameSet = m_currSet;
    entry.m_nFrameNo = m_currImage;

    // save string to string table
    CStringTable *table = gf.getStringTable();
    QString str = m_ui->eHint->toPlainText();
    str = str.trimmed();
    if (entry.m_string) {
        const char* s = q2c(str);
        entry.m_string = table->set(entry.m_string, s);
    } else {
        if (!str.isEmpty()) {
            const char* s = q2c(str);
            entry.m_string = table->add(s);
        }
    }

    if (entry.m_path) {
        CPathBlock & paths = *(gf.getPaths());
        CPath & path = paths[entry.m_path];
        CPath & newPath = *m_path;
        if (m_deletePath || !newPath.getSize()) {
            path.forget();
            entry.m_path = 0;
        } else {
            path = newPath;
        }
    }

    unsigned int pathOptions = 0;
    pathOptions += m_ui->cPlayback->isChecked() ? CPathBlock::PB_PLAYBACK : 0;
    pathOptions += m_ui->cApplyActive->isChecked() ? CPathBlock::PB_APPLY_ACTIVE : 0;
    pathOptions += m_ui->cAttackPlayer->isChecked() ? CPathBlock::PB_ATTACK_PLAYER : 0;
    pathOptions += m_ui->cIgnoreGravity->isChecked() ? CPathBlock::PB_IGNORE_GRAVITY : 0;
    pathOptions += m_ui->cIgnoreSolid->isChecked() ? CPathBlock::PB_IGNORE_SOLID : 0;
    pathOptions += m_ui->cLoop->isChecked() ? CPathBlock::PB_LOOP : 0;
    pathOptions += m_ui->cReturnOrigin->isChecked() ? CPathBlock::PB_RETURN_ORIGIN : 0;
    pathOptions += m_ui->cCustomHandler->isChecked() ? CPathBlock::PB_CUSTOM_HANDLER : 0;
    entry.m_playback = pathOptions;
}

void CDlgEntry::on_btnObject_clicked()
{
    CGameFile &gf = *((CGameFile*)m_gameFile);
    CProto proto = gf.m_arrProto[m_proto];
    CDlgObject *d = new CDlgObject (this);
    d->setGameDB(m_gameFile);
    d->load( m_proto );
    if (d->exec() == QDialog::Accepted) {
        d->save( m_proto );
        if (proto != gf.m_arrProto[ m_proto ]) {
            gf.setDirty(true);
            updateText();
            enableCheckboxes();
            // setImage
            m_currSet = proto.m_nFrameSet;
            m_currImage = proto.m_nFrameNo;
            setImage(m_currSet, m_currImage);
        }
    }
    delete d;
}

void CDlgEntry::setImage(int frameSet, int frameNo)
{
    CFrameSet & fs = m_gameFile->toFrameSet(frameSet);
    uint8_t *png;
    int size;
    fs[frameNo]->toPng(png, size);
    QImage img;
    if (!img.loadFromData( png, size )) {
        qDebug("failed to load png $$\n");
    }
    delete [] png;
    QPixmap pm = QPixmap::fromImage(img);
    m_ui->sImage->setPixmap(pm);
}

void CDlgEntry::on_btnNextImage_clicked()
{
    CFrameSet & fs = m_gameFile->toFrameSet(m_currSet);
    m_currImage = (m_currImage + 1) % fs.getSize();
    setImage(m_currSet, m_currImage);
}

void CDlgEntry::on_btnPrevImage_clicked()
{
    CFrameSet & fs = m_gameFile->toFrameSet(m_currSet);
    --m_currImage;
    if (m_currImage < 0) {
        m_currImage = std::max(fs.getSize() - 1, 0);
    }
    setImage(m_currSet, m_currImage);
}

void CDlgEntry::on_cPlayback_toggled(bool checked)
{
    enablePlaybackOptions(checked);
}

void CDlgEntry::on_btnDeletePath_clicked()
{
    m_ui->btnDeletePath->setEnabled(false);
    m_ui->btnRawPath->setEnabled(false);
    m_ui->cPlayback->setEnabled(false);
    m_deletePath = true;
    enablePlaybackOptions(false);
}

void CDlgEntry::on_cCustomHandler_clicked()
{
    enablePlaybackOptions(true);
}

void CDlgEntry::on_btnRawPath_clicked()
{
    QString s;
    CPath & path = *m_path;
    for (int i=0; i < path.getSize(); ++i) {
        if (i) {
            s += " ";
        }
        QString tmp = QString::asprintf("%2.2x", path[i]);
        s += tmp.mid(0,2);
    }

    bool ok = false;
    QString result = QInputDialog::getText(this, QString(""),
            tr("Edit path"), QLineEdit::Normal,
            s, &ok);
    if (ok) {
        CPath tmp;
        // remove trailing white spaces
        result = result.trimmed();
        // decode the string
        for (int k=0; k < result.length(); k +=3) {
            char aim = strtol(q2c(result.mid(k, 2)), NULL, 16);
            if (CPath::isValidAim(aim)) {
                tmp.add(aim);
            }
        }
        path = tmp;
        s = QString(tr("path size: %1 move(s)").arg(path.getSize()));
        if (!path.getSize()) {
            m_ui->btnDeletePath->setEnabled(false);
            m_ui->btnRawPath->setEnabled(false);
            s = QString(tr("path size: %1 move").arg(0));
        }
        m_ui->sPathSize->setText(s);
    }
}

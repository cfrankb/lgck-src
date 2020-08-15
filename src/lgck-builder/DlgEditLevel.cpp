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
﻿/*
    LGCK Builder GUI
    Copyright (C) 1999, 2013  Francois Blanchette

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

#include <QColorDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QClipboard>
#include "DlgEditLevel.h"
#include "DlgSource.h"
#include "ui_DlgEditLevel.h"
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/Game.h"
#include "../shared/Level.h"
#include "../shared/Settings.h"
#include "../shared/qtgui/cheat.h"

CDlgEditLevel::CDlgEditLevel(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgEditLevel)
{
    m_ui->setupUi(this);
    m_ui->tabWidget->setCurrentIndex(0);
    m_ui->btnColor->setBuddy(m_ui->bkColor);
    m_ui->btnBorderColor->setBuddy(m_ui->eBorderColor);
    m_ui->btnColorMod->setBuddy(m_ui->eColorMod);
    m_ui->btnIBkColor->setBuddy(m_ui->eIBkColor);
    m_ui->btnITextColor->setBuddy(m_ui->eITextColor);
    m_bNewLevel = false;
}

CDlgEditLevel::~CDlgEditLevel()
{
    delete m_ui;
}

void CDlgEditLevel::changeEvent(QEvent *e)
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

void CDlgEditLevel::setTitle(const QString title)
{
    m_ui->title->setText(title);
}

void CDlgEditLevel::setHint(const QString hint)
{
    m_ui->hint->setPlainText(hint);
}

void CDlgEditLevel::setColor(QLineEdit *edit, const QString color)
{
    edit->setText(color);
}

QString CDlgEditLevel::getTitle ()
{
    return m_ui->title->text().trimmed();
}

QString CDlgEditLevel::getHint()
{
    return m_ui->hint->toPlainText().trimmed();
}

QString CDlgEditLevel::getColor(QLineEdit *edit)
{
    return edit->text().mid(0,6);
}

void CDlgEditLevel::load(CLevel *s)
{
    CLevel & level = * s;
    m_level = s;

    // page 1

    setTitle(level.getSetting("title"));
    setHint(level.getSetting("hint"));
    m_ui->eUUID->setText(level.getSetting("uuid"));
    m_ui->eAuthor->setText(level.getSetting("author"));

    // page 5
    setColor(m_ui->bkColor, level.getSetting("bkcolor"));
    setColor(m_ui->eBorderColor, level.getSetting("borderColor"));
    setColor(m_ui->eIBkColor, level.getSetting("introBkColor"));
    setColor(m_ui->eITextColor, level.getSetting("introTextColor"));
    setColor(m_ui->eColorMod, level.getSetting("colorMod"));

    // page 2

    static QString endLevelOptions [] = {
        tr("goal completed"),
        tr("goal completed & player at top"),
        tr("goal completed & player at bottom"),
        tr("goal completed & player at left"),
        tr("goal completed & player at right"),
        tr("no automatic completion")
    };

    for (unsigned int i=0; i < sizeof(endLevelOptions)/sizeof(QString); ++i) {
        m_ui->cbEndLevel->addItem(endLevelOptions[i]);
    }

    int goal = strtol(level.getSetting("goal"), nullptr, 10);
    m_ui->cbEndLevel->setCurrentIndex( goal );

    for (int i=0; i < 32; ++i) {
        if (i != 0) {
            m_ui->cbTrigger->addItem(QString("%1").arg(i));
        } else {
            m_ui->cbTrigger->addItem(QString(tr("(none)")));
        }
    }

    int trigger = strtol(level.getSetting("trigger"), nullptr, 10);
    m_ui->cbTrigger->setCurrentIndex( trigger );

    int wrap = strtol(level.getSetting("wrap"), nullptr, 10);
    m_ui->cWrapUp->setChecked( wrap & CLevel::WRAP_UP );
    m_ui->cWrapDown->setChecked( wrap & CLevel::WRAP_DOWN );
    m_ui->cWrapLeft->setChecked( wrap & CLevel::WRAP_LEFT );
    m_ui->cWrapRight->setChecked( wrap & CLevel::WRAP_RIGHT );
    m_ui->cOpenTop->setChecked( wrap & CLevel::OPENTOP );

    int noGravity = strtol(level.getSetting("no_gravity"), nullptr, 10);
    m_ui->cNoGravity->setChecked( noGravity != 0 );

    // page3

    m_ui->eMusic->setText(level.getSetting("music"));
    m_ui->eTimeLimit->setText(level.getSetting("time"));
    m_ui->eTickRate->setText(level.getSetting("tick_rate"));
    m_ui->eClosure->setText(level.getSetting("closure"));
    m_ui->eTimeIntro->setText(level.getSetting("introTime"));

    // page4

    m_ui->treeEvents->setColumnCount(2);
    m_ui->treeEvents->setColumnWidth(0, 128);
    m_ui->treeEvents->setColumnWidth(1, 128);
    m_ui->treeEvents->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ui->treeEvents->setWordWrap(false);
    m_ui->treeEvents->setRootIsDecorated(false);
    m_ui->treeEvents->setAlternatingRowColors(true);

    for (int i = 0; i < level.getEventCount(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        item->setText(0, level.getEventName(i));
        if (level.getEvent(i)[0]) {
            item->setText(1, tr("[event procedure]"));
        }
        m_ui->treeEvents->addTopLevelItem(item);
    }

    // page6
    m_ui->eWidth->setText(level.getSetting("width"));
    m_ui->eHeight->setText(level.getSetting("height"));
    bool isChecked = level.getSettingInt("width") &&
            level.getSettingInt("height");
    m_ui->cCanvas->setChecked(isChecked);
    if (!isChecked) {
        m_ui->eWidth->setEnabled(false);
        m_ui->eHeight->setEnabled(false);
    }
    if (m_bNewLevel) {
        m_ui->btnWorldSize->setEnabled(false);
    }
}

void CDlgEditLevel::save(CLevel *s)
{
    CLevel & level = * s;

    // page 1
    level.setSetting("title", q2c(getTitle()));
    level.setSetting("hint", q2c(getHint()));
    level.setSetting("author", q2c(m_ui->eAuthor->text().trimmed()));

    // page5   
    level.setSetting("bkcolor", q2c(getColor(m_ui->bkColor)));
    level.setSetting("borderColor", q2c(getColor(m_ui->eBorderColor)));
    level.setSetting("introBkColor", q2c(getColor(m_ui->eIBkColor)));
    level.setSetting("introTextColor", q2c(getColor(m_ui->eITextColor)));
    level.setSetting("colorMod", q2c(getColor(m_ui->eColorMod)));

    // page 2
    level.setSetting("goal", q2c(QString("%1").arg(m_ui->cbEndLevel->currentIndex())));
    level.setSetting("trigger", q2c(QString("%1").arg(m_ui->cbTrigger->currentIndex())));
    int wrap = 0;
    if (m_ui->cWrapUp->checkState() == Qt::Checked) {
        wrap += CLevel::WRAP_UP;
    }

    if (m_ui->cWrapDown->checkState() == Qt::Checked) {
        wrap += CLevel::WRAP_DOWN;
    }

    if (m_ui->cWrapLeft->checkState() == Qt::Checked) {
        wrap += CLevel::WRAP_LEFT;
    }

    if (m_ui->cWrapRight->checkState() == Qt::Checked) {
        wrap += CLevel::WRAP_RIGHT;
    }

    if (m_ui->cOpenTop->checkState() == Qt::Checked) {
        wrap += CLevel::OPENTOP;
    }

    level.setSetting("wrap", q2c(QString("%1").arg(wrap)));

    int noGravity = (int) m_ui->cNoGravity->checkState() == Qt::Checked;
    level.setSetting("no_gravity", q2c(QString("%1").arg(noGravity)));

    // page 3

    level.setSetting("music", q2c(m_ui->eMusic->text()));
    level.setSetting("time", q2c(m_ui->eTimeLimit->text()));
    level.setSetting("tick_rate", q2c(m_ui->eTickRate->text()));
    level.setSetting("closure", q2c(m_ui->eClosure->text()));
    level.setSetting("introTime", q2c(m_ui->eTimeIntro->text()));

    // page 4
    // events

    // page 6
    if (m_ui->cCanvas->isChecked()) {
        level.setSetting("width", q2c(m_ui->eWidth->text()), 0xfff8);
        level.setSetting("height", q2c(m_ui->eHeight->text()), 0xfff8);
    } else {
        level.setSetting("width", "");
        level.setSetting("height", "");
    }
}

void CDlgEditLevel::on_treeEvents_doubleClicked(QModelIndex index)
{
    CGameFile & gf = *m_gameFile;
    CLevel & level = *m_level;

    if (index.row() < 0) {
        return;
    }

    CDlgSource *d = new CDlgSource((QWidget*) parent());
    d->init(m_gameFile);
    QString t = QString(tr("Edit Event ``%1``")).arg(CLevel::getEventName(index.row()));
    d->setWindowTitle(t);
    d->setText(level.getEvent(index.row()));
    if (d->exec() == QDialog::Accepted) {
        level.setEvent(index.row(), q2c(d->getText()));
        QTreeWidgetItem * item = m_ui->treeEvents->topLevelItem( index.row() );
        if (d->getText().isEmpty()) {
            item->setText(1, "");
        } else {
            item->setText(1, tr("[event procedure]"));
        }
        gf.setDirty( true );
    }

    delete d;
}

void CDlgEditLevel::on_cCanvas_toggled(bool checked)
{
    m_ui->eWidth->setEnabled(checked);
    m_ui->eHeight->setEnabled(checked);
}

void CDlgEditLevel::on_btnWorldSize_clicked()
{
    CGame *game = static_cast<CGame*>(m_gameFile);
    int width;
    int height;
    game->calculateWorldSize(m_level, width, height);
    m_ui->eWidth->setText(QString("%1").arg(width));
    m_ui->eHeight->setText(QString("%1").arg(height));
}

void CDlgEditLevel::setNewLevel()
{
    m_bNewLevel = true;
}

void CDlgEditLevel::on_btnColorMod_clicked()
{

}

void CDlgEditLevel::on_pushButton_clicked()
{
    QClipboard *clip = QApplication::clipboard();
    clip->setText(m_ui->eUUID->text());
}

void CDlgEditLevel::on_btnMusic_clicked()
{
    QString fileFilter = tr("Music files (*.ogg)");
    QString fileName = m_gameFile->m_path.c_str();
    fileName = QFileDialog::getOpenFileName(this, tr("Select Music File"), fileName, fileFilter);
    if (!fileName.isEmpty()) {
#ifdef Q_OS_WIN32
        fileName = fileName.replace("\\", "/");
#endif
        int len = m_gameFile->m_path.length();
        qDebug("sub: %s", q2c(fileName.mid(0, len)));
        qDebug("name: %s", q2c(fileName.mid(len)));
        if (fileName.mid(0, len) == m_gameFile->m_path.c_str()) {
            m_ui->eMusic->setText(fileName.mid(len));
        } else {

        }
    }
}

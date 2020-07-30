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

#include "DlgFrameSet.h"
#include "ui_DlgFrameSet.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QPushButton>
#include <QStringList>
#include <QProcess>
#include <QTemporaryFile>
#include "../shared/stdafx.h"
#include "../shared/FrameSet.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/FileWrap.h"
#include "launcher.h"

CDlgFrameSet::CDlgFrameSet(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgFrameSet)
{
    m_ui->setupUi(this);
    m_frameSet = NULL;
    m_ui->sUUID->setText("UUID: -");
}

CDlgFrameSet::~CDlgFrameSet()
{
    delete m_ui;
}

void CDlgFrameSet::changeEvent(QEvent *e)
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

void CDlgFrameSet::on_btnAddFrames_clicked()
{
    emit imagesAdded();
}

void CDlgFrameSet::on_btnDeleteFrame_clicked()
{
    emit imageDeleted();
}

void CDlgFrameSet::init(CFrameSet * frameSet)
{
    connect(this,
            SIGNAL(imageDeleted()),
            m_ui->treeWidget,
            SLOT(deleteImage()));

    connect(this,
            SIGNAL(imagesAdded()),
            m_ui->treeWidget,
            SLOT(addImages()));

    connect(this,
            SIGNAL(refill()),
            m_ui->treeWidget,
            SLOT(reload()));

    connect(m_ui->treeWidget,
            SIGNAL(countChanged()),
            this,
            SLOT(updateButtons()));

    m_frameSet = frameSet;
    m_ui->treeWidget->setFrameSet(frameSet);

    CFrameSet * fs = (CFrameSet *) m_frameSet;
    m_ui->eName->setText(fs->getName());
    m_ui->sUUID->setText(QString("UUID: ") + QString(fs->tag("UUID").c_str()));

    emit refill();
    updateButtons();

    // disable button if sprite editor not found
    QString appDir = QCoreApplication::applicationDirPath();

    Path outPath;
    bool found = getCmd(SPRITE_EDITOR, outPath);
    m_ui->btnEdit->setDisabled(!found);
}

void CDlgFrameSet::updateButtons()
{
    QModelIndex index = m_ui->treeWidget->currentIndex();
    m_ui->btnDeleteFrame->setEnabled( index.row() >= 0 );

    QPushButton *button=m_ui->buttonBox->button(QDialogButtonBox::Save);
    button->setEnabled(m_ui->treeWidget->topLevelItemCount()
         && !m_ui->eName->text().trimmed().isEmpty());
}

void CDlgFrameSet::save()
{
    CFrameSet * fs = (CFrameSet *) m_frameSet;
    fs->setName(q2c(m_ui->eName->text().trimmed().mid(0 , 31)));
}

void CDlgFrameSet::on_treeWidget_itemSelectionChanged()
{
    updateButtons();
}

void CDlgFrameSet::on_eName_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    updateButtons();
}

bool CDlgFrameSet::fileExists(QString & path)
{
    return QFileInfo::exists(path) && QFileInfo(path).isFile();
}

void CDlgFrameSet::on_btnEdit_clicked()
{
    if (QMessageBox::information(this, "", tr("You are about to edit this image set using the Sprite Editor.\nJust save and exit when you are done with your changes."),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {

         // set cursor to hourglass
         QApplication::setOverrideCursor(Qt::WaitCursor);

        // create temp file
        QString appDir = QCoreApplication::applicationDirPath();
        QTemporaryFile tmp(QDir::temp().absoluteFilePath("XXXXXXXXXXX.obl")) ;
        tmp.open();
        tmp.setAutoRemove(false);
        tmp.close();
        QString fileName = tmp.fileName();
        qDebug() << QString("temp file:") + fileName;

        // same image set to temp file
        CFileWrap file;
        if (file.open(q2c(tmp.fileName()),"wb")) {
            m_frameSet->write(file);
            file.close();
        }

        // lauch the editor
        Path outPath;
        getCmd(SPRITE_EDITOR, outPath); // we assume that it was found

        QStringList list = QStringList{fileName};
        QProcess proc;
        proc.setWorkingDirectory(outPath.path);
        proc.start(outPath.cmd, list);
        proc.waitForFinished(-1);

        // check the exitCode
        int result = proc.exitCode();
        if (result) {
            QStringList cmd = QStringList {outPath.path, outPath.cmd};
            QString outMsg = QString(tr("An error occured: %1\nCode: %2"))
                           .arg(cmd.join("/") + " " + list.join(" ")).arg(result);
            qDebug() << outMsg;
            QMessageBox::critical(this, "", outMsg);
        } else {
            // reload the changes
            if (file.open(q2c(tmp.fileName()),"rb")) {
                std::string uuid = m_frameSet->tag("UUID");
                m_frameSet->read(file);
                m_frameSet->setTag("UUID", uuid.c_str());
                file.close();
                emit refill();
            } else {
                QString outMsg = QString("Failed to read %1").arg(tmp.fileName());
                QMessageBox::critical(this, "", outMsg);
            }
        }
        tmp.setAutoRemove(true);
        // restore cursor
        QApplication::restoreOverrideCursor();
    }
}

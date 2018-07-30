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
#include <QFileDialog>
#include <QFileInfo>
#include <QPushButton>
#include <QStringList>
#include <QProcess>
#include <QTemporaryFile>
#include "../shared/stdafx.h"
#include "../shared/FrameSet.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/FileWrap.h"

CDlgFrameSet::CDlgFrameSet(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CDlgFrameSet)
{
    m_ui->setupUi(this);
    m_frameSet = NULL;
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

    emit refill();
    updateButtons();
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

void CDlgFrameSet::on_btnEdit_clicked()
{
    if (QMessageBox::information(this, "", tr("You are about to edit this image set using the Sprite Editor.\nJust save and exit when you are done with your changes."),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {

        // create temp file
        QString appDir = QCoreApplication::applicationDirPath();
        QTemporaryFile tmp ;
        tmp.setFileTemplate("XXXXXXXXXXXX.obl");
        tmp.open();
        tmp.setAutoRemove(false);
        tmp.close();
        QString fileName = tmp.fileName();
        qDebug() << QString("temp file:") + fileName;

        // same image set to temp file
        CFileWrap file;
        if (file.open(q2c(tmp.fileName()),"w")) {
            m_frameSet->write(file);
            file.close();
        }

        // lauch the editor
        QString path = appDir + "/obl5edit.exe";
        QProcess proc;
        proc.setWorkingDirectory(appDir);
        QStringList list;
        list << fileName;
        proc.start(path, list);
        proc.waitForFinished(-1);

        // check the exitCode
        int result = proc.exitCode();
        if (result) {
            QString outMsg = QString(tr("An error occured: %1\nCode: %2"))
                           .arg(path + " " + list.join(" ")).arg(result);
            qDebug() << outMsg;
            QMessageBox msgBox(QMessageBox::Critical, "", outMsg, 0, this);
            msgBox.exec();
        } else {
            // reload the changes
            if (file.open(q2c(tmp.fileName()),"r")) {
               m_frameSet->read(file);
                file.close();
                emit refill();
            } else {
                QString outMsg = QString("Failed to read %1").arg(tmp.fileName());
                QMessageBox msgBox(QMessageBox::Critical, "", outMsg, 0, this);
                msgBox.exec();
            }
        }
        tmp.setAutoRemove(true);
    }
}

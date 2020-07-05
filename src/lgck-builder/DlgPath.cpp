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

#include "DlgPath.h"
#include "ui_DlgPath.h"
#include "DlgPathDir.h"
#include <QInputDialog>
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/Path.h"
#include "../shared/FileWrap.h"
#include "../shared/qtgui/cheat.h"

QString CDlgPath::m_iconNames[] = {
  "aim-up.png",
  "aim-down.png",
  "aim-left.png",
  "aim-right.png",
  "aim-separator.png"
};

QString CDlgPath::m_iconText[] = {
    tr("UP"),
    tr("DOWN"),
    tr("LEFT"),
    tr("RIGHT"),
    tr("SEPARATOR")
};

int CDlgPath::m_iconValues[] = {
    CGameFile::UP,
    CGameFile::DOWN,
    CGameFile::LEFT,
    CGameFile::RIGHT,
    CGameFile::SEPARATOR
};

CDlgPath::CDlgPath(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgPath)
{
    ui->setupUi(this);
    m_path = (void*) new CPath;
}

CDlgPath::~CDlgPath()
{
    delete ui;
    delete (CPath*) m_path;
}

void CDlgPath::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CDlgPath::load(void *p)
{
    CPath *s = (CPath *)p;
    CPath *d = (CPath *)m_path;

    *d = *s;

    //qDebug("CDlgPath::load() >> path size: %d", d->getSize());
    for (int i=0; i < d->getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        updateIcon(item, (*d)[i]);
        ui->treeWidget->addTopLevelItem(item);
    //    qDebug(">> item added");
    }
}

void CDlgPath::save(void *p)
{
    CPath *s = (CPath *)m_path;
    CPath *d = (CPath *)p;

    *d = *s;
}

void CDlgPath::updateButtons()
{

}

void CDlgPath::on_btnAdd_clicked()
{
    CPath & path = * ((CPath*) m_path);
    CDlgPathDir *dlg = new CDlgPathDir( (QWidget*) parent());
    QString s = tr("Add Direction");
    dlg->setWindowTitle( s );
    dlg->load(-1);

    if (dlg->exec()) {
        char aim = -1;
        dlg->save(aim);
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        updateIcon(item, aim);
        ui->treeWidget->addTopLevelItem(item);
        path.add(aim);
    }

    delete dlg;

    updateButtons();
}

void CDlgPath::on_btnDelete_clicked()
{
    CPath & path = * ((CPath*) m_path);
    QModelIndex index = ui->treeWidget->currentIndex();
    int i = index.row();
    if (i != -1) {
        QAbstractItemModel * model =  ui->treeWidget->model();
        model->removeRow( index.row() );
        //path.debug();
        path.removeAt(i);
        //qDebug("path removeAt(%d)", i);
        //path.debug();
    }

    updateButtons();
}

void CDlgPath::on_treeWidget_doubleClicked(QModelIndex index)
{
    CPath & path = * ((CPath*) m_path);

    int idx = index.row();
    if (idx != -1) {
        CDlgPathDir *dlg = new CDlgPathDir( (QWidget*) parent());
        QString s = tr("Edit Direction");
        dlg->setWindowTitle( s );

        dlg->load(path[idx]);

        if (dlg->exec()) {
            dlg->save(path[idx]);
            QTreeWidgetItem *item = ui->treeWidget->topLevelItem( idx ) ;
            updateIcon(item, path[idx]);
        }

        delete dlg;
    }

    updateButtons();
}

void CDlgPath::on_treeWidget_itemSelectionChanged()
{
    updateButtons();
}

void CDlgPath::updateIcon(QTreeWidgetItem *item, int aim)
{
    int i = aim;
    if (i == -1) {
        i = 4;
    }

    CFileWrap file;

    QIcon icon;
    if (file.open(q2c(QString(":/images/%1").arg(m_iconNames[i])))) {

        int size = file.getSize();
        uint8_t *png = new uint8_t [ size ];
        file.read(png, size);
        file.close();

        QImage img;
        if (!img.loadFromData( png, size )) {
            qDebug("failed to load png\n");
        }

        QPixmap pm = QPixmap::fromImage(img);
        icon.addPixmap(pm, QIcon::Normal, QIcon::On);

        delete [] png;
    } else {
        qDebug("failed to load resource icon: %s\n", q2c(m_iconNames[i]));
    }

    item->setIcon(0, icon);
    item->setText(0, m_iconText[i]);
}

void CDlgPath::on_btnRaw_clicked()
{
    QString tmp;
    QString s;
    CPath & path = * ((CPath*) m_path);

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
//            qDebug("%s = %2.2x ", q2c(result.mid(k, 2)), aim);
            if (CPath::isValidAim(aim)) {
                tmp.add(aim);
            } else {

            }
        }

        // clear the path treeWidget
        QAbstractItemModel * model =  ui->treeWidget->model();
        for (int i=0; i<path.getSize(); ++i) {
            model->removeRow( 0 );
        }

        // fill treeWidget
        path = tmp;
        for (int i=0; i < path.getSize(); ++i) {
            QTreeWidgetItem *item = new QTreeWidgetItem(0);
            updateIcon(item, path[i]);
            ui->treeWidget->addTopLevelItem(item);
        }
    }
}

void CDlgPath::on_btnDuplicate_clicked()
{
    CPath & path = * ((CPath*) m_path);
    QModelIndex index = ui->treeWidget->currentIndex();
    int i = index.row();
    if (i != -1) {
        QAbstractItemModel * model =  ui->treeWidget->model();
        model->insertRow(i);
//        model->removeRow( index.row() );
//        path.debug();
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem( i ) ;
        int aim = path[i];
        path.insertAt(i, aim);
        updateIcon(item, path[i]);
        //path.removeAt(i);
//        qDebug("path removeAt(%d)", i);
//        path.debug();
    }
}

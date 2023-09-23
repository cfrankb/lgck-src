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
#include "DlgExportSprite.h"
#include "ui_DlgExportSprite.h"
#include "../shared/GameFile.h"
#include "../shared/Frame.h"
#include "../shared/qtgui/cheat.h"
#include "../shared/qtgui/qthelper.h"
#include "../shared/qtgui/qfilewrap.h"
#include "OBL5File.h"
#include <QFileDialog>
#include <QMessageBox>

QString CDlgExportSprite::m_outDir;

CDlgExportSprite::CDlgExportSprite(QWidget *parent, CGameFile *gf) :
    QDialog(parent),
    ui(new Ui::CDlgExportSprite)
{
    ui->setupUi(this);
    m_gameFile = gf;
    reloadSprites();
    updateBtnExport();
}

CDlgExportSprite::~CDlgExportSprite()
{
    delete ui;
}

void CDlgExportSprite::reloadSprites()
{
    CGameFile &gf = * m_gameFile;
    ui->treeObjects->setColumnCount(2);
    int x,y,w,h;
    ui->treeObjects->geometry().getRect(&x, &y, &w, &h);
    ui->treeObjects->setColumnWidth(0, 32);
    ui->treeObjects->setColumnWidth(1, 96);
    ui->treeObjects->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeObjects->setWordWrap(true);
    ui->treeObjects->setRootIsDecorated(false);
    ui->treeObjects->setIconSize(QSize(32,32));
    ui->treeObjects->setLineWidth(48);
    ui->treeObjects->setAllColumnsShowFocus(true);
    ui->treeObjects->setBaseSize(32,32);
    ui->treeObjects->setUniformRowHeights(false);
    ui->treeObjects->setAlternatingRowColors(true);
    ui->treeObjects->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeObjects->enableDrag(false);
    for (int i = 1; i < gf.frames().getSize(); ++i) {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);
        updateIcon(item, i );
        ui->treeObjects->addTopLevelItem(item);
    }
    connect(ui->treeObjects->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(updateBtnExport()));
    ui->cbFormat->addItem("PNG");
    ui->cbFormat->addItem("OBL5");
    ui->cbFormat->addItem("META");
}

void CDlgExportSprite::updateIcon(QTreeWidgetItem * item, int protoId)
{
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
    CProto & proto = gf.toProto(protoId);

    QIcon icon = frame2icon(gf.toFrame(proto.m_nFrameSet, proto.m_nFrameNo));
    icon.actualSize(QSize(32,32));
    QString className;
    if (gf.m_className[proto.m_nClass].empty()) {
        className = QString(tr("Unknown class %1").arg( proto.m_nClass ));
    } else {
        className = gf.m_className[proto.m_nClass].c_str();
    }
    QString label = QString("%1\n%2").arg(proto.m_szName, className);
    item->setIcon(0, icon);
    item->setText(1, label);
}

int CDlgExportSprite::askQuestion(QString filename)
{
    QMessageBox::StandardButton ret = QMessageBox::warning(
        this, "",
        tr("File already exists: %1.\nReplace?").arg(filename),
        QMessageBox::Yes | QMessageBox::YesAll | QMessageBox::No |
            QMessageBox::NoAll | QMessageBox::Cancel);
    return ret;
}

void CDlgExportSprite::on_btnExport_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Destination"),
         m_outDir,
         QFileDialog::ShowDirsOnly
         | QFileDialog::DontResolveSymlinks);
    QString suffix;
    std::string outFormat;
    if (!dir.isEmpty()) {
        m_outDir = dir;
        typedef struct {
            char format[5];
            char suffix[6];
            bool isObl;
        } Format;

        Format formats[] = {
            {"PNG", "png", true},
            {"OBL5", "obl", true},
            {"META", "proto", false}
        };

        int formatId = ui->cbFormat->currentIndex();
        suffix = formats[formatId].suffix;
        outFormat = formats[formatId].format;

        QList<QTreeWidgetItem *> itemList;
        itemList = ui->treeObjects->selectedItems();

        bool noAll = false;
        bool yesAll = false;
        foreach(QTreeWidgetItem *item, itemList) {
           CGameFile & gf = *m_gameFile;
           ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
           CProto & proto = gf.toProto(data->protoId);
           QString fileName = QDir(dir).filePath(QString(proto.m_szName) + "." + suffix);

           if (QFile::exists(fileName) && !yesAll) {
               if (noAll) {
                   continue;
               }
               int ret = askQuestion(fileName);
               if (ret == QMessageBox::No || ret == QMessageBox::NoAll) {
                   noAll = ret == QMessageBox::NoAll;
                   continue;
               } else if (ret == QMessageBox::Cancel) {
                   break;
               } else if (ret == QMessageBox::YesAll){
                   yesAll = ret == QMessageBox::YesAll;
               }
           }

           if (formats[formatId].isObl) {
               CFrameSet & frameSet = gf.toFrameSet(proto.m_nFrameSet);
               COBL5File oblDoc;
               oblDoc.setFormat(outFormat.c_str());
               oblDoc.getImageSet() = frameSet;
               oblDoc.setFileName(fileName);
               oblDoc.write();
           } else {
               QFileWrap file;
               if (file.open(fileName, "wb")) {
                   m_gameFile->protos().exportMeta(file, data->protoId);
                   file.close();
               }
           }
        }
    }
}

void CDlgExportSprite::updateBtnExport()
{
    QList<QTreeWidgetItem *> itemList;
    itemList = ui->treeObjects->selectedItems();
    ui->btnExport->setEnabled(itemList.size()!=0);
}

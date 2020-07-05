#include "DlgExportSprite.h"
#include "ui_DlgExportSprite.h"
#include "../shared/stdafx.h"
#include "../shared/GameFile.h"
#include "../shared/Frame.h"
#include "OBL5File.h"
#include <QFileDialog>

static QString outDir;

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
    CProto & proto = gf.m_arrProto[ protoId ];

    uint8_t *png;
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

void CDlgExportSprite::on_btnExport_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Destination"),
         outDir,
         QFileDialog::ShowDirsOnly
         | QFileDialog::DontResolveSymlinks);
    QString suffix;
    char outFormat[5];
    if (!dir.isEmpty()) {
        outDir = dir;
        if (ui->cbFormat->currentIndex()==0) {
            strcpy(outFormat, "PNG");
            suffix = "png";
        } else {
            strcpy(outFormat, "OBL5");
            suffix = "obl";
        }
        QList<QTreeWidgetItem *> itemList;
        itemList = ui->treeObjects->selectedItems();
        COBL5File oblDoc;
        foreach(QTreeWidgetItem *item, itemList) {
           ITEM_DATA * data = (*item).data(0, Qt::UserRole).value<ITEM_DATA*>();
           CGameFile & gf = *m_gameFile;
           CProto & proto = gf.m_arrProto[ data->protoId ];
           CFrameSet & frameSet = gf.toFrameSet(proto.m_nFrameSet);
           QString fileName = QDir(dir).filePath(QString(proto.m_szName) + "." + suffix);
           oblDoc.setFormat(outFormat);
           oblDoc.getImageSet() = frameSet;
           oblDoc.setFileName(fileName);
           oblDoc.write();
        }
    }
}

void CDlgExportSprite::updateBtnExport()
{
    QList<QTreeWidgetItem *> itemList;
    itemList = ui->treeObjects->selectedItems();
    ui->btnExport->setEnabled(itemList.size()!=0);
}

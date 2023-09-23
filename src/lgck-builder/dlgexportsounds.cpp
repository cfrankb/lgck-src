#include "dlgexportsounds.h"
#include "ui_dlgexportsounds.h"
#include "../shared/GameFile.h"
#include "../shared/Snd.h"
#include "../shared/qtgui/qfilewrap.h"
#include <QListWidget>
#include <QFileDialog>
#include <QMessageBox>

QString CDlgExportSounds::m_outDir;

CDlgExportSounds::CDlgExportSounds(QWidget *parent, CGameFile *gf) :
    QDialog(parent),
    ui(new Ui::CDlgExportSounds)
{
    ui->setupUi(this);
    m_gameFile = gf;
    reloadSounds();
    updateBtnExport();
}

CDlgExportSounds::~CDlgExportSounds()
{
    delete ui;
}

void CDlgExportSounds::reloadSounds()
{
    CSndArray &arr = m_gameFile->m_arrSounds;
    QListWidget *list = ui->listWidget;
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    for (int i=1; i < arr.getSize(); ++i) {
        CSnd snd = arr[i];
        QListWidgetItem *item = new QListWidgetItem(snd.getName());
        list->addItem(item);
        QVariant v;
        v.setValue( i );
        item->setData(Qt::UserRole, v);
    }
    connect(ui->listWidget->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(updateBtnExport()));
}

int CDlgExportSounds::askQuestion(QString filename)
{
    QMessageBox::StandardButton ret = QMessageBox::warning(
        this, "",
        tr("File already exists: %1.\nReplace?").arg(filename),
        QMessageBox::Yes | QMessageBox::YesAll | QMessageBox::No |
            QMessageBox::NoAll | QMessageBox::Cancel);
    return ret;
}

void CDlgExportSounds::on_btnExport_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Select Destination"),
        m_outDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        m_outDir = dir;
        CSndArray &arr = m_gameFile->m_arrSounds;
        QListWidget *list = ui->listWidget;
        QList<QListWidgetItem *> itemList;
        itemList = list->selectedItems();
        bool yesAll = false;
        bool noAll = false;
        foreach(auto *item, itemList) {
            int i = item->data(Qt::UserRole).toInt();
            CSnd snd = arr[i];
            QString fileName = QDir(dir).filePath(snd.getName());

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

            QFileWrap file;
            if (file.open(fileName, "wb")) {
                file.write(snd.getData(), snd.getSize());
                file.close();
            }
        }
    }
}

void CDlgExportSounds::updateBtnExport()
{
    QList<QListWidgetItem *> itemList;
    itemList = ui->listWidget->selectedItems();
    ui->btnExport->setEnabled(itemList.size()!=0);
}

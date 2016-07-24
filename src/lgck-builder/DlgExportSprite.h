#ifndef DLGEXPORTSPRITE_H
#define DLGEXPORTSPRITE_H

#include <QDialog>

class CGameFile;
class QTreeWidgetItem;

namespace Ui {
class CDlgExportSprite;
}

class CDlgExportSprite : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgExportSprite(QWidget *parent, CGameFile *gf);
    ~CDlgExportSprite();

private slots:
    void on_btnExport_clicked();
    void updateBtnExport();

private:
    Ui::CDlgExportSprite *ui;
    CGameFile *m_gameFile;
    void reloadSprites();
    void updateIcon(QTreeWidgetItem * item, int protoId);
};

#endif // DLGEXPORTSPRITE_H

#ifndef DLGEXPORTSOUNDS_H
#define DLGEXPORTSOUNDS_H

#include <QDialog>
class CGameFile;

namespace Ui {
class CDlgExportSounds;
}

class CDlgExportSounds : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgExportSounds(QWidget *parent, CGameFile *gf);
    ~CDlgExportSounds();

private slots:
    void on_btnExport_clicked();
    void updateBtnExport();

private:
    Ui::CDlgExportSounds *ui;
    CGameFile *m_gameFile = nullptr;
    static QString m_outDir;
    void reloadSounds();
    int askQuestion(QString filename);
};

#endif // DLGEXPORTSOUNDS_H

#ifndef DLGDISTRIBUTEGAME_H
#define DLGDISTRIBUTEGAME_H

#include <QDialog>

namespace Ui {
class CDlgDistributeGame;
}

class CDlgDistributeGame : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgDistributeGame(QWidget *parent = 0);
    enum {
        OUT_ZIP,
        OUT_7ZIP,
        OUT_SFX,
        OUT_NONE=-1
    };
    int outType();
    void getRuntime(QString & runtime);
    void getSFX(QString & sfx);
    ~CDlgDistributeGame();

private slots:
    void on_btnRuntime_clicked();
    void on_btnSFX_clicked();
    void on_rZip_clicked();
    void on_r7z_clicked();
    void on_rSFX_clicked();

    void on_eRuntime_textChanged(const QString &arg1);

    void on_eSFX_textChanged(const QString &arg1);

private:
    Ui::CDlgDistributeGame *ui;
    void updateButtons();
};

#endif // DLGDISTRIBUTEGAME_H

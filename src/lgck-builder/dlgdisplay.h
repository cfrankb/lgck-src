#ifndef DLGDISPLAY_H
#define DLGDISPLAY_H

#include <QDialog>
class CDisplay;
class CGameFile;

namespace Ui {
class CDlgDisplay;
}

class CDlgDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgDisplay(QWidget *parent = 0);
    ~CDlgDisplay();

    void load(CDisplay & d);
    void save(CDisplay & d);
    void setGameFile(CGameFile *gf);

private slots:
    void on_cbType_currentIndexChanged(int index);

    void on_cbFrameSet_currentIndexChanged(int index);

    void on_cbBaseFrame_currentIndexChanged(int index);

    void on_eName_textChanged(const QString &arg1);

private:
    void setImage(int frameSet, int frameNo);
    QIcon makeIcon(void *png, int size);
    void fillFrameCombo(int frameSet);
    CGameFile *m_gameFile;
    Ui::CDlgDisplay *ui;
    void enableType(int type);
};

#endif // DLGDISPLAY_H

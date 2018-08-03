#ifndef DLGDISPLAY_H
#define DLGDISPLAY_H

#include <QDialog>
class CDisplay;

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

private slots:
    void on_cbType_currentIndexChanged(int index);

private:
    Ui::CDlgDisplay *ui;
    void enableType(int type);
};

#endif // DLGDISPLAY_H

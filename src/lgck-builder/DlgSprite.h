#ifndef DLGSPRITE_H
#define DLGSPRITE_H

#include <QDialog>

namespace Ui {
    class CDlgSprite;
}

class CDlgSprite : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSprite(QWidget *parent = 0);
    ~CDlgSprite();

private:
    Ui::CDlgSprite *ui;
};

#endif // DLGSPRITE_H

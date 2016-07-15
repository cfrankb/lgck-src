#ifndef DLGEDITFRAMES_H
#define DLGEDITFRAMES_H

#include <QDialog>

namespace Ui {
class CDlgEditFrames;
}

class CDlgEditFrames : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgEditFrames(QWidget *parent = 0);
    ~CDlgEditFrames();

private:
    Ui::CDlgEditFrames *ui;
};

#endif // DLGEDITFRAMES_H

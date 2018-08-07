#ifndef DLGSTEP_H
#define DLGSTEP_H

#include <QDialog>

namespace Ui {
class CDlgStep;
}

class CStep;

class CDlgStep : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgStep(QWidget *parent = 0);
    ~CDlgStep();
    void save(CStep & step);
    void load(CStep & step);
    void setName(const QString &name);

private slots:
    void on_btnAdd_clicked();
    void closeTab(int index);
    void on_btnDelete_clicked();

private:
    Ui::CDlgStep *ui;
};

#endif // DLGSTEP_H

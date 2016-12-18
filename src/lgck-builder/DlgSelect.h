#ifndef DLGSELECT_H
#define DLGSELECT_H

#include <QDialog>

namespace Ui {
class CDlgSelect;
}

class CDlgSelect : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSelect(QWidget *parent = 0);
    ~CDlgSelect();
    int getState() {
        return m_state;
    }
    
    enum {
        OPEN,
        NEW,
        CLOSE,
        SKIP,
        NO_SHOW
    };

private:
    Ui::CDlgSelect *ui;
    int m_state;

private slots:
    void openFile();
    void newFile();
    void skipBox();
    void on_btnNoShow_clicked();
};

#endif // DLGSELECT_H

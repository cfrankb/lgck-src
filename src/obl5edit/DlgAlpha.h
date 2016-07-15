#ifndef DLGALPHA_H
#define DLGALPHA_H

#include <QDialog>

namespace Ui {
class CDlgAlpha;
}

class CDlgAlpha : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgAlpha(QWidget *parent = 0);
    int alpha();
    void setAlpha(int alpha);
    ~CDlgAlpha();
    
private slots:
    void on_sldAlpha_sliderMoved(int position);
    void on_eAlpha_textChanged(const QString &arg1);

    void on_sldAlpha_valueChanged(int value);

private:
    Ui::CDlgAlpha *ui;
    int m_alpha;
    void updateAlpha();
};

#endif // DLGALPHA_H

#ifndef DLGINDICATOR_H
#define DLGINDICATOR_H

#include <QDialog>

namespace Ui {
class CDlgIndicator;
}

class CDlgIndicator : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgIndicator(QWidget *parent = nullptr);
    ~CDlgIndicator();
    void setText(QString text);

private:
    Ui::CDlgIndicator *ui;
    QString m_html;
};

#endif // DLGINDICATOR_H

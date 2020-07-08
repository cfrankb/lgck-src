#ifndef INFODOCK_H
#define INFODOCK_H

#include <QDockWidget>

namespace Ui {
class CInfoDock;
}

class CInfoDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit CInfoDock(QWidget *parent = nullptr);
    ~CInfoDock();

public slots:
    void appendText(const QString & text);

private:
    Ui::CInfoDock *ui;
};

#endif // INFODOCK_H

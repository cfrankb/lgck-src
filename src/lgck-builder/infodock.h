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

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::CInfoDock *ui;
};

#endif // INFODOCK_H

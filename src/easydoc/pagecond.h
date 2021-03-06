#ifndef PAGECOND_H
#define PAGECOND_H

#include <QWidget>
#include <QString>

namespace Ui {
class CPageCond;
}

class CStep;

class CPageCond : public QWidget
{
    Q_OBJECT

public:
    explicit CPageCond(QWidget *parent = 0);
    ~CPageCond();
    void load(const CStep & step, int i);
    void save(CStep & step, int i);

private:
    Ui::CPageCond *ui;
};

#endif // PAGECOND_H

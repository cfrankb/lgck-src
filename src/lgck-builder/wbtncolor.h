#ifndef WBTNCOLOR_H
#define WBTNCOLOR_H

#include <QPushButton>
class QLineEdit;

class CWBtnColor : public QPushButton
{
    Q_OBJECT
public:
    explicit CWBtnColor(QWidget *parent = 0);
    void setBuddy(QLineEdit *q);

signals:
    void colorChanged(int &red, int &green, int &blue);
    void colorChanged(const QString & color);

public slots:
    void setColor(int red, int green, int blue);
    void setColor(const QString & color);
    void click();
    unsigned int color();
    unsigned int colorBGR();

protected:
    void style();
    int m_red;
    int m_green;
    int m_blue;
    QLineEdit *m_buddy;
};

#endif // WBTNCOLOR_H

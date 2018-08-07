#ifndef WSLIDER_H
#define WSLIDER_H

#include <QSlider>
class QLineEdit;

class CWSlider : public QSlider
{
    Q_OBJECT
public:
    CWSlider(QWidget *parent);
    void setBuddy(QLineEdit *q);

signals:
    void valueChanged(const QString & s);

public slots:
    void setValue(int i);
    void setValue(const QString & s);

private:
    QLineEdit * m_buddy;
};

#endif // WSLIDER_H

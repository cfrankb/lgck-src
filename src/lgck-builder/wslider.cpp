#include "wslider.h"
#include <string>
#include <QLineEdit>

CWSlider::CWSlider(QWidget *parent):
    QSlider(parent)
{
    m_buddy = NULL;
}

void CWSlider::setBuddy(QLineEdit *q)
{
    connect(q, SIGNAL(textChanged(QString)), this, SLOT(setValue(QString)));
    connect(this, SIGNAL(valueChanged(QString)), q, SLOT(setText(QString)));
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
}

void CWSlider::setValue(int i)
{
    QString s = QString().sprintf("%d", i);
    emit valueChanged(s);
}

void CWSlider::setValue(const QString & s)
{
    int i = s.toInt();
    if (i < minimum()) {
        i = minimum();
        setValue(i);
    } else if (i > maximum()) {
       i = maximum();
       setValue(i);
    }
    setSliderPosition(i);
}

#include "wbtncolor.h"
#include <QColorDialog>
#include <QLineEdit>

CWBtnColor::CWBtnColor(QWidget *parent) :
    QPushButton(parent)
{
    m_buddy = NULL;
    m_red = 0;
    m_blue = 0;
    m_red = 0;
    connect(this, SIGNAL(clicked()), this, SLOT(click()));
    style();
}

void CWBtnColor::setColor(int red, int green, int blue)
{
    m_red = red;
    m_green = green;
    m_blue = blue;
    style();
}

void CWBtnColor::setColor(const QString & str)
{
    bool ok;
    uint color = str.toUInt(&ok,16);
    int red = color >> 16 & 0xff;
    int green = color >> 8 & 0xff;
    int blue = color & 0xff;
    setColor(red, green, blue);
}

void CWBtnColor::click()
{
    QColor color = QColor(m_red & 0xff,m_green & 0xff ,m_blue & 0xff);
    QColorDialog *d = new QColorDialog(color, static_cast<QWidget*>(parent()));
    d->setOption(QColorDialog::DontUseNativeDialog);
    int result = d->exec();
    if (result == QDialog::Accepted) {
        QColor color = d->selectedColor();//d->currentColor();
        QString newColor = QString::asprintf("%.2x%.2x%.2x", color.red(), color.green(), color.blue());
        m_red = color.red();
        m_green = color.green();
        m_blue = color.blue();
        emit colorChanged(m_red, m_green, m_blue);
        emit colorChanged(newColor);
        if (m_buddy) {
            m_buddy->setText(newColor);
        }
        style();
    }
    delete d;
}

void CWBtnColor::setBuddy(QLineEdit* q)
{
    m_buddy = q;
    connect(q, SIGNAL(textChanged(QString)), this, SLOT(setColor(QString)));
    connect(this, SIGNAL(colorChanged(QString)), q, SLOT(setText(QString)));
}

void CWBtnColor::style()
{
    setStyleSheet( QString("* { background-color: rgb(%1,%2,%3) }")
        .arg(m_red).arg(m_green).arg(m_blue));
}

unsigned int CWBtnColor::color()
{
    return m_red +
        (m_green << 8) +
        (m_blue << 16);
}

unsigned int CWBtnColor::colorBGR()
{
    return (m_red << 16) +
        (m_green << 8) +
        (m_blue);
}

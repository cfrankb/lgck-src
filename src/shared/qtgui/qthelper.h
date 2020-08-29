#ifndef __QT_HELPER_H
#define __QT_HELPER_H
class QIcon;
class CFrame;
class QString;

#include <QIcon>
#include <QPixmap>

QIcon frame2icon(CFrame & frame);
QPixmap frame2pixmap(CFrame & frame);
QString formatVersion(bool singleDigit=false);
#endif

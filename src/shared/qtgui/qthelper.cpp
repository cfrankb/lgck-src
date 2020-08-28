#include "qthelper.h"
#include "../Frame.h"
#include "../ss_version.h"
#include <QtDebug>
#include <QString>

QIcon frame2icon(CFrame & frame)
{
    QIcon icon;
    icon.addPixmap(frame2pixmap(frame), QIcon::Normal, QIcon::On);
    return icon;
}

QPixmap frame2pixmap(CFrame & frame)
{
    uint8_t *png;
    int size;
    frame.toPng(png, size);
    QImage img;
    if (!img.loadFromData(png, size)) {
        qWarning("failed to load png");
    }
    delete [] png;
    return QPixmap::fromImage(img);
}

QString formatVersion()
{
    int version = SS_LGCK_VERSION;
    int vv[4]={0,0,0,0};
    for (int i=3; i >= 0; --i) {
        vv[i] = version & 0xff;
        version /= 256;
    }
    return QString::asprintf("%.2d.%.2d.%.2d.%.2d", vv[0], vv[1], vv[2], vv[3]);
}

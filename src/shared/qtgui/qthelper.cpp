#include "qthelper.h"
#include "../Frame.h"
#include "../GameFile.h"
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

QString formatVersion(bool singleDigit)
{
    int version = CGameFile::getEngineVersion();
    int vv[4]={0,0,0,0};
    for (int i=3; i >= 0; --i) {
        vv[i] = version & 0xff;
        version /= 256;
    }
    const char *fmt = singleDigit ? "%.1d.%.1d.%.1d.%.1d" : "%.2d.%.2d.%.2d.%.2d";
    return QString::asprintf(fmt, vv[0], vv[1], vv[2], vv[3]);
}

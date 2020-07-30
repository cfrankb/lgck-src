#ifndef __LAUNCHER__H
#define __LAUNCHER__H

#include <QString>

constexpr char SPRITE_EDITOR[] = "obl5edit";

typedef struct {
    QString path;
    QString cmd;
} Path;

bool getCmd(const QString & appName, Path &outPath);
bool fileExists(const QString & path);
bool launchProcess(const Path & path);

#endif

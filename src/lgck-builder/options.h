#ifndef COPTIONS_H
#define COPTIONS_H

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <unordered_map>
#include "optiongroup.h"

class QSettings;

class COptions
{
public:
    COptions();
    ~COptions();
    COptionGroup & operator[](const char *name);
    void read(QSettings & settings);
    void write(QSettings & settings);

protected:
    std::unordered_map<std::string, COptionGroup> m_groups;
};

#endif // COPTIONS_H

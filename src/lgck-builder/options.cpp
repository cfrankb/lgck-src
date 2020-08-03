#include "options.h"
#include <QSettings>
#include <QDebug>

COptions::COptions()
{

}

COptions::~COptions()
{

}

COptionGroup & COptions::operator[](const char *name)
{
    COptionGroup & group = m_groups[name];
    group.setName(name);
    return group;
}

void COptions::read(QSettings & settings)
{
    for (auto it = m_groups.begin(); it != m_groups.end(); ++it) {
        it->second.read(settings);
    }
}

void COptions::write(QSettings & settings)
{
    for (auto it = m_groups.begin(); it != m_groups.end(); ++it) {
        it->second.write(settings);
    }
}

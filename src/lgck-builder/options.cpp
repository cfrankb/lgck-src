#include "options.h"
#include <QSettings>
#include <QDebug>

COptions::COptions(const char *group)
{
    Q_ASSERT(group && group[0]);
    m_group = group;
}

COptions::~COptions()
{

}

QVariant & COptions::get(const char *name)
{
    return m_options[name];
}

COptions &COptions::set(const char *name, QVariant value)
{
    m_options[name] = value;
    return *this;
}

void COptions::read(QSettings & settings)
{
    settings.beginGroup(m_group);
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        qDebug() << QString("read option: ") << it->first.c_str() << it->second;
        std::string name = it->first;
        m_options[name] = settings.value(name.c_str(), it->second);
    }
    settings.endGroup();
}

void COptions::write(QSettings &settings)
{
    settings.beginGroup(m_group);
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        qDebug() << QString("write option: ") << it->first.c_str() << it->second;
        std::string name = it->first;
        settings.setValue(name.c_str(), it->second);
    }
    settings.endGroup();
}

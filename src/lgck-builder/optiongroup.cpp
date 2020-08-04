#include <QDebug>
#include "optiongroup.h"

COptionGroup::COptionGroup()
{
}

COptionGroup::~COptionGroup()
{

}

void COptionGroup::setName(const char *name)
{
    Q_ASSERT(name && name[0]);
    m_group = name;
}

QVariant & COptionGroup::get(const char *name)
{
    return m_options[name];
}

QVariant & COptionGroup::operator[](const char *name)
{
    return get(name);
}

COptionGroup & COptionGroup::operator=(COptionGroup &s)
{
    for (auto it = s.m_options.begin(); it != s.m_options.end(); ++it) {
        m_options[it->first] = it->second;
    }
    return *this;
}

COptionGroup &COptionGroup::set(const char *name, QVariant value)
{
    m_options[name] = value;
    return *this;
}

void COptionGroup::read(QSettings & settings)
{
    settings.beginGroup(m_group);
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        std::string name = it->first;
        m_options[name] = settings.value(name.c_str(), it->second);
    }
    settings.endGroup();
}

void COptionGroup::write(QSettings &settings)
{
    settings.beginGroup(m_group);
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        std::string name = it->first;
        settings.setValue(name.c_str(), it->second);
    }
    settings.endGroup();
}

void COptionGroup::debug()
{
    for (auto it = m_options.begin(); it != m_options.end(); ++it) {
        qDebug() << QString("debug option: ") << it->first.c_str() << it->second;
    }
}

void COptionGroup::unset(const char *name)
{
    m_options.erase(name);
}

#ifndef COPTIONGROUP_H
#define COPTIONGROUP_H

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <unordered_map>

class QSettings;

class COptionGroup
{
public:
    COptionGroup();
    ~COptionGroup();
    QVariant & operator[](const char *name);
    COptionGroup & operator=(COptionGroup &s);
    void setName(const char *name);
    COptionGroup & set(const char *name, QVariant value);
    QVariant & get(const char *name);
    void read(QSettings & settings);
    void write(QSettings & settings);
    void debug();

protected:
    QString m_group;
    std::unordered_map<std::string, QVariant> m_options;

friend class COptions;
};


#endif // COPTIONGROUP_H

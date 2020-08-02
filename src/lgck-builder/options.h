#ifndef CFONTOPTIONS_H
#define CFONTOPTIONS_H

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <unordered_map>

class QSettings;

class COptions: public QObject
{
    //Q_OBJECT

public:
    COptions(const char *group);
    ~COptions();
    COptions & set(const char *name, QVariant value);
    QVariant & get(const char *name);
    void read(QSettings & settings);
    void write(QSettings & settings);

protected:
    QString m_group;
    std::unordered_map<std::string, QVariant> m_options;
};

#endif // CFONTOPTIONS_H

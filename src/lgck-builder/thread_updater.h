#ifndef THREAD_UPDATER_H
#define THREAD_UPDATER_H

#include <QThread>
#include <QString>

class CThreadUpdater : public QThread
{
    Q_OBJECT

public:
    void setUrl(const QString & url);

protected:
    void run();
    void sendRequest();

signals:
    void newVersion(const QString & url, const QString & ver);

public slots:

private:
    QString m_url;

};

#endif // THREAD_UPDATER_H

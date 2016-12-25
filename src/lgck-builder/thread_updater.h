#ifndef THREAD_UPDATER_H
#define THREAD_UPDATER_H

#include <QThread>
#include <QString>
#include <QByteArray>

class CThreadUpdater : public QThread
{
    Q_OBJECT

public:
    void setUrl(const QString & url);
    void setData(const QByteArray & data);

protected:
    void run();
    void sendRequest();

signals:
    void newVersion(const QString & url, const QString & ver);

public slots:

private:
    QString m_url;
    QByteArray m_data;

};

#endif // THREAD_UPDATER_H

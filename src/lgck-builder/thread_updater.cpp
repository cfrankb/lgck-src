#include "thread_updater.h"
#include <qdebug.h>
#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include "../shared/qtgui/cheat.h"

// http://karanbalkar.com/2014/02/sending-a-http-request-using-qt-5-framework/
void CThreadUpdater::run()
{
    qDebug("starting...");
    sendRequest();
}

void CThreadUpdater::setUrl(const QString &url)
{
    m_url = url;
}

void CThreadUpdater::setData(const QByteArray & data)
{
    m_data = data;
}

void CThreadUpdater::sendRequest()
{
    //const char *link = "http://cfrankb.fb/lgck/api/chkv.php?ver=00.06.00.06";
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    qDebug() << "URL:" << m_url;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QUrl url = QUrl( m_url );
    QNetworkRequest request( url );
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    //QNetworkReply *reply = manager.get(request);
    QNetworkReply *reply = manager.post(request, m_data);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    QString result;
    if (reply->error() == QNetworkReply::NoError) {
        //success
        result = reply->readAll();
        qDebug() << "Success" <<reply->readAll();
        delete reply;
    } else {
        //failure
        result = reply->readAll();
        qWarning() << "Failure" <<reply->errorString();
        delete reply;
    }

    if (result.indexOf("result: all okay\n") != -1) {
        QRegExp exp = QRegExp("download: [a-zA-z0-9:\\._/]+");
        QString dwUrl = "";
        if (exp.indexIn(result)!=-1) {
            QStringList list = exp.capturedTexts();
            dwUrl = list[0].mid(strlen("download: "));
        }
        exp = QRegExp("version: [0-9a-z\\.]+");
        QString dwVersion = "";
        if (exp.indexIn(result)!=-1) {
            QStringList list = exp.capturedTexts();
            dwVersion = list[0].mid(strlen("version: "));
        }
        if (!dwUrl.isEmpty()) {
            emit newVersion(dwUrl, dwVersion);
        }
    }
}

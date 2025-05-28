/*
    LGCK Builder Runtime
    Copyright (C) 1999, 2020  Francois Blanchette

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "thread_updater.h"
#include <qdebug.h>
#include <QCoreApplication>
#include <QDebug>
#include <QtNetwork>
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
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    sslConfiguration.setProtocol(QSsl::TlsV1_2);
    //const char *link = "http://cfrankb.fb/lgck/api/chkv.php?ver=00.06.00.06";
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    qDebug() << "sslLibraryBuildVersionString: " << QSslSocket::sslLibraryBuildVersionString();
    qDebug("sslLibraryVersionNumber: %ld", QSslSocket::sslLibraryVersionNumber());
    qDebug() << "URL:" << m_url;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QUrl url = QUrl( m_url );
    QNetworkRequest request( url );
    if (m_url.startsWith("https://")) {
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    }
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    //QNetworkReply *reply = manager.get(request);
    QNetworkReply *reply = manager.post(request, m_data);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    QString result = reply->readAll();;
    if (reply->error() == QNetworkReply::NoError) {
        //success
        qDebug() << "Success: " << result;
    } else {
        //failure
        qWarning() << "Failure: " << reply->errorString();
    }
    delete reply;

    QRegularExpressionMatch match;
    if (result.indexOf("result: all okay\n") != -1) {
        QRegularExpression exp = QRegularExpression("download: [a-zA-z0-9:\\._/]+");
        QString dwUrl = "";

        match = exp.match(result);
        if (match.hasMatch()) {
        //if (exp.indexIn(result)!=-1) {
          //  QStringList list = exp.capturedTexts();
            dwUrl = match.captured(0).mid(strlen("download: "));
            //dwUrl = list[0].mid(strlen("download: "));
        }
        exp = QRegularExpression("version: [0-9a-z\\.]+");
        match = exp.match(result);
        QString dwVersion = "";
        if (match.hasMatch()) {
        //if (exp.indexIn(result)!=-1) {
            //QStringList list = exp.capturedTexts();
            dwVersion = match.captured(0).mid(strlen("version: "));
            //dwVersion = list[0].mid(strlen("version: "));
        }
        qDebug() << "dwUrl: " << dwUrl;
        qDebug() << "dwVersion: " << dwVersion;
        if (!dwUrl.isEmpty()) {
            emit newVersion(dwUrl, dwVersion);
        }
    } else {
        qWarning("Incomplete version information returned.");
    }

    eventLoop.quit();
}

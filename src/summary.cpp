/*
    Copyright 2014 Lukáš Tinkl <lukas@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QJsonObject>
#include <QUuid>
#include <QDebug>
#include <QNetworkRequest>

#include <KConfig>
#include <KConfigGroup>

#include "summary.h"
#include "hardware.h"
#include "kde.h"
#include "system.h"

using namespace KAnalytics;

Summary::Summary(QObject * parent):
    QObject(parent),
    m_manager(new QNetworkAccessManager(this))
{
    KConfig cfg("kanalytics");
    KConfigGroup grp(&cfg, "General");
    const bool hasUuid = grp.hasKey("uuid");
    if (hasUuid) {
        //qDebug() << "User has UUID already";
        m_uuid = grp.readEntry("uuid");
    } else {
        //qDebug() << "Creating new UUID";
        m_uuid = QUuid::createUuid().toString().remove('{').remove('}');
        grp.writeEntry("uuid", m_uuid);
    }
}

Summary::~Summary()
{
}

QString Summary::userUuid() const
{
    return m_uuid;
}

QByteArray Summary::toJson() const
{
    QJsonDocument doc;
    QJsonObject tmpObj;
    tmpObj.insert("uuid", m_uuid);
    tmpObj.insert("hardware", Hardware().toJson());
    tmpObj.insert("system", System().toJson());
    tmpObj.insert("KDE", KDE().toJson());
    doc.setObject(tmpObj);
    return doc.toJson();
}

void Summary::exportData()
{
    connect(m_manager, &QNetworkAccessManager::finished, this, &Summary::replyFinished);
    const QByteArray data = toJson();
    QNetworkRequest request(QUrl("http://developer.kde.org/~lukas/kanalytics/kanalytics.php")); // FIXME testing page
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("KAnalytics/%1").arg(KANALYTICS_VERSION));
    m_manager->post(request, data);
}

void Summary::replyFinished(QNetworkReply *reply)
{
    qDebug() << "Sending data finished: " << reply->error();
    Q_EMIT exportFinished(reply->error());
    reply->deleteLater();
}

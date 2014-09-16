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

#ifndef SUMMARY_H
#define SUMMARY_H

#include <QJsonDocument>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace KAnalytics {

/**
 * KAnalytics Summary
 */
class Q_DECL_EXPORT Summary: public QObject
{
    Q_OBJECT
public:
    Summary(QObject *parent = 0);
    virtual ~Summary();

    /**
     * @return the user's UUID used to identify the user
     */
    QString userUuid() const;

    /**
     * Gather basic overall analytics data.
     *
     * First time this method is invoked, a unique random UUID is created so that
     * future data can be related to the same user.
     *
     * @return Analytics data formatted as JSON
     */
    QByteArray toJson() const;

    /**
      * Send the analytics data to a KDE server using the JSON format.
      *
      * Emits the signal exportFinished()
      */
    void exportData();

Q_SIGNALS:
    /**
     * Emitted when the data has (not) been exported
     * @param errorCode @p 0 in case no error ocurred
     */
    void exportFinished(QNetworkReply::NetworkError errorCode);

private slots:
    void replyFinished(QNetworkReply* reply);

private:
    QString m_uuid;
    QNetworkAccessManager *m_manager;
};

}

#endif // SUMMARY_H

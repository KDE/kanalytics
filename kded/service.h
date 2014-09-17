/*
    Copyright 2014 Lukáš Tinkl <lukas@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KANALYTICS_KDED_SERVICE_H
#define KANALYTICS_KDED_SERVICE_H

#include <QNetworkReply>
#include <QTimer>
#include <QNetworkAccessManager>

#include <KDEDModule>
#include <KSharedConfig>

#include "summary.h"

class Q_DECL_EXPORT KAnalyticsService : public KDEDModule
{
    Q_CLASSINFO("D-Bus Interface", "org.kde.analytics")
    Q_PROPERTY(QString version READ version SCRIPTABLE true)
    Q_PROPERTY(QString uuid READ uuid SCRIPTABLE true)
    Q_PROPERTY(uint timestamp READ timestamp SCRIPTABLE true)
    Q_PROPERTY(bool haveUserApproval READ haveUserApproval SCRIPTABLE true)
    Q_OBJECT
public:
    KAnalyticsService(QObject * parent, const QVariantList&);
    virtual ~KAnalyticsService();

    /**
     * @return KAnalytics version
     */
    QString version() const;

    /**
     * @return the user's UUID used to identify the user
     */
    QString uuid() const;

    /**
     * @return the timestamp of the last successful call to exportData()
     *
     * @see QDateTime::toTime_t()
     */
    uint timestamp() const;

    /**
     * @return @p true if the user approved exporting the data
     */
    bool haveUserApproval() const;

public Q_SLOTS:
    /**
      * Send the analytics data unconditionally to a KDE server using the JSON format.
      *
      * Emits the signal exportFinished(), writes the timestamp to the config file upon
      * successful completion
      */
    Q_SCRIPTABLE void exportData();

Q_SIGNALS:
    /**
     * Emitted when the data has (not) been exported
     * @param errorCode @p 0 in case no error ocurred
     *
     * @see QNetworkReply::NetworkError
     */
    Q_SCRIPTABLE void exportFinished(int errorCode);

private Q_SLOTS:
    void init();
    void replyFinished(QNetworkReply* reply);

private:
    QTimer * m_timer;
    QNetworkAccessManager *m_manager;
    KAnalytics::Summary m_summary;
    QDateTime m_timestamp;
    KSharedConfig::Ptr m_cfg;
    bool m_haveUserApproval;
};

#endif // KANALYTICS_KDED_SERVICE_H

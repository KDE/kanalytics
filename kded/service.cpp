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

#include "service.h"
#include "summary.h"
#include "kde.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QDateTime>

#include <KPluginFactory>
#include <KConfigGroup>
#include <KMessageBox>
#include <KLocalizedString>

static const uint ONE_WEEK = 7*24*60*60*1000; // 7 days * 24 hrs * 60 min * 60 sec * 1000 msec

K_PLUGIN_FACTORY(KAnalyticsServiceFactory, registerPlugin<KAnalyticsService>();)

KAnalyticsService::KAnalyticsService(QObject * parent, const QVariantList&)
    : KDEDModule(parent), m_haveUserApproval(false)
{
    connect(this, SIGNAL(moduleRegistered(QDBusObjectPath)), this, SLOT(init()));
}

KAnalyticsService::~KAnalyticsService()
{
}

void KAnalyticsService::init()
{
    // init objects
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished, this, &KAnalyticsService::replyFinished);
    m_timer = new QTimer(this);
    m_timer->setTimerType(Qt::VeryCoarseTimer); // 1sec accuracy, enough for us
    connect(m_timer, &QTimer::timeout, this, &KAnalyticsService::exportData);
    m_cfg = KSharedConfig::openConfig("kanalytics");

    // read the timestamp from config
    KConfigGroup grp(m_cfg, "Export");
    m_timestamp = grp.readEntry<QDateTime>("Timestamp", QDateTime());
    //qDebug() << "Initial timestamp" << m_timestamp;

    // check if the user approved exporting data
    m_haveUserApproval = grp.readEntry<bool>("UserApproval", false);
    if (m_haveUserApproval) {
        //qDebug() << "We have user approval";
        if (!m_timestamp.isValid() || m_timestamp.daysTo(QDateTime::currentDateTime()) > 7) { // no export happened yet or more than one week ago, do it now
            //qDebug() << "EXPORTING NOW ";
            exportData();
        } else { // just schedule the next sync
            const int interval = qMin(ONE_WEEK, ONE_WEEK - QDateTime::currentDateTime().toTime_t()*1000 - m_timestamp.toTime_t()*1000);
            //qDebug() << "Scheduling next sync in: " << interval;
            m_timer->start(interval); // start the timer with ONE_WEEK period since the last sync, ONE_WEEK max
        }
    } else if (!grp.hasKey("UserApproval")) { // new user, ask for approval
        //qDebug() << "new user, asking for approval";
        // FIXME improve this text, link to "real info" page
        const QString text = i18n("<p>Please help us improve KDE software by giving your approval to send some "
                                  "basic analytics data to KDE servers.</p><p>The data is completely <em>anonymous</em> and contains "
                                  "information about your hardware (CPU, GPU, etc.), system (OS name, version, etc.) and KDE software. "
                                  "Thank you, your consent will contribute to better KDE software in the future.</p>"
                                  "<p><a href='http://www.kde.org'>More information</a> about the data being collected</p>");
        if (KMessageBox::questionYesNo(0, text, i18n("KAnalytics Consent"), KGuiItem(i18n("Yes, help improve KDE")),
                                       KGuiItem(i18n("No thanks")), QStringLiteral(),
                                       KMessageBox::Notify | KMessageBox::AllowLink | KMessageBox::PlainCaption) == KMessageBox::Yes) {
            //qDebug() << "user agrees";
            m_haveUserApproval = true;
            grp.writeEntry("UserApproval", true);
            grp.sync();
            exportData(); // export data
        } else {
            //qDebug() << "user disagrees";
            // FIXME perhaps we might as well disable this module completely?
            grp.writeEntry("UserApproval", false);
            grp.sync();
        }
    }
}

QString KAnalyticsService::version() const
{
    return KANALYTICS_VERSION;
}

QString KAnalyticsService::uuid() const
{
    return m_summary.userUuid();
}

uint KAnalyticsService::timestamp() const
{
    return m_timestamp.toTime_t();
}

bool KAnalyticsService::haveUserApproval() const
{
    return m_haveUserApproval;
}

void KAnalyticsService::exportData()
{
    const QByteArray data = m_summary.toJson();
    QNetworkRequest request(QUrl("http://developer.kde.org/~lukas/kanalytics/kanalytics.php")); // FIXME testing page
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("KAnalytics/%1").arg(KANALYTICS_VERSION));
    //qDebug() << "Exporting data: " << data;
    m_manager->post(request, data);
}

void KAnalyticsService::replyFinished(QNetworkReply *reply)
{
    //qDebug() << "Sending data finished: " << reply->error() << " with msg: " << reply->errorString();
    if (reply->error() == QNetworkReply::NoError) { // set and write timestamp and last seen Plasma version
        m_timestamp = QDateTime::currentDateTime();
        KConfigGroup grp(m_cfg, "Export");
        grp.writeEntry("Timestamp", m_timestamp);
        KAnalytics::KDE k;
        grp.writeEntry("LastSeenPlasmaVersion", k.plasmaVersion());
        grp.sync();
    }
    m_timer->start(ONE_WEEK); // restart the timer with one week period
    Q_EMIT exportFinished(reply->error());
    reply->deleteLater();
}

#include "service.moc"

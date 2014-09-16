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

#include <QStringList>
#include <QFile>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QGuiApplication>

#include <KF5/plasma/version.h>
#include <KShell>

#include "system.h"

static void setVar(QString *var, const QString &value)
{
    // Values may contain quotation marks, strip them as we have no use for them.
    KShell::Errors error;
    QStringList args = KShell::splitArgs(value, KShell::NoOptions, &error);
    if (error != KShell::NoError) { // Failed to parse.
        return;
    }
    *var = args.join(QChar(' '));
}

using namespace KAnalytics;

System::System()
{
    // set values from uname
    m_isUtsValid = (uname(&m_utsName) != -1);

    // parse /etc/os-release
    QFile file("/etc/os-release");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString line;
    QStringList comps;
    QMap<QString,QString> map;
    while (!file.atEnd()) {
        line = file.readLine();

        if (line.startsWith(QChar('#'))) {
            // Comment line
            continue;
        }

        comps = line.split(QChar('='));

        if (comps.size() != 2) {
            // Invalid line.
            continue;
        }

        map.insert(comps.at(0), comps.at(1).trimmed());
    }

    if (map.contains(QStringLiteral("NAME")))
        setVar(&m_distroName, map.value(QStringLiteral("NAME")));

    if (map.contains(QStringLiteral("VERSION_ID"))) // prefer the numeric VERSION_ID
        setVar(&m_distroVersion, map.value(QStringLiteral("VERSION_ID")));
    else if (map.contains(QStringLiteral("VERSION")))
        setVar(&m_distroVersion, map.value(QStringLiteral("VERSION")));
}

QString System::osName() const
{
    if (m_isUtsValid)
        return m_utsName.sysname;

    return QString();
}

QString System::osVersion() const
{
    if (m_isUtsValid)
        return m_utsName.release;

    return QString();
}

QString System::distroName() const
{
    if (osName().contains("linux", Qt::CaseInsensitive)) {
        return m_distroName;
    }

    return QString();
}

QString System::distroVersion() const
{
    if (osName().contains("linux", Qt::CaseInsensitive)) {
        return m_distroVersion;
    }

    return QString();
}

QString System::platformName() const
{
    return QGuiApplication::platformName();
}

QJsonObject System::toJson() const
{
    QJsonObject obj;
    obj.insert("osName", osName());
    obj.insert("osVersion", osVersion());
    obj.insert("distroName", distroName());
    obj.insert("distroVersion", distroVersion());
    obj.insert("platformName", platformName());
    return obj;
}

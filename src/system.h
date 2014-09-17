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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <QJsonObject>
#include <QString>

#include <sys/utsname.h>

namespace KAnalytics {

/**
 * System information
 *
 * Gathers basic information about the user's operating system.
 */
class Q_DECL_EXPORT System
{
public:
    System();

    /**
     * @return Name of the user's operation system
     */
    QString osName() const;

    /**
     * @return Version of the user's operation system
     */
    QString osVersion() const;

    /**
     * @return Name of the user's distribution (in case of Linux)
     */
    QString distroName() const;

    /**
     * @return Version of the user's distribution (in case of Linux)
     */
    QString distroVersion() const;

    /**
     * @return Name of the underlying platform plugin (e.g. xcb, windows or ios)
     */
    QString platformName() const;

    /**
     * @return System information analytics data as a QJsonObject
     */
    QJsonObject toJson() const;
private:
    bool m_isUtsValid;
    struct utsname m_utsName;
    QString m_distroName;
    QString m_distroVersion;
};

}

#endif // SYSTEM_H

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

#ifndef KDE_H
#define KDE_H

#include <QLocale>
#include <QJsonObject>

class QString;

namespace KAnalytics {

/**
 * KDE information
 *
 * Gathers basic information about the user's desktop environment.
 */
class Q_DECL_EXPORT KDE
{
public:
    KDE();

    /**
     * @return the current runtime Qt version
     */
    QString qtVersion() const;

    /**
     * @return the current runtime Plasma version
     */
    QString plasmaVersion() const;

    /**
     * @return the current user's locale
     */
    QString userLocale() const;

    /**
     * @return the current user's configured language
     */
    QLocale::Language userLanguage() const;

    /**
     * @return the current user's configured country
     */
    QLocale::Country userCountry() const;

    /**
     * @return @p true if the application's layout direction is right-to-left
     */
    bool isRtl() const;

    /**
     * @return KDE information analytics data as a QJsonObject
     */
    QJsonObject toJson() const;
};

}

#endif // KDE_H

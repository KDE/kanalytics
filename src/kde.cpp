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

#include <QString>
#include <QJsonDocument>
#include <QLocale>
#include <QGuiApplication>

#include <KF5/plasma/version.h>

#include "kde.h"

using namespace KAnalytics;

KDE::KDE()
{
}

QString KDE::qtVersion() const
{
    return qVersion();
}

QString KDE::plasmaVersion() const
{
    return Plasma::versionString();
}

QString KDE::userLocale() const
{
    return QLocale().name();
}

QLocale::Language KDE::userLanguage() const
{
    return QLocale().language();
}

QLocale::Country KDE::userCountry() const
{
    return QLocale().country();
}

bool KDE::isRtl() const
{
    return QGuiApplication::isRightToLeft();
}

QJsonObject KDE::toJson() const
{
    QJsonObject obj;
    obj.insert("qtVersion", qVersion());
    obj.insert("plasmaVersion", plasmaVersion());
    obj.insert("userLocale", userLocale());
    obj.insert("userLanguage", QLocale::languageToString(userLanguage()));
    obj.insert("userCountry", QLocale::countryToString(userCountry()));
    obj.insert("rtl", isRtl());
    return obj;
}

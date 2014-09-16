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

#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

#include <QList>
#include <QtGlobal>
#include <QJsonObject>

#include <Solid/Device>

class QString;

namespace KAnalytics {

/**
 * Hardware analytics
 *
 * Gathers basic information about the system hardware.
 */
class Q_DECL_EXPORT Hardware
{
public:
    Hardware();

    /**
     * @return the chassis or form factor of this computer (e.g. "laptop")
     */
    QString chassis() const;

    /**
     * @return HW platform
     */
    QString machine() const;

    /**
     * @return number of CPUs present in the system
     */
    int numCpus() const;

    /**
     * @return model of the (first) CPU
     */
    QString cpuModel() const;

    /**
     * @return vendor of the (first) CPU
     */
    QString cpuVendor() const;

    /**
     * @return maximum speed of the (first) CPU in MHz
     */
    int cpuSpeed() const;

    /**
     * @return architecture of the system (either 32 or 64)
     */
    int architecture() const;

    /**
     * @return total RAM present in the system, in bytes
     */
    qlonglong totalRam() const;

    /**
     * @return the number of logical dots or pixels per inch.
     */
    qreal screenDpi() const;

    /**
     * @return the pixel resolution of the screen
     */
    QSize screenResolution() const;

    /**
     * @return the primary screen's physical size (in millimeters).
     */
    QSizeF screenSize() const;

    /**
     * @return whether the system has any HDD (rotational storage)
     */
    bool hasHdd() const;

    /**
     * @return whether the system has any SSD (non-rotational storage)
     */
    bool hasSsd() const;

    /**
     * @return hardware information analytics data as a QJsonObject
     */
    QJsonObject toJson() const;

private:
    void analyzeDrives();
    QList<Solid::Device> m_cpuList;
    bool m_hasHdd;
    bool m_hasSsd;
};

}

#endif // HARDWAREINFO_H

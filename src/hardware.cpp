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
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDebug>
#include <QJsonDocument>
#include <QGuiApplication>
#include <QScreen>

#include <Solid/Processor>
#include <Solid/StorageDrive>
#include <Solid/GenericInterface>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#endif
#include <sys/utsname.h>

#include "hardware.h"

const QString hostname1Service = QStringLiteral("org.freedesktop.hostname1");

using namespace KAnalytics;

Hardware::Hardware()
    : m_hasHdd(false), m_hasSsd(false)
{
    m_cpuList = Solid::Device::listFromType(Solid::DeviceInterface::Processor);
    analyzeDrives();
}

QString Hardware::chassis() const
{
    if (!QDBusConnection::systemBus().interface()->isServiceRegistered(hostname1Service)) {
        QDBusConnection::systemBus().interface()->startService(hostname1Service);
    }

    QDBusInterface iface(hostname1Service, "/org/freedesktop/hostname1", hostname1Service, QDBusConnection::systemBus());
    if (iface.isValid()) {
        return iface.property("Chassis").toString();
    }

    return QString();
}

QString Hardware::machine() const
{
    struct utsname utsName;
    if (uname(&utsName) != -1) {
        return utsName.machine;
    }

    return QString();
}

int Hardware::numCpus() const
{
    return m_cpuList.count();
}

QString Hardware::cpuModel() const
{
    // we just take the first one, hopefully nobody has different kinds of them :)
    if (!m_cpuList.isEmpty()) {
        return m_cpuList.first().product();
    }

    return QString();
}

QString Hardware::cpuVendor() const
{
    // we just take the first one, hopefully nobody has different kinds of them :)
    if (!m_cpuList.isEmpty()) {
        return m_cpuList.first().vendor();
    }

    return QString();
}

int Hardware::cpuSpeed() const
{
    // we just take the first one, hopefully nobody has different kinds of them :)
    if (!m_cpuList.isEmpty()) {
        const Solid::Processor * proc = m_cpuList.first().as<Solid::Processor>();
        return proc->maxSpeed();
    }

    return 0;
}

int Hardware::architecture() const
{
    return QT_POINTER_SIZE == 8 ? 64 : 32;
}

qlonglong Hardware::totalRam() const
{
    qlonglong ret = -1;
#ifdef Q_OS_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0)
        // manpage "sizes are given as multiples of mem_unit bytes"
        ret = info.totalram * info.mem_unit;
#endif
    // TODO other OS', see kinfocenter
    return ret;
}

QSize Hardware::screenResolution() const
{
    return QGuiApplication::primaryScreen()->size();
}

QSizeF Hardware::screenSize() const
{
    return QGuiApplication::primaryScreen()->physicalSize();
}

qreal Hardware::screenDpi() const
{
    return QGuiApplication::primaryScreen()->logicalDotsPerInch();
}

bool Hardware::hasHdd() const
{
    return m_hasHdd;
}

bool Hardware::hasSsd() const
{
    return m_hasSsd;
}

QJsonObject Hardware::toJson() const
{
    QJsonObject obj;
    obj.insert("chassis", chassis());
    obj.insert("machine", machine());
    obj.insert("numCpus", numCpus());
    obj.insert("cpuModel", cpuModel());
    obj.insert("cpuVendor", cpuVendor());
    obj.insert("cpuSpeed", cpuSpeed());
    obj.insert("architecture", architecture());
    obj.insert("totalRam", totalRam());
    obj.insert("hdd", hasHdd());
    obj.insert("ssd", hasSsd());
    obj.insert("screenDpi", screenDpi());
    const QSize res = screenResolution();
    obj.insert("screenResolution", QStringLiteral("%1x%2").arg(res.width()).arg(res.height()));
    const QSizeF size = screenSize();
    obj.insert("screenSize", QStringLiteral("%1x%2").arg(size.width()).arg(size.height()));
    return obj;
}

void Hardware::analyzeDrives()
{
    const QList<Solid::Device> driveList = Solid::Device::listFromType(Solid::DeviceInterface::StorageDrive);
    foreach (Solid::Device device, driveList) {
        Solid::StorageDrive * drive = device.as<Solid::StorageDrive>();
        if (drive) {
            if (drive->driveType() != Solid::StorageDrive::HardDisk) { // not interested in optical drives and such
                continue;
            }
            Solid::GenericInterface * genIface = device.as<Solid::GenericInterface>();
            const int rotationRate = genIface->property("RotationRate").toInt();
            //qDebug() << "Drive " << device.udi() << " rate: " << rotationRate;
            if (rotationRate == 0) { // 0 means no rotational media, -1 rotational but unknown, everything else reports the rate
                m_hasSsd = true;
            } else {
                m_hasHdd = true;
            }
        }
    }
}

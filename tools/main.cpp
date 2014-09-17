/*************************************************************************************
 *  Copyright (C) 2014 Lukáš Tinkl <lukas@kde.org>                                   *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include <unistd.h>
#include <stdio.h>

#include <QProcess>
#include <QDebug>
#include <QCommandLineParser>
#include <QTextStream>
#include <QApplication>
#include <QJsonDocument>
#include <QDBusInterface>

#include <KAboutData>
#include <KLocalizedString>
#include <KFormat>

#include "system.h"
#include "hardware.h"
#include "kde.h"
#include "summary.h"

#define TAB "\t"

using namespace std;

static QTextStream out(stdout);

void showCommands()
{
    out << "Commands: " << endl;
    out << TAB << "dump" << TAB << "Dump various information about this system" << endl;
    out << TAB << TAB << "Possible arguments include: all, system, hardware, kde" << endl;
    out << TAB << "export" << TAB << "Export and upload overall analytics data about this system to a KDE server" << endl;
}

void showUuid() {
    KAnalytics::Summary s;
    out << "UUID: " << s.userUuid() << endl;
}

void dumpSystemInfo(bool json) {
    KAnalytics::System sys;
    out << "System info:" << endl;

    if (json) {
        out << QJsonDocument(sys.toJson()).toJson();
    } else {
        out << TAB << "OS name: " << sys.osName() << endl;
        out << TAB << "OS version: " << sys.osVersion() << endl;
        out << TAB << "Distro name: " << sys.distroName() << endl;
        out << TAB << "Distro version: " << sys.distroVersion() << endl;
        out << TAB << "Platform plugin name: " << sys.platformName() << endl;
    }
}

void dumpHwInfo(bool json) {
    KAnalytics::Hardware hw;
    out << "Hardware info:" << endl;

    if (json) {
        out << QJsonDocument(hw.toJson()).toJson();
    } else {
        out << TAB << "Chassis/form factor: " << hw.chassis() << endl;
        out << TAB << "Machine: " << hw.machine() << endl;
        out << TAB << "Architecture: " << hw.architecture() << "bit" << endl;
        out << TAB << "Number of CPUs: " << hw.numCpus() << endl;
        out << TAB << "CPU vendor: " << hw.cpuVendor() << endl;
        out << TAB << "CPU model: " << hw.cpuModel() << endl;
        out << TAB << "CPU speed: " << hw.cpuSpeed() << " MHz" << endl;
        out << TAB << "Total RAM: " << KFormat().formatByteSize(hw.totalRam()) << endl;
        out << TAB << "Has HDD: " << hw.hasHdd() << endl;
        out << TAB << "Has SSD: " << hw.hasSsd() << endl;
        out << TAB << "Logical screen DPI: " << hw.screenDpi() << endl;
        const QSize res = hw.screenResolution();
        out << TAB << "Screen resolution: " << QStringLiteral("%1x%2").arg(res.width()).arg(res.height()) << endl;
        const QSizeF size = hw.screenSize();
        out << TAB << "Physical screen size (mm): " << QStringLiteral("%1x%2").arg(size.width()).arg(size.height()) << endl;
    }
}

void dumpKdeInfo(bool json) {
    KAnalytics::KDE k;
    out << "KDE info:" << endl;

    if (json) {
        out << QJsonDocument(k.toJson()).toJson();
    } else {
        out << TAB << "Qt version: " << k.qtVersion() << endl;
        out << TAB << "KDE Plasma version: " << k.plasmaVersion() << endl;
        out << TAB << "User locale: " << k.userLocale() << endl;
        out << TAB << "User language: " << QLocale::languageToString(k.userLanguage()) << endl;
        out << TAB << "User country: " << QLocale::countryToString(k.userCountry()) << endl;
        out << TAB << "RTL: " << k.isRtl() << endl;
    }
}

void dumpAll(bool json) {
    if (json) {
        KAnalytics::Summary s;
        out << s.toJson();
    } else {
        showUuid();
        dumpHwInfo(false);
        dumpSystemInfo(false);
        dumpKdeInfo(false);
    }
}

void exportData() {
    QDBusInterface iface("org.kde.kded5", "/modules/kanalytics", "org.kde.analytics");
    QDBusConnection::sessionBus().connect(iface.service(), iface.path(), iface.interface(), "exportFinished", qApp, SLOT(quit()));
    iface.call("exportData");
}

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);
    KAboutData aboutData("kanalytics-console", i18n("KAnalytics Console Application"), "1.0",
                         i18n("Console app to inspect KAnalytics data"),
                         KAboutLicense::GPL, i18n("(c) 2014 KAnalytics Team"));
    KAboutData::setApplicationData(aboutData);

    aboutData.addAuthor(QStringLiteral("Lukáš Tinkl"), i18n("Maintainer"), "lukas@kde.org");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(QCommandLineOption("commands", i18n("List the available commands")));
    parser.addOption(QCommandLineOption("json", i18n("Dump data in JSON format")));
    parser.addOption(QCommandLineOption("uuid", i18n("Show the user UUID")));
    parser.addPositionalArgument("command", i18n("Command to execute"));
    parser.addPositionalArgument("[args...]", i18n("Arguments for the specified command"));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    // uuid
    if (parser.isSet("uuid")) {
        showUuid();
        return 1;
    }

    // commands
    QString command;
    if (!parser.positionalArguments().isEmpty()) {
        command = parser.positionalArguments().first();
    }

    //qDebug() << "COMMAND:" << command;

    if (command.isEmpty() || parser.isSet("commands")) {
        showCommands();
        return 1;
    }

    const bool toJson = parser.isSet("json");

    if (command == "dump") {
        const QString subcommand = parser.positionalArguments().value(1);
        //qDebug() << "SUBCOMMAND:" << command;
        if (subcommand == "system") {
            dumpSystemInfo(toJson);
            return 0;
        } else if (subcommand == "hardware") {
            dumpHwInfo(toJson);
            return 0;
        } else if (subcommand == "kde") {
            dumpKdeInfo(toJson);
            return 0;
        } else if (subcommand == "all") {
            dumpAll(toJson);
            return 0;
        } else {
            qWarning() << "Unsupported argument for the <dump> command";
            showCommands();
            return 1;
        }
    } else if (command == "export") {
        exportData();
        return app.exec();
    } else {
        qWarning() << "Unsupported command";
        showCommands();
        return 1;
    }
}

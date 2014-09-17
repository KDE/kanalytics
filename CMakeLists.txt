project(KAnalytics)

cmake_minimum_required(VERSION 2.8.12)

set(KANALYTICS_VERSION 0.0.1)

add_definitions("-DKANALYTICS_VERSION=\"${KANALYTICS_VERSION}\"")

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS Widgets Xml Network DBus)
find_package(KF5 REQUIRED COMPONENTS Solid I18n Plasma CoreAddons Service Config DBusAddons WidgetsAddons)

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(FeatureSummary)

add_definitions(-DTRANSLATION_DOMAIN="kanalytics")

add_subdirectory(src)
add_subdirectory(kded)
add_subdirectory(tools)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

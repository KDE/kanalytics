include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})

set(kded_kanalytics_SRCS
    service.cpp
)

add_library(kded_kanalytics MODULE ${kded_kanalytics_SRCS})

target_link_libraries(kded_kanalytics
    Qt5::Network
    Qt5::DBus
    KF5::CoreAddons
    KF5::DBusAddons
    KF5::ConfigCore
    KF5::WidgetsAddons
    KF5::I18n
    kanalytics
)

install(TARGETS kded_kanalytics DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kanalytics.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)

set(kanalytics_SRCS
    hardware.cpp
    system.cpp
    kde.cpp
    summary.cpp
)

add_library(kanalytics SHARED ${kanalytics_SRCS})

target_link_libraries(kanalytics
    KF5::Solid
    KF5::I18n
    KF5::Plasma
    KF5::ConfigCore
    Qt5::Xml
    Qt5::Network
    Qt5::DBus
)

set_target_properties(kanalytics PROPERTIES KANALYTICS_VERSION ${KANALYTICS_VERSION} SOVERSION 0)

install(TARGETS kanalytics ${INSTALL_TARGETS_DEFAULT_ARGS})

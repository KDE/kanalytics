include_directories(${CMAKE_SOURCE_DIR}/src/)
link_directories(${CMAKE_BINARY_DIR}/src)

add_executable(kanalytics-console main.cpp)
#ecm_mark_nongui_executable(kanalytics-console)
target_link_libraries(kanalytics-console
  Qt5::Core
  Qt5::Widgets # QApplication
  KF5::Solid
  KF5::CoreAddons # KAboutData
  KF5::I18n # KLocalizedString
  kanalytics) # our lib

install(TARGETS kanalytics-console ${INSTALL_TARGETS_DEFAULT_ARGS} RENAME kanalytics)

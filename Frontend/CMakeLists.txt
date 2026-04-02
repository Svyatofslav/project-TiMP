cmake_minimum_required(VERSION 3.16)

project(Frontend VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network)

add_executable(Frontend
    main.cpp
    clientapi.h clientapi.cpp
    functions.h functions.cpp
    windowmanager.h windowmanager.cpp
    authwidget.h authwidget.cpp authwidget.ui
    registerwidget.h registerwidget.cpp registerwidget.ui
    mainmenuwidget.h mainmenuwidget.cpp mainmenuwidget.ui
    taskwidget.h taskwidget.cpp taskwidget.ui
    statswidget.h statswidget.cpp statswidget.ui
)

target_link_libraries(Frontend PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

include(GNUInstallDirs)
install(TARGETS Frontend
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

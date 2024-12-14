cmake_minimum_required(VERSION 3.16)

project(YoutubePlayer VERSION 0.1)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
)

# QTP0002 정책 활성화
qt_policy(SET QTP0002 NEW)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(YoutubePlayer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        extracter.h extracter.cpp
        searchpage.h searchpage.cpp
        resource.qrc
        player.h player.cpp
        streamiodevice.h streamiodevice.cpp
        itag.h itag.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET YoutubePlayer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(YoutubePlayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(YoutubePlayer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

qt_add_resources(YoutubePlayer "resource.qrc")

find_package(Qt6 COMPONENTS Network REQUIRED)
target_link_libraries(YoutubePlayer PRIVATE Qt6::Network)

find_package(Qt6 COMPONENTS Qml REQUIRED)
target_link_libraries(YoutubePlayer PRIVATE Qt6::Qml)

find_package(Qt6 COMPONENTS Multimedia)
target_link_libraries(YoutubePlayer PRIVATE Qt6::Multimedia)

find_package(Qt6 COMPONENTS MultimediaWidgets)
target_link_libraries(YoutubePlayer PRIVATE Qt6::MultimediaWidgets)

find_package(Qt6 REQUIRED COMPONENTS Quick)
target_link_libraries(YoutubePlayer PRIVATE Qt6::Quick)

find_package(Qt6 REQUIRED COMPONENTS QuickWidgets)
target_link_libraries(YoutubePlayer PRIVATE Qt6::QuickWidgets)

target_link_libraries(YoutubePlayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.YoutubePlayer)
endif()
set_target_properties(YoutubePlayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS YoutubePlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(YoutubePlayer)
endif()

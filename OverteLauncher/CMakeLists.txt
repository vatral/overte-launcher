cmake_minimum_required(VERSION 3.19)
project(OverteLauncher LANGUAGES CXX VERSION 0.1)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Network Xml)

qt_standard_project_setup()

qt_add_executable(OverteLauncher
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    s3bucketlister.h s3bucketlister.cpp
    downloadlistdialog.h downloadlistdialog.cpp downloadlistdialog.ui
    downloadprogressdialog.h downloadprogressdialog.cpp downloadprogressdialog.ui
    s3bucketfilter.h s3bucketfilter.cpp
    constants.h
    installedlistmodel.h installedlistmodel.cpp
    installedversion.h installedversion.cpp
    programrunner.h programrunner.cpp
    enums.h
)

qt_add_translations(
    TARGETS OverteLauncher
    TS_FILES OverteLauncher_en_US.ts
)

qt_add_resources(
    OverteLauncher "images"
    PREFIX "/"
    FILES "images/logo_white.svg" "images/logo_black.svg"
)


# Set up strict compiler warnings for all supported compilers
add_library(strict_warnings INTERFACE)
target_compile_options(strict_warnings INTERFACE
  "$<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic>"
  "$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic>"
  "$<$<COMPILE_LANG_AND_ID:C,MSVC>:/W4>"
  "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4>"
)


set_property(TARGET OverteLauncher PROPERTY COMPILE_WARNING_AS_ERROR ON)


target_link_libraries(OverteLauncher
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt::Xml
        strict_warnings
)

install(TARGETS OverteLauncher
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET OverteLauncher
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
add_subdirectory(OverteLauncherTests)


set(CPACK_PACKAGE_NAME "Overte Launcher")
set(CPACK_PACKAGE_VENDOR "Overte e.V.")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "WIX")

# Required fo modern WiX versions, or it'll fail with being unable to
# find the WiX candle executable (executable doesn't exist in later versions)
set(CPACK_WIX_VERSION 4)
set(CPACK_WIX_UPGRADE_GUID "1413e850-edb5-4f98-bfc1-5417f322cc4f")
include(CPack)


cmake_minimum_required(VERSION 3.21)

project(OverteLauncherTests VERSION 0.1 LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Test Network Xml)

qt_standard_project_setup()

include_directories("${CMAKE_SOURCE_DIR}/..")

qt_add_executable(ManifestDownloaderTest
    tst_manifestdownloadertest.cpp ../s3bucketlister.cpp
)


add_test(NAME ManifestDownloaderTest COMMAND ManifestDownloaderTest)


set_target_properties(ManifestDownloaderTest PROPERTIES
    MACOSX_BUNDLE FALSE
    WIN32_EXECUTABLE FALSE
)

target_link_libraries(ManifestDownloaderTest
    PRIVATE Qt6::Test Qt6::Network Qt6::Xml
)

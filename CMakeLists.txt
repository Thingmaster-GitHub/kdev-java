project(java)


cmake_minimum_required(VERSION 3.10)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test Qml Quick)
find_package(KF6I18n REQUIRED)
find_package(KF6KIO REQUIRED)
find_package(KDevPlatform REQUIRED)




find_package (ECM 5.28.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(GenerateExportHeader)

find_package(KF6 REQUIRED COMPONENTS Archive)

find_package(KDevPlatform REQUIRED)
find_package(KDevelop-PG-Qt REQUIRED)

include_directories(${KDEVPGQT_INCLUDE_DIR})

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

add_subdirectory(parser)
add_subdirectory(settings)
add_subdirectory(duchain)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)


########### next target ###############

set(kdevjavalanguagesupport_PART_SRCS
    javalanguagesupport.cpp
    parsejob.cpp

    completion/worker.cpp
    completion/items.cpp
    completion/context.cpp
    completion/model.cpp
    completion/helpers.cpp

    $<TARGET_OBJECTS:kdev_javasettings>
)

kdevplatform_add_plugin(kdevjavalanguagesupport JSON kdevjavasupport.json SOURCES
    ${kdevjavalanguagesupport_PART_SRCS}
)

target_link_libraries(kdevjavalanguagesupport
    KF6::ThreadWeaver
    KF6::Archive
    KF6::KIOCore
    KF6::KIOFileWidgets
    KF6::KIOWidgets
    kdev4javaparser
    kdevjavaduchain
)

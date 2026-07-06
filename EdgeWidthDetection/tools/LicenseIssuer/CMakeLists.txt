cmake_minimum_required(VERSION 3.20)

project(LicenseIssuer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(${PROJECT_NAME}
    main.cpp
    DlgLicenseIssuer.cpp
    DlgLicenseIssuer.h
    DlgLicenseIssuer.ui
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Core
    PRIVATE Qt6::Widgets
    PRIVATE actCry
    PRIVATE hoei
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${QT_PATH}/bin/windeployqt.exe --no-compiler-runtime --no-opengl-sw $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Running windeployqt to package Qt dependencies for LicenseIssuer"
)

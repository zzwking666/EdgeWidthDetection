cmake_minimum_required(VERSION 3.10)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(EdgeWidthDetectionSolution VERSION 0.1 LANGUAGES CXX CUDA)

# 版本信息：版本号 + 构建时间戳 + git短哈希
# 每次 cmake 配置时重新生成 build.version 文件和 BuildVersion.hpp（编译进二进制做兜底）
set(EWD_VERSION_MAJOR 1)
set(EWD_VERSION_MINOR 0)
set(EWD_VERSION_PATCH 0)
set(EWD_VERSION_STRING "${EWD_VERSION_MAJOR}.${EWD_VERSION_MINOR}.${EWD_VERSION_PATCH}")

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
string(TIMESTAMP BUILD_TIMESTAMP "%Y%m%d_%H%M%S")
if(GIT_COMMIT_SHORT)
    set(EWD_BUILD_ID "${BUILD_TIMESTAMP}_${GIT_COMMIT_SHORT}")
else()
    set(EWD_BUILD_ID "${BUILD_TIMESTAMP}")
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/EdgeWidthDetection/include/func/BuildVersion.hpp.in
    ${CMAKE_BINARY_DIR}/EdgeWidthDetection/BuildVersion.hpp
    @ONLY
)

file(WRITE ${CMAKE_BINARY_DIR}/build.version "${EWD_VERSION_STRING}_${EWD_BUILD_ID}\n")

find_package(Python3 REQUIRED COMPONENTS Interpreter)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/iniRW_UL.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeCacheIni.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ComplierIni.cmake)

set(INSTALL_PATH ${CMAKE_SOURCE_DIR}/install/${CMAKE_BUILD_TYPE})

add_subdirectory(ThirdRep)
add_subdirectory(EdgeWidthDetection)

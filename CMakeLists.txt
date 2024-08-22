cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Analyzer VERSION 1.0)   

add_executable(Analyzer src/analyzer.cpp)

include_directories("${PROJECT_SOURCE_DIR}/lib/fftw-3.3.5-dll64")
include_directories("${PROJECT_SOURCE_DIR}/lib/ffmpeg/include")

include_directories("${PROJECT_SOURCE_DIR}/build/bin")
include_directories("${PROJECT_SOURCE_DIR}/src")

link_directories("${PROJECT_SOURCE_DIR}/lib/fftw-3.3.5-dll64")
link_directories("${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib")

link_directories("${PROJECT_SOURCE_DIR}/src")

add_library(fftw SHARED IMPORTED)
set_target_properties(fftw PROPERTIES
    IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/lib/fftw-3.3.5-dll64/libfftw3-3.lib"
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/fftw-3.3.5-dll64/libfftw3-3.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/lib/fftw-3.3.5-dll64")
target_link_libraries(Analyzer PRIVATE fftw)

add_library(avcodec SHARED IMPORTED)
set_target_properties(avcodec PROPERTIES 
    IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/avcodec.lib"
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/ffmpeg/bin/avcodec-61.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/lib/ffmpeg/include")
target_link_libraries(Analyzer PRIVATE avcodec)

add_library(avutil SHARED IMPORTED)
set_target_properties(avutil PROPERTIES 
    IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/avutil.lib"
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/ffmpeg/bin/avutil-59.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/lib/ffmpeg/include")
target_link_libraries(Analyzer PRIVATE avutil)

add_library(avformat SHARED IMPORTED)
set_target_properties(avformat PROPERTIES 
    IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/avformat.lib"
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/ffmpeg/bin/avformat-61.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/lib/ffmpeg/include")
target_link_libraries(Analyzer PRIVATE avformat)

add_library(swresample SHARED IMPORTED)
set_target_properties(swresample PROPERTIES 
    IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/swresample.lib"
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/ffmpeg/bin/swresample-5.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/lib/ffmpeg/include")
target_link_libraries(Analyzer PRIVATE swresample)
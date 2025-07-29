cmake_minimum_required(VERSION 3.16)
project(sonifycpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable Qt
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

# External libraries
find_package(SDL3 REQUIRED)
find_package(SndFile REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua5.3)  # Adjust as needed
find_package(FFTW3 REQUIRED)

# Add Qt resource file
qt_add_resources(APP_RESOURCES application.qrc)

# Source files
set(SOURCES
    src/gv.cpp
    src/main.cpp
    src/sonification.cpp
    src/waveform_savedialog.cpp
    src/sonify.cpp
    src/mapping.cpp
    src/sonifier.cpp
    src/notes.cpp
    src/qcustomplot.cpp
    src/tonegenerator.cpp
    src/waveformwidget.cpp
    src/aboutdialog.cpp
    src/screenRecorder.cpp
    src/SpectrumAnalyzer.cpp
    src/ReverbDialog.cpp
    src/ImageEditorDialog.cpp
)

# Header files
set(HEADERS
    src/gv.hpp
    src/line.hpp
    src/sonification.hpp
    src/waveform_savedialog.hpp
    src/sonify.hpp
    src/traverse.hpp
    src/circle.hpp
    src/path.hpp
    src/mapping.hpp
    src/sonifier.hpp
    src/notes.hpp
    src/qcustomplot.h
    src/tonegenerator.hpp
    src/waveformwidget.hpp
    src/aboutdialog.hpp
    src/screenRecorder.hpp
    src/SpectrumAnalyzer.hpp
    src/pixelColumn.hpp
    src/ReverbDialog.hpp
    src/ImageEditorDialog.hpp
    src/utils.hpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${APP_RESOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    SDL3
    SndFile::sndfile
    ${LUA_LIBRARIES}
    fftw3
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${LUA_INCLUDE_DIRS}
    ${SDL3_INCLUDE_DIRS}
)

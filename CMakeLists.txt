cmake_minimum_required(VERSION 3.16)
project(sonifycpp LANGUAGES CXX VERSION 0.1.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)  # gives CMAKE_INSTALL_INCLUDEDIR, LIBDIR, BINDIR, etc.

message(${CMAKE_INSTALL_LIBDIR})

# Compiler flags

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -DDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Enable Qt
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

# External libraries
find_package(SDL3 REQUIRED)
find_package(SndFile REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(FFTW3 REQUIRED)

# Add Qt resource file
qt_add_resources(APP_RESOURCES application.qrc)

# -----------------------------
# Library target
# -----------------------------
set(LIB_NAME sonifycpp)

set(LIB_SOURCES
  src/utils.cpp
)

add_library(${LIB_NAME} STATIC ${LIB_SOURCES})

target_include_directories(${LIB_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# -----------------------------
# Executable target
# -----------------------------
# Source files
set(SOURCES
  src/gv.cpp
  src/main.cpp
  src/sonification.cpp
  src/waveform_savedialog.cpp
  src/sonify.cpp
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
  src/statusbar.cpp
  src/utils_internal.cpp
  src/utils.cpp
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
  src/sonifier.hpp
  src/notes.hpp
  src/qcustomplot.h
  src/tonegenerator.hpp
  src/waveformwidget.hpp
  src/aboutdialog.hpp
  src/screenRecorder.hpp
  src/SpectrumAnalyzer.hpp
  src/ReverbDialog.hpp
  src/ImageEditorDialog.hpp
  src/defaultMaps/IntensityMap.hpp
  src/defaultMaps/HSVMap.hpp

  include/${LIB_NAME}/pixel.hpp
)

# Create the executable
add_executable(${PROJECT_NAME}_app ${SOURCES} ${HEADERS} ${APP_RESOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}_app
  PRIVATE
  sonifycpp
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::PrintSupport
  SDL3
  SndFile::sndfile
  fftw3
)

# Include directories
target_include_directories(${PROJECT_NAME}_app PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${SDL3_INCLUDE_DIRS}
)

# -----------------------------
# Installation
# -----------------------------
# Install library
install(TARGETS ${LIB_NAME}
  EXPORT ${LIB_NAME}-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install headers
install(DIRECTORY include/${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
)

# Install app binary
install(TARGETS ${PROJECT_NAME}_app
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  RENAME ${PROJECT_NAME}   # final executable name = "sonifycpp"
)

# Export CMake package
install(EXPORT ${PROJECT_NAME}-targets
  FILE ${PROJECT_NAME}-config.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

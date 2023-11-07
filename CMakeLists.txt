cmake_minimum_required(VERSION 3.5)

project(Shelldoku
LANGUAGES CXX

)


# find the systemd files, code from https://fossies.org/linux/apt/CMake/FindSystemd.cmake
find_package(PkgConfig)

pkg_check_modules(SYSTEMD_PKGCONF libsystemd)

find_path(SYSTEMD_INCLUDE_DIRS
  NAMES systemd/sd-bus.h
  PATHS ${SYSTEMD_PKGCONF_INCLUDE_DIRS}
)

find_library(SYSTEMD_LIBRARIES
  NAMES systemd
  PATHS ${SYSTEMD_PKGCONF_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Systemd DEFAULT_MSG SYSTEMD_INCLUDE_DIRS SYSTEMD_LIBRARIES)

mark_as_advanced(SYSTEMD_INCLUDE_DIRS SYSTEMD_LIBRARIES)



set(DCMAKE_EXPORT_COMPILE_COMMANDS=1)

set(SOURCE main.cpp shelldokuPrinter.cpp sudoku.cpp input.cpp sudokuMovement.cpp events/events.cpp events/dispatcher.cpp events/eventQueue.cpp events/listener.cpp) 

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${SYSTEMD_LIBRARIES})

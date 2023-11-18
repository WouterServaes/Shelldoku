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



set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)


add_library(SUDOKU_GENERATOR SHARED lib/sudokuGenerator.cpp)

set(COMMON common/input.cpp common/events/events.cpp common/events/dispatcher.cpp common/events/eventQueue.cpp common/events/listener.cpp)
set(SHELLDOKU shelldoku/shelldokuPrinter.cpp shelldoku/sudoku.cpp shelldoku/sudokuMovement.cpp shelldoku/sudokuSolver.cpp)

set(SOURCE main.cpp ${COMMON} ${SHELLDOKU}) 

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} ${SYSTEMD_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${SUDOKU_GENERATOR})

target_include_directories(${PROJECT_NAME} PUBLIC lib/include/public/)
target_include_directories(${PROJECT_NAME} PUBLIC common/include/public)
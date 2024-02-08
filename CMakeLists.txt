cmake_minimum_required(VERSION 3.5)
project(global)

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


add_library(SUDOKU_SOLVER SHARED lib/sudokuSolver.cpp)
add_library(SUDOKU_GENERATOR SHARED lib/sudokuGenerator.cpp)
# link solver to generator, generator needs to solve
target_link_libraries(SUDOKU_GENERATOR ${SUDOKU_SOLVER})

file(GLOB COMMON common/*.cpp common/events/*.cpp)

# Is this not done by linking the library? 
file(GLOB LIBRARY_CPP lib/*.cpp)

set(PUBLIC_INCLUDE_DIRS lib/include/public/ common/include/public/ common/events/include/public/)
set(PRIVATE_INCLUDE_DIRS lib/include/private)
target_include_directories(SUDOKU_SOLVER PUBLIC ${PUBLIC_INCLUDE_DIRS})
target_include_directories(SUDOKU_SOLVER PRIVATE ${PRIVATE_INCLUDE_DIRS}) 
target_include_directories(SUDOKU_GENERATOR PUBLIC ${PUBLIC_INCLUDE_DIRS}) 
target_include_directories(SUDOKU_GENERATOR PRIVATE ${PRIVATE_INCLUDE_DIRS}) 

add_subdirectory(shelldoku)
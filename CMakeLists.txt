cmake_minimum_required(VERSION 3.5)
project(global)


#========== SYSTEM LIBRARIES 
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

#=========== COMPILE COMMANDS
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)

#=========== FILE GLOBS AND VARIABLES
file(GLOB COMMON common/*.cpp common/events/*.cpp)
set(PUBLIC_INCLUDE_DIRS_APPS ../lib/sudokuTools/include/public/ ../common/include/public/ ../common/events/include/public/)

#=========== LIBRARIES
add_library(SUDOKU_SOLVER SHARED lib/sudokuTools/sudokuSolver.cpp)
add_library(SUDOKU_GENERATOR SHARED lib/sudokuTools/sudokuGenerator.cpp)
add_library(SUDOKU_PARSER SHARED lib/sudokuTools/sudokuParser.cpp)

# link solver to generator, generator needs to solve
target_link_libraries(SUDOKU_GENERATOR SUDOKU_SOLVER)

target_precompile_headers(SUDOKU_SOLVER PRIVATE 
common/include/public/logger.h
)
target_precompile_headers(SUDOKU_GENERATOR PRIVATE
common/include/public/logger.h
common/include/public/shelldokuPrinter.h
)

set(PUBLIC_INCLUDE_DIRS lib/include/public/ common/include/public/ common/events/include/public/)
set(PRIVATE_INCLUDE_DIRS lib/include/private/)
target_include_directories(SUDOKU_SOLVER PUBLIC ${PUBLIC_INCLUDE_DIRS} PRIVATE ${PRIVATE_INCLUDE_DIRS})
target_include_directories(SUDOKU_GENERATOR PUBLIC ${PUBLIC_INCLUDE_DIRS} PRIVATE ${PRIVATE_INCLUDE_DIRS}) 
target_include_directories(SUDOKU_PARSER PUBLIC ${PUBLIC_INCLUDE_DIRS} PRIVATE ${PRIVATE_INCLUDE_DIRS}) 

#============ SUBDIRECTORIES
add_subdirectory(lib/sudokuTools)
add_subdirectory(lib/ansiUIFramework)
add_subdirectory(shelldoku)
add_subdirectory(shelldoku_generator)

target_precompile_headers(Shelldoku PRIVATE 
    common/include/public/shelldokuPrinter.h 
    common/include/public/ansi.h 
    common/include/public/input.h 
    common/include/public/logger.h)
  
target_precompile_headers(Shelldoku_generator REUSE_FROM Shelldoku)

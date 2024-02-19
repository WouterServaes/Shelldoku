add_library(ANSI_UI_FRAMEWORK SHARED ansiUiFramework.cpp)

target_include_directories(ANSI_UI_FRAMEWORK 
  PUBLIC include/public/ ${CMAKE_SOURCE_DIR}/common/include/public/ 
  PRIVATE include/private/)

target_precompile_headers(ANSI_UI_FRAMEWORK PRIVATE
${CMAKE_SOURCE_DIR}/common/include/public/ansi.h
)



project(AnsiUiFramework_test
LANGUAGES CXX)

add_executable(AnsiUiFramework_test main.cpp)
target_link_libraries(AnsiUiFramework_test ANSI_UI_FRAMEWORK ${SYSTEMD_LIBRARIES})
target_include_directories(AnsiUiFramework_test PUBLIC include/public/)


# ctest for memory leak testing 
include(CTest)
add_test(testAnsiUiFramework AnsiUiFramework_test)
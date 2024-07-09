project(Shelldoku
LANGUAGES CXX)

set(SHELLDOKU sudoku.cpp sudokuMovement.cpp sudokuMessager.cpp)
set(SOURCE main.cpp ${COMMON} ${SHELLDOKU} ) 

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} ${SYSTEMD_LIBRARIES} SUDOKU_SOLVER SUDOKU_GENERATOR SUDOKU_PARSER ANSI_UI_FRAMEWORK)

target_include_directories(${PROJECT_NAME} 
PUBLIC ${PUBLIC_INCLUDE_DIRS_APPS} ${CMAKE_SOURCE_DIR}/lib/ansiUIFramework/include/public/
PRIVATE include/private/ include/public/)

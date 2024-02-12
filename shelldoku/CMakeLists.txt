project(Shelldoku
LANGUAGES CXX)

set(SHELLDOKU sudoku.cpp sudokuMovement.cpp)
set(SOURCE main.cpp ${COMMON} ${SHELLDOKU} ) 

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} ${SYSTEMD_LIBRARIES} SUDOKU_SOLVER SUDOKU_GENERATOR)

target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS_APPS} PRIVATE include/private/ include/public/)

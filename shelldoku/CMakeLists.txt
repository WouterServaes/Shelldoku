project(Shelldoku
LANGUAGES CXX)

set(SHELLDOKU sudoku.cpp shelldokuPrinter.cpp sudokuMovement.cpp)
set(SOURCE main.cpp ${COMMON} ${SHELLDOKU} ) 

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} ${SYSTEMD_LIBRARIES})
target_link_libraries(${PROJECT_NAME} SUDOKU_GENERATOR)
target_link_libraries(${PROJECT_NAME} SUDOKU_SOLVER)

target_include_directories(${PROJECT_NAME} PRIVATE include/private/)
target_include_directories(${PROJECT_NAME} PRIVATE include/public/)
target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS_APPS})



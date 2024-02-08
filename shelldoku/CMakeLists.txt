project(Shelldoku
LANGUAGES CXX)

set(SHELLDOKU sudoku.cpp shelldokuPrinter.cpp sudokuMovement.cpp)
set(SOURCE main.cpp ${COMMON} ${SHELLDOKU} ${LIBRARY_CPP}) 

add_executable(${PROJECT_NAME} ${SOURCE})
message(${PUBLIC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SYSTEMD_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${SUDOKU_GENERATOR})
target_link_libraries(${PROJECT_NAME} ${SUDOKU_SOLVER})

target_include_directories(${PROJECT_NAME} PRIVATE include/private/)
target_include_directories(${PROJECT_NAME} PRIVATE include/public/)
target_include_directories(${PROJECT_NAME} PUBLIC ../lib/include/public/ ../common/include/public/ ../common/events/include/public/)



add_library(SUDOKU_SOLVER SHARED sudokuSolver.cpp)
add_library(SUDOKU_GENERATOR SHARED sudokuGenerator.cpp sudokuDifficulty.cpp)
add_library(SUDOKU_PARSER SHARED sudokuParser.cpp)
# link solver to generator, generator needs to solve
target_link_libraries(SUDOKU_GENERATOR SUDOKU_SOLVER)

set(PUBLIC_INCLUDE_DIRS include/public/ ${CMAKE_SOURCE_DIR}/common/include/public/ ${CMAKE_SOURCE_DIR}/common/events/include/public/) 
set(PRIVATE_INCLUDE_DIRS include/private/)
target_include_directories(SUDOKU_SOLVER PUBLIC ${PUBLIC_INCLUDE_DIRS} PRIVATE ${PRIVATE_INCLUDE_DIRS})
target_include_directories(SUDOKU_GENERATOR PUBLIC ${PUBLIC_INCLUDE_DIRS} PRIVATE ${PRIVATE_INCLUDE_DIRS}) 
target_include_directories(SUDOKU_PARSER PUBLIC ${PUBLIC_INCLUDE_DIRS} PRIVATE ${PRIVATE_INCLUDE_DIRS}) 

target_precompile_headers(SUDOKU_SOLVER PRIVATE 
${CMAKE_SOURCE_DIR}/common/include/public/logger.h
)
target_precompile_headers(SUDOKU_GENERATOR PRIVATE
${CMAKE_SOURCE_DIR}/common/include/public/logger.h
${CMAKE_SOURCE_DIR}/common/include/public/shelldokuPrinter.h
)

install(
  TARGETS SUDOKU_SOLVER SUDOKU_GENERATOR SUDOKU_PARSER 
  LIBRARY DESTINATION ${SHELLDOKU_LIB_DEST}
)


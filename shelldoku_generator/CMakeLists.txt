project(Shelldoku_generator
  LANGUAGES CXX)

set(SOURCE main.cpp ${COMMON})

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} ${SYSTEMD_LIBRARIES} SUDOKU_GENERATOR SUDOKU_PARSER)

target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS_APPS} PRIVATE include/public/ include/private/)
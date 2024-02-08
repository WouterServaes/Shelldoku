project(Shelldoku_generator
  LANGUAGES CXX)

set(SOURCE main.cpp ${COMMON})

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} ${SYSTEMD_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PUBLIC_INCLUDE_DIRS_APPS})
target_include_directories(${PROJECT_NAME} PRIVATE include/public/)
target_include_directories(${PROJECT_NAME} PRIVATE include/private/)
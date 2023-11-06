cmake_minimum_required(VERSION 3.5)

project(Shelldoku
LANGUAGES CXX

)

set(DCMAKE_EXPORT_COMPILE_COMMANDS=1)

set(SOURCE main.cpp shelldokuPrinter.cpp sudoku.cpp input.cpp events/events.cpp events/dispatcher.cpp events/eventQueue.cpp events/listener.cpp) 

add_executable(${PROJECT_NAME} ${SOURCE})

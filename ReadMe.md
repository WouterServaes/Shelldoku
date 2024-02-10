cmake -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX . -B build/release

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX . -B build/debug


w/o C

cmake -DCMAKE_CXX_COMPILER=$CXX . -B build/release

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$CXX . -B build/debug

For working intellisense/clangd in VScode the compile_arguments are generated using the cmake flag DCMAKE_EXPORT_COMPILE_COMMANDS: set(DCMAKE_EXPORT_COMPILE_COMMANDS ON),
the generated compile_commands.json should then be moved to the root workspace folder, and VS code should then be restarted to fix all visual include-related errors.


https://www.sudokuwiki.org/sudoku.htm      
https://zhangroup.aporc.org/images/files/Paper_3485.pdf

# Shelldoku
sudoku in terminal

# Shelldoku generator
sudoku generator
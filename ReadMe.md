cmake -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX . -B build/release

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX . -B build/debug


w/o C

cmake -DCMAKE_CXX_COMPILER=$CXX . -B build/release

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$CXX . -B build/debug
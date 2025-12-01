OUTPUT=./build/main

mkdir -p build
if [ -f $OUTPUT ] ; then
    rm $OUTPUT 
fi

clang++ src/main.cpp src/utils.cpp -g -O0 -std=c++20 -o $OUTPUT && $OUTPUT $1 $2 $3

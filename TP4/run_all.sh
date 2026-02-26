#!/bin/bash

echo "Compiling..."
clang -Xpreprocessor -fopenmp exo1.c \
  -I$(brew --prefix libomp)/include \
  -L$(brew --prefix libomp)/lib -lomp \
  -o exo1

clang -Xpreprocessor -fopenmp exo2.c \
  -I$(brew --prefix libomp)/include \
  -L$(brew --prefix libomp)/lib -lomp \
  -o exo2

clang -Xpreprocessor -fopenmp exo3.c \
  -I$(brew --prefix libomp)/include \
  -L$(brew --prefix libomp)/lib -lomp \
  -o exo3

clang -Xpreprocessor -fopenmp exo4.c \
  -I$(brew --prefix libomp)/include \
  -L$(brew --prefix libomp)/lib -lomp \
  -o exo4


echo "Running Exercise 1"
export OMP_NUM_THREADS=3
./exo1

echo "Running Exercise 2 (1 thread)"
export OMP_NUM_THREADS=1
./exo2

echo "Running Exercise 2 (4 threads)"
export OMP_NUM_THREADS=4
./exo2

echo "Running Exercise 3"
export OMP_NUM_THREADS=3
./exo3

echo "Running Exercise 4"
for v in 1 3
do
    echo "Version $v"
    for t in 1 2 4 8 16
    do
        export OMP_NUM_THREADS=$t
        echo "Threads = $t"
        ./exo4 $v
    done
done

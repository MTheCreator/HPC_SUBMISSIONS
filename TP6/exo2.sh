#!/bin/bash

echo "Matrix-Vector Multiplication Performance Analysis"
echo "=================================================="
echo ""

# Compile the code
mpicc -o ex2 exo2.c -lm

# Test different matrix sizes
SIZES=(500 1000 2000 4000)
PROCS=(1 2 4 8)

echo "| Processes | Matrix Size | Serial Time | Parallel Time | Speedup | Efficiency | Max Error |"
echo "|-----------|-------------|-------------|---------------|---------|------------|-----------|"

for N in "${SIZES[@]}"; do
    for P in "${PROCS[@]}"; do
        mpirun -np $P ./ex2 $N
    done
    echo ""
done
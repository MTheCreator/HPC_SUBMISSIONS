#!/bin/bash
# filepath: run_code.sh

mkdir -p results

RESULTS_FILE="results/code_results.txt"
echo "Parallel Performance Analysis" > $RESULTS_FILE
echo "===========================================" >> $RESULTS_FILE

# Test different system sizes
SIZES=(120 240 480 960)

# Test different thread counts
THREADS=(1 2 4 8 16)

# Compilation function
compile_code() {
    local n=$1
    clang -Xpreprocessor -fopenmp code.c \
      -I$(brew --prefix libomp)/include \
      -L$(brew --prefix libomp)/lib -lomp \
      -DVAL_N=$n -DVAL_D=80 -lm \
      -o code
}

echo "Testing different thread counts with N=120..."
compile_code 120

for t in "${THREADS[@]}"; do
    echo "" >> $RESULTS_FILE
    echo "Threads: $t" >> $RESULTS_FILE
    export OMP_NUM_THREADS=$t
    ./code >> $RESULTS_FILE
done

# Test different system sizes with 4 threads
echo "" >> $RESULTS_FILE
echo "Testing different system sizes (4 threads)..." >> $RESULTS_FILE
export OMP_NUM_THREADS=4

for size in "${SIZES[@]}"; do
    echo "" >> $RESULTS_FILE
    echo "Compiling with system size: $size"
    compile_code $size
    ./code >> $RESULTS_FILE
done

echo ""
echo "Results saved to $RESULTS_FILE"
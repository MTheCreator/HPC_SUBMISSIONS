#!/bin/bash
# filepath: run_matrix_tests.sh

# Compile the program
echo "Compiling matrix multiplication program..."
gcc-17 -fopenmp code.c -o code

# Create output directory for results
mkdir -p results

# Output file for results
RESULTS_FILE="results/performance_results.txt"
echo "Matrix Multiplication Performance Analysis" > $RESULTS_FILE
echo "==========================================" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Test with different numbers of threads
echo "Testing with different thread counts..."
echo "Thread Count Tests (default scheduling)" >> $RESULTS_FILE
echo "----------------------------------------" >> $RESULTS_FILE

for threads in 1 2 4 8 16; do
    echo "Running with $threads threads..."
    export OMP_NUM_THREADS=$threads
    echo -n "Threads: $threads - " >> $RESULTS_FILE
    ./code >> $RESULTS_FILE
done

echo "" >> $RESULTS_FILE

# Test different scheduling policies with 4 threads
echo "Testing different scheduling policies (4 threads)..."
export OMP_NUM_THREADS=4

echo "Scheduling Policy Tests (4 threads)" >> $RESULTS_FILE
echo "------------------------------------" >> $RESULTS_FILE

# STATIC scheduling with different chunk sizes
echo "Testing STATIC scheduling..." >> $RESULTS_FILE
for chunk in 1 10 50 100; do
    echo "  Chunk size: $chunk"
    export OMP_SCHEDULE="static,$chunk"
    echo -n "STATIC (chunk=$chunk): " >> $RESULTS_FILE
    ./code >> $RESULTS_FILE
done

echo "" >> $RESULTS_FILE

# DYNAMIC scheduling with different chunk sizes
echo "Testing DYNAMIC scheduling..." >> $RESULTS_FILE
for chunk in 1 10 50 100; do
    echo "  Chunk size: $chunk"
    export OMP_SCHEDULE="dynamic,$chunk"
    echo -n "DYNAMIC (chunk=$chunk): " >> $RESULTS_FILE
    ./code >> $RESULTS_FILE
done

echo "" >> $RESULTS_FILE

# GUIDED scheduling with different chunk sizes
echo "Testing GUIDED scheduling..." >> $RESULTS_FILE
for chunk in 1 10 50 100; do
    echo "  Chunk size: $chunk"
    export OMP_SCHEDULE="guided,$chunk"
    echo -n "GUIDED (chunk=$chunk): " >> $RESULTS_FILE
    ./code >> $RESULTS_FILE
done

echo "" >> $RESULTS_FILE
echo "All tests completed! Results saved in $RESULTS_FILE"

# Create a Python script to generate plots
cat > results/plot_results.py << 'EOF'
import matplotlib.pyplot as plt
import numpy as np

# You'll need to manually extract timing data from the results file
# Example data structure:
threads = [1, 2, 4, 8, 16]
times = []  # Fill with actual times from results file

# Calculate speedup and efficiency
if len(times) > 0:
    sequential_time = times[0]
    speedup = [sequential_time / t for t in times]
    efficiency = [s / threads[i] * 100 for i, s in enumerate(speedup)]
    
    # Plot speedup
    plt.figure(figsize=(12, 5))
    
    plt.subplot(1, 2, 1)
    plt.plot(threads, speedup, 'bo-', label='Actual Speedup')
    plt.plot(threads, threads, 'r--', label='Ideal Speedup')
    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup')
    plt.title('Speedup vs Number of Threads')
    plt.legend()
    plt.grid(True)
    
    # Plot efficiency
    plt.subplot(1, 2, 2)
    plt.plot(threads, efficiency, 'go-')
    plt.xlabel('Number of Threads')
    plt.ylabel('Efficiency (%)')
    plt.title('Efficiency vs Number of Threads')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('results/performance_plots.png')
    print("Plots saved to results/performance_plots.png")
else:
    print("Please fill in the 'times' list with actual data from results file")
EOF

echo ""
echo "To generate plots:"
echo "1. Extract timing data from $RESULTS_FILE"
echo "2. Edit results/plot_results.py and fill in the 'times' array"
echo "3. Run: python3 results/plot_results.py"
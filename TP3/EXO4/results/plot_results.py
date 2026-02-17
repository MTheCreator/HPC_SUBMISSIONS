import matplotlib.pyplot as plt
import numpy as np

# You'll need to manually extract timing data from the results file
# Example data structure:
threads = [1, 2, 4, 8, 16]
times = [0.269674, 0.121995, 0.067952, 0.052612, 0.049701]  # Fill with actual times from results file

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

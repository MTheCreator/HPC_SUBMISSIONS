#!/bin/bash
#SBATCH --job-name=TP2_All
#SBATCH --output=tp2_all_output_%j.txt
#SBATCH --error=tp2_all_error_%j.txt
#SBATCH --time=01:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=8G
#SBATCH --partition=batch

# Load modules (adjust based on your HPC system)
# module load gcc/11.2.0
# module load valgrind

echo "========================================"
echo "TP2: Foundations of Parallel Computing"
echo "Running Exercises 3 and 4"
echo "Job ID: $SLURM_JOB_ID"
echo "Running on node: $(hostname)"
echo "Start time: $(date)"
echo "========================================"
echo ""

# Create results directory
mkdir -p results
cd results

#===========================================
# EXERCISE 3
#===========================================
echo ""
echo "########################################"
echo "# EXERCISE 3: Array Operations"
echo "########################################"
echo ""

# Compile Exercise 3
echo "Compiling exercise3_complete.c..."
gcc -O2 -g -Wall ../exo3.c -o exercise3_complete

if [ $? -ne 0 ]; then
    echo "Exercise 3 compilation failed!"
else
    echo "Compilation successful!"
    echo ""
    
    # Run Exercise 3
    echo "Running Exercise 3..."
    ./exercise3_complete | tee exercise3_results.txt
    
    echo ""
    # Profile with Valgrind if available
    if command -v valgrind &> /dev/null; then
        echo "Profiling with Valgrind Callgrind..."
        valgrind --tool=callgrind --callgrind-out-file=callgrind_ex3.out ./exercise3_complete > /dev/null 2>&1

        if [ -f callgrind_ex3.out ]; then
            callgrind_annotate callgrind_ex3.out > callgrind_ex3_annotated.txt
            echo "Callgrind profiling complete!"

            # Display key results
            echo ""
            echo "Top functions by instruction count:"
            grep -E "file:function" callgrind_ex3_annotated.txt -A 10 | head -15
        fi
    else
        echo "Valgrind not available - using timing-based analysis"
    fi
fi

#===========================================
# EXERCISE 4
#===========================================
echo ""
echo "########################################"
echo "# EXERCISE 4: Matrix Multiplication"
echo "########################################"
echo ""

# Compile Exercise 4
echo "Compiling exercise4_complete.c..."
gcc -O2 -g -Wall ../exo4.c -o exercise4_complete

if [ $? -ne 0 ]; then
    echo "Exercise 4 compilation failed!"
else
    echo "Compilation successful!"
    echo ""
    
    # Run Exercise 4
    echo "Running Exercise 4..."
    ./exercise4_complete | tee exercise4_results.txt
    
    echo ""
    # Profile with Valgrind if available
    if command -v valgrind &> /dev/null; then
        echo "Profiling with Valgrind Callgrind..."
        valgrind --tool=callgrind --callgrind-out-file=callgrind_ex4.out ./exercise4_complete > /dev/null 2>&1

        if [ -f callgrind_ex4.out ]; then
            callgrind_annotate callgrind_ex4.out > callgrind_ex4_annotated.txt
            echo "Callgrind profiling complete!"

            # Display key results
            echo ""
            echo "Top functions by instruction count:"
            grep -E "file:function" callgrind_ex4_annotated.txt -A 10 | head -15
        fi
    else
        echo "Valgrind not available - using timing-based analysis"
    fi
fi


#===========================================
# ANALYSIS AND PLOTTING
#===========================================
echo ""
echo "########################################"
echo "# ANALYSIS AND VISUALIZATION"
echo "########################################"
echo ""

# Extract fs values from results
if [ -f exercise3_results.txt ] && [ -f exercise4_results.txt ]; then
    FS_EX3=$(grep "fs (sequential fraction)" exercise3_results.txt | awk -F'= ' '{print $2}')
    FS_EX4=$(grep "fs (sequential fraction)" exercise4_results.txt | awk -F'= ' '{print $2}')
    
    echo "Extracted sequential fractions:"
    echo "  Exercise 3: fs = $FS_EX3"
    echo "  Exercise 4: fs = $FS_EX4"
    echo ""
    
    # Generate Python analysis script
    cat > analyze_results.py << 'PYTHON_SCRIPT_END'
#!/usr/bin/env python3
import sys
import re

def amdahl_speedup(fs, p):
    return 1.0 / (fs + (1 - fs) / p)


def gustafson_speedup(fs, p):
    return p - fs * (p - 1)

# Get fs values from command line
fs_ex3 = float(sys.argv[1])
fs_ex4 = float(sys.argv[2])

processors = [1, 2, 4, 8, 16, 32, 64]

print("\n" + "="*80)
print("AMDAHL'S AND GUSTAFSON'S LAW PREDICTIONS")
print("="*80)

# Exercise 3
print("\nEXERCISE 3 (fs = {:.6f}):".format(fs_ex3))
print("-"*80)
print("{:<12} {:>15} {:>15} {:>15} {:>15}".format(
    "Processors", "Amdahl S(p)", "Amdahl Eff%", "Gustafson S(p)", "Gustafson Eff%"))
print("-"*80)
for p in processors:
    amd_s = amdahl_speedup(fs_ex3, p)
    gus_s = gustafson_speedup(fs_ex3, p)
    amd_eff = (amd_s / p) * 100
    gus_eff = (gus_s / p) * 100
    print("{:<12} {:>15.4f} {:>15.2f} {:>15.4f} {:>15.2f}".format(
        p, amd_s, amd_eff, gus_s, gus_eff))
print("-"*80)
print("Max Amdahl Speedup: {:.2f}".format(1/fs_ex3))
# Exercise 4
print("\n\nEXERCISE 4 (fs = {:.8f}):".format(fs_ex4))
print("-"*80)
print("{:<12} {:>15} {:>15} {:>15} {:>15}".format(
    "Processors", "Amdahl S(p)", "Amdahl Eff%", "Gustafson S(p)", "Gustafson Eff%"))
print("-"*80)
for p in processors:
    amd_s = amdahl_speedup(fs_ex4, p)
    gus_s = gustafson_speedup(fs_ex4, p)
    amd_eff = (amd_s / p) * 100
    gus_eff = (gus_s / p) * 100
    print("{:<12} {:>15.4f} {:>15.2f} {:>15.4f} {:>15.2f}".format(
        p, amd_s, amd_eff, gus_s, gus_eff))
print("-"*80)
print("Max Amdahl Speedup: {:.0f}".format(1/fs_ex4))

# Comparison
print("\n\nCOMPARISON:")
print("="*80)
ratio = fs_ex3 / fs_ex4
print("Exercise 4 is {:.0f}x better at parallelization than Exercise 3".format(ratio))
print("="*80)
PYTHON_SCRIPT_END
    
    chmod +x analyze_results.py
    
    # Run analysis
    echo "Running analysis..."
    python3 analyze_results.py "$FS_EX3" "$FS_EX4" | tee analysis_tables.txt
    
    echo ""
    echo "Generating plots..."
    
    # Generate plotting script
    cat > generate_plots.py << 'PLOT_SCRIPT_END'
#!/usr/bin/env python3
import sys
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend for HPC
import matplotlib.pyplot as plt
import numpy as np

def amdahl_speedup(fs, p):
    return 1.0 / (fs + (1 - fs) / p)

def gustafson_speedup(fs, p):
    return p - fs * (p - 1)

# Get fs values
fs_ex3 = float(sys.argv[1])
fs_ex4 = float(sys.argv[2])

processors = np.array([1, 2, 4, 8, 16, 32, 64])

# Calculate speedups
amdahl_ex3 = np.array([amdahl_speedup(fs_ex3, p) for p in processors])
amdahl_ex4 = np.array([amdahl_speedup(fs_ex4, p) for p in processors])
gustafson_ex3 = np.array([gustafson_speedup(fs_ex3, p) for p in processors])
gustafson_ex4 = np.array([gustafson_speedup(fs_ex4, p) for p in processors])

# Create figure
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))

# Plot 1: Amdahl's Law
ax1.plot(processors, amdahl_ex3, 'o-', linewidth=2, markersize=8, 
         label=f'Exercise 3 (fs={fs_ex3:.4f})', color='blue')
ax1.plot(processors, amdahl_ex4, 's-', linewidth=2, markersize=8, 
         label=f'Exercise 4 (fs={fs_ex4:.6f})', color='red')
ax1.plot(processors, processors, 'k:', linewidth=2, alpha=0.3, label='Ideal')
ax1.axhline(y=1/fs_ex3, color='blue', linestyle='--', alpha=0.3, 
            label=f'Max Ex3 = {1/fs_ex3:.2f}')
ax1.set_xlabel('Number of Processors (p)', fontsize=12)
ax1.set_ylabel('Speedup S(p)', fontsize=12)
ax1.set_title("Amdahl's Law - Strong Scaling", fontsize=14, fontweight='bold')
ax1.legend(fontsize=9)
ax1.grid(True, alpha=0.3)
ax1.set_xscale('log', base=2)
ax1.set_yscale('log', base=2)

# Plot 2: Gustafson's Law
ax2.plot(processors, gustafson_ex3, 'o-', linewidth=2, markersize=8, 
         label='Exercise 3', color='blue')
ax2.plot(processors, gustafson_ex4, 's-', linewidth=2, markersize=8, 
         label='Exercise 4', color='red')
ax2.plot(processors, processors, 'k:', linewidth=2, alpha=0.3, label='Ideal')
ax2.set_xlabel('Number of Processors (p)', fontsize=12)
ax2.set_ylabel('Speedup S(p)', fontsize=12)
ax2.set_title("Gustafson's Law - Weak Scaling", fontsize=14, fontweight='bold')
ax2.legend(fontsize=9)
ax2.grid(True, alpha=0.3)
ax2.set_xscale('log', base=2)
ax2.set_yscale('log', base=2)

# Plot 3: Amdahl Efficiency
eff_amd_ex3 = (amdahl_ex3 / processors) * 100
eff_amd_ex4 = (amdahl_ex4 / processors) * 100
ax3.plot(processors, eff_amd_ex3, 'o-', linewidth=2, markersize=8, 
         label='Exercise 3', color='blue')
ax3.plot(processors, eff_amd_ex4, 's-', linewidth=2, markersize=8, 
         label='Exercise 4', color='red')
ax3.axhline(y=100, color='k', linestyle=':', alpha=0.3, label='100%')
ax3.set_xlabel('Number of Processors (p)', fontsize=12)
ax3.set_ylabel('Efficiency (%)', fontsize=12)
ax3.set_title('Amdahl Efficiency', fontsize=14, fontweight='bold')
ax3.legend(fontsize=9)
ax3.grid(True, alpha=0.3)
ax3.set_xscale('log', base=2)

# Plot 4: Gustafson Efficiency
eff_gus_ex3 = (gustafson_ex3 / processors) * 100
eff_gus_ex4 = (gustafson_ex4 / processors) * 100
ax4.plot(processors, eff_gus_ex3, 'o-', linewidth=2, markersize=8, 
         label='Exercise 3', color='blue')
ax4.plot(processors, eff_gus_ex4, 's-', linewidth=2, markersize=8, 
         label='Exercise 4', color='red')
ax4.axhline(y=100, color='k', linestyle=':', alpha=0.3, label='100%')
ax4.set_xlabel('Number of Processors (p)', fontsize=12)
ax4.set_ylabel('Efficiency (%)', fontsize=12)
ax4.set_title('Gustafson Efficiency', fontsize=14, fontweight='bold')
ax4.legend(fontsize=9)
ax4.grid(True, alpha=0.3)
ax4.set_xscale('log', base=2)

plt.tight_layout()
plt.savefig('scalability_comparison.png', dpi=300, bbox_inches='tight')
print("Plot saved: scalability_comparison.png")
PLOT_SCRIPT_END
    
    chmod +x generate_plots.py
    
    # Try to generate plots
    if command -v python3 &> /dev/null; then
        if python3 -c "import matplotlib" 2>/dev/null; then
            python3 generate_plots.py "$FS_EX3" "$FS_EX4"
            if [ -f scalability_comparison.png ]; then
                echo "\u2713 Plots generated successfully!"
            fi
        else
            echo "matplotlib not available - skipping plot generation"
            echo "You can run generate_plots.py locally with: python3 generate_plots.py $FS_EX3 $FS_EX4"
        fi
    fi
fi

#===========================================
# CALLGRIND VISUALIZATION
#===========================================
echo ""
echo "########################################"
echo "# CALLGRIND VISUALIZATION"
echo "########################################"
echo ""

# Generate Callgrind visualization if data exists
if [ -f callgrind_ex3.out ] || [ -f callgrind_ex4.out ]; then
    cat > visualize_callgrind.py << 'CALLGRIND_VIS_END'
#!/usr/bin/env python3
import sys
import re
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

def parse_callgrind(filename):
    """Parse callgrind annotated file to extract function costs"""
    functions = {}
    
    try:
        with open(filename, 'r') as f:
            content = f.read()

        # Find the function listing section
        pattern = r'(\d[\d,]+)\s+\([\d.]+%\)\s+([^:\[]+)'
        matches = re.findall(pattern, content)

        for match in matches:
            instructions = int(match[0].replace(',', ''))
            func_name = match[1].strip()
            # Clean function names
            if 'exercise' in func_name.lower() or any(x in func_name for x in 
                ['add_noise', 'init_b', 'compute_addition', 'reduction', 
                 'generate_noise', 'init_matrix', 'matmul']):
                functions[func_name] = instructions

        return functions
    except:
        return {}

# Parse both exercises
funcs_ex3 = parse_callgrind('callgrind_ex3_annotated.txt')
funcs_ex4 = parse_callgrind('callgrind_ex4_annotated.txt')

if funcs_ex3 or funcs_ex4:
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))
    
    # Exercise 3
    if funcs_ex3:
        names = list(funcs_ex3.keys())
        values = list(funcs_ex3.values())
        total = sum(values)
        percentages = [(v/total)*100 for v in values]

        axes[0].barh(names, percentages, color='steelblue')
        axes[0].set_xlabel('Percentage of Instructions', fontsize=12)
        axes[0].set_title('Exercise 3: Callgrind Profile', fontsize=14, fontweight='bold')
        axes[0].grid(True, alpha=0.3, axis='x')

        # Add percentage labels
        for i, (name, pct) in enumerate(zip(names, percentages)):
            axes[0].text(pct + 1, i, f'{pct:.2f}%', va='center', fontsize=10)
    
    # Exercise 4
    if funcs_ex4:
        names = list(funcs_ex4.keys())
        values = list(funcs_ex4.values())
        total = sum(values)
        percentages = [(v/total)*100 for v in values]

        axes[1].barh(names, percentages, color='coral')
        axes[1].set_xlabel('Percentage of Instructions', fontsize=12)
        axes[1].set_title('Exercise 4: Callgrind Profile', fontsize=14, fontweight='bold')
        axes[1].grid(True, alpha=0.3, axis='x')

        # Add percentage labels
        for i, (name, pct) in enumerate(zip(names, percentages)):
            axes[1].text(pct + 0.5, i, f'{pct:.2f}%', va='center', fontsize=10)
    
    plt.tight_layout()
    plt.savefig('callgrind_profiles.png', dpi=300, bbox_inches='tight')
    print("Callgrind visualization saved: callgrind_profiles.png")
else:
    print("Could not parse Callgrind data")
CALLGRIND_VIS_END
    
    chmod +x visualize_callgrind.py
    
    if command -v python3 &> /dev/null; then
        if python3 -c "import matplotlib" 2>/dev/null; then
            python3 visualize_callgrind.py
        else
            echo "matplotlib not available for Callgrind visualization"
        fi
    fi
else
    echo "No Callgrind data found - visualization skipped"
fi

#===========================================
# SUMMARY
#===========================================
echo ""
echo "########################################"
echo "# SUMMARY"
echo "########################################"
echo ""

if [ -f exercise3_results.txt ] && [ -f exercise4_results.txt ]; then
    echo "Both exercises completed successfully!"
    echo ""
    
    echo "Exercise 3 Sequential Fraction:"
    grep "fs (sequential fraction)" exercise3_results.txt
    
    echo ""
    echo "Exercise 4 Sequential Fraction:"
    grep "fs (sequential fraction)" exercise4_results.txt
    
    echo ""
    echo "Files created in results/ directory:"
    ls -lh
    
    echo ""
    echo "Generated files:"
    [ -f analysis_tables.txt ] && echo "  \u2713 analysis_tables.txt - Amdahl & Gustafson tables"
    [ -f scalability_comparison.png ] && echo "  \u2713 scalability_comparison.png - 4-panel scalability plots"
    [ -f callgrind_profiles.png ] && echo "  \u2713 callgrind_profiles.png - Callgrind visualization"
    [ -f callgrind_ex3_annotated.txt ] && echo "  \u2713 callgrind_ex3_annotated.txt - Exercise 3 profiling"
    [ -f callgrind_ex4_annotated.txt ] && echo "  \u2713 callgrind_ex4_annotated.txt - Exercise 4 profiling"
fi

echo ""
echo "========================================"
echo "TP2 completed!"
echo "End time: $(date)"
echo "========================================"

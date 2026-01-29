#!/bin/bash
# Automated HPL Testing Script - Single Process (P=1, Q=1)
# Tests all combinations of N and NB as specified in the exercise

# Define test parameters
N_SIZES=(1000 5000 10000 20000)
NB_SIZES=(1 2 4 8 16 32 64 128 256)

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BIN_DIR="${SCRIPT_DIR}"

# Results file
RESULTS_FILE="${BIN_DIR}/hpl_results.csv"
LOG_DIR="${BIN_DIR}/logs"

# Create logs directory
mkdir -p "$LOG_DIR"

# Check if xhpl exists
if [ ! -f "${BIN_DIR}/xhpl" ]; then
    echo "ERROR: xhpl executable not found in ${BIN_DIR}"
    echo "Please make sure HPL is built successfully"
    exit 1
fi

# Use 1 process as required by the exercise
MPI_CMD="mpirun -np 1"

echo "=========================================="
echo "HPL Benchmark Testing Suite"
echo "Single Process Execution (P=1, Q=1)"
echo "=========================================="
echo ""

# Create results file with header
echo "N,NB,P,Q,Time(s),GFLOPS,Residual,Status,Timestamp" > "$RESULTS_FILE"

# Counter for progress
TOTAL_TESTS=$((${#N_SIZES[@]} * ${#NB_SIZES[@]}))
CURRENT_TEST=0

echo "Total tests to run: $TOTAL_TESTS"
echo "Results will be saved to: $RESULTS_FILE"
echo "Log files will be saved to: $LOG_DIR"
echo ""
echo "Estimated time: 20-40 minutes"
echo ""

# Start time
OVERALL_START=$(date +%s)

# Loop through all combinations
for N in "${N_SIZES[@]}"; do
    echo ""
    echo "=========================================="
    echo "Testing Matrix Size N = $N"
    echo "=========================================="
    echo ""
    
    for NB in "${NB_SIZES[@]}"; do
        CURRENT_TEST=$((CURRENT_TEST + 1))
        
        echo "Test $CURRENT_TEST/$TOTAL_TESTS: N=$N, NB=$NB"
        
        # Create HPL.dat for this configuration
        cat > HPL.dat << EOF
HPLinpack benchmark input file
Innovative Computing Laboratory, University of Tennessee
HPL.out      output file name (if any)
6            device out (6=stdout,7=stderr,file)
1            # of problems sizes (N)
$N           Ns
1            # of NBs
$NB          NBs
0            PMAP process mapping (0=Row-,1=Column-major)
1            # of process grids (P x Q)
1            Ps
1            Qs
16.0         threshold
1            # of panel fact
2            PFACTs (0=left, 1=Crout, 2=Right)
1            # of recursive stopping criterium
4            NBMINs (>= 1)
1            # of panels in recursion
2            NDIVs
1            # of recursive panel fact.
1            RFACTs (0=left, 1=Crout, 2=Right)
1            # of broadcast
1            BCASTs (0=1rg,1=1rM,2=2rg,3=2rM,4=Lng,5=LnM)
1            # of lookahead depth
1            DEPTHs (>=0)
2            SWAP (0=bin-exch,1=long,2=mix)
64           swapping threshold
0            L1 in (0=transposed,1=no-transposed) form
0            U  in (0=transposed,1=no-transposed) form
1            Equilibration (0=no,1=yes)
8            memory alignment in double (> 0)
EOF
        
        # Log file for this test
        LOG_FILE="${LOG_DIR}/hpl_N${N}_NB${NB}.log"
        
        # Run HPL with MPI
        TEST_START=$(date +%s)
        $MPI_CMD ./xhpl > "$LOG_FILE" 2>&1
        TEST_END=$(date +%s)
        TEST_ELAPSED=$((TEST_END - TEST_START))
        
        # Parse results
        if grep -q "PASSED" "$LOG_FILE"; then
            STATUS="PASSED"
        elif grep -q "FAILED" "$LOG_FILE"; then
            STATUS="FAILED"
        else
            STATUS="ERROR"
        fi
        
        # Extract performance data
        # Look for the line with performance results (WR format)
        RESULT_LINE=$(grep "^WR" "$LOG_FILE" | tail -1)
        
        if [ -n "$RESULT_LINE" ]; then
            # Parse the result line
            # Format: WR... N NB P Q Time Gflops
            P=$(echo "$RESULT_LINE" | awk '{print $4}')
            Q=$(echo "$RESULT_LINE" | awk '{print $5}')
            TIME=$(echo "$RESULT_LINE" | awk '{print $6}')
            GFLOPS=$(echo "$RESULT_LINE" | awk '{print $7}')
            
            # Extract residual if available
            RESIDUAL=$(grep "||Ax-b||_oo" "$LOG_FILE" | tail -1 | awk '{print $2}')
            if [ -z "$RESIDUAL" ]; then
                RESIDUAL="N/A"
            fi
        else
            P="1"
            Q="1"
            TIME="ERROR"
            GFLOPS="ERROR"
            RESIDUAL="N/A"
        fi
        
        # Get timestamp
        TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
        
        # Record results
        echo "$N,$NB,$P,$Q,$TIME,$GFLOPS,$RESIDUAL,$STATUS,$TIMESTAMP" >> "$RESULTS_FILE"
        
        # Display results
        printf "  %-10s %s\n" "Status:" "$STATUS"
        printf "  %-10s %s seconds (%ds elapsed)\n" "Time:" "$TIME" "$TEST_ELAPSED"
        printf "  %-10s %s GFLOPS\n" "Performance:" "$GFLOPS"
        echo ""
    done
done

# Calculate overall time
OVERALL_END=$(date +%s)
OVERALL_ELAPSED=$((OVERALL_END - OVERALL_START))
MINUTES=$((OVERALL_ELAPSED / 60))
SECONDS=$((OVERALL_ELAPSED % 60))

echo "=========================================="
echo "All tests completed!"
echo "=========================================="
echo ""
echo "Total time: ${MINUTES}m ${SECONDS}s"
echo ""
echo "Results saved to: $RESULTS_FILE"
echo "Logs saved to: $LOG_DIR"
echo ""

# Summary statistics
PASSED=$(grep "PASSED" "$RESULTS_FILE" | wc -l | tr -d ' ')
FAILED=$(grep "FAILED" "$RESULTS_FILE" | wc -l | tr -d ' ')
ERROR=$(grep "ERROR" "$RESULTS_FILE" | wc -l | tr -d ' ')

echo "Summary:"
echo "  Tests passed: $PASSED"
echo "  Tests failed: $FAILED"
echo "  Tests with errors: $ERROR"
echo ""

# Find best performance
echo "Best Performance:"
tail -n +2 "$RESULTS_FILE" | sort -t',' -k6 -rn | head -1 | awk -F',' '{
    printf "  N=%s, NB=%s: %.2f GFLOPS\n", $1, $2, $6
}'
echo ""

echo "Next steps:"
echo "  1. View results: cat $RESULTS_FILE"
echo "  2. Analyze results: python3 analyze_results.py"
echo ""
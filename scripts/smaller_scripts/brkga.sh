#!/bin/bash

instance="${1:-A}" 
run="${2:-1}"

echo "Executing BRKGA-NSGA2 with: Instance=$instance, Run=$run"

log_dir="../dataset/brkga/$instance/$run"
log_file="$log_dir/log.txt"

path="../dataset/brkga/$instance/$run/"
log_path="../dataset/moead/$instance/$run/log.txt"




mkdir -p "$log_dir"

{
    echo "============================== LOG =============================="
    echo "Instance: $instance"
    echo "Run: $run"
    echo "Metaheuristic: BRKGA-NSGA2"
    echo "Execution started at: $(date)"

    { time ./exe/brkga "$instance" "$path"; } 2>&1

    echo "Execution ended at: $(date)"
    echo "================================================================="
} > "$log_file" 2>&1

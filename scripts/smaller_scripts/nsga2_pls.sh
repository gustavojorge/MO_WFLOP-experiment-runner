#!/bin/bash

instance="${1:-A}" 
run="${2:-1}"

echo "Executing NSGA-II_PLS with: Instance=$instance, Run=$run"

log_dir="../dataset/nsga2_pls/$instance/$run"
log_file="$log_dir/log.txt"

path="../dataset/nsga2_pls/$instance/$run/"
log_path="../dataset/moead/$instance/$run/log.txt"

angle=$(grep "Angle:" "$log_path" | awk '{print $2}')
wind=$(grep "Wind:" "$log_path" | awk '{print $2}')

mkdir -p "$log_dir"

{
    echo "============================== LOG =============================="
    echo "Instance: $instance"
    echo "Run: $run"
    echo "Metaheuristic: COMOLS/D"
    echo "Execution started at: $(date)"

    { time ./exe/nsga2_pls "$instance" "$path" "$angle" "$wind"; } 2>&1

    echo "Execution ended at: $(date)"
    echo "================================================================="
} > "$log_file" 2>&1

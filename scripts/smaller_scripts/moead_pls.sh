#!/bin/bash

instance="${1:-A}" 
run="${2:-1}"

echo "Executing MOEA/D_PLS with: Instance=$instance, Run=$run"

log_dir="../dataset/moead_pls/$instance/$run"
log_file="$log_dir/log.txt"

path="../dataset/moead_pls/$instance/$run/"
log_path="../dataset/moead/$instance/$run/log.txt"

angle=$(grep "Angle:" "$log_path" | awk '{print $2}')
wind=$(grep "Wind:" "$log_path" | awk '{print $2}')

mkdir -p "$log_dir"

{
    echo "============================== LOG =============================="
    echo "Instance: $instance"
    echo "Run: $run"
    echo "Metaheuristic: MOEA/D Pareto Local Search"
    echo "Execution started at: $(date)"

    { time ./exe/moead_pls "$instance" "$path" "$angle" "$wind"; } 2>&1

    echo "Execution ended at: $(date)"
    echo "================================================================="
} > "$log_file" 2>&1

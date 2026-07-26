#!/bin/bash

ALGO="${ALGO:-nsga2_hybrid}"
LS="${LS:-apls}"

instance="${1:-A}" 
run="${2:-1}"

echo "Executing $ALGO with: Instance=$instance, Run=$run"

log_dir="../dataset/$ALGO_$LS/$instance/$run"
log_file="$log_dir/log.txt"

path="../dataset/$ALGO_$LS/$instance/$run/"

angle=0.0
wind=0.0

base_log="../dataset/${ANGLE_WIND_SOURCE:-moead}/$instance/$run/log.txt"
if [[ -f "$base_log" ]]; then
    read_angle=$(grep "Angle:" "$base_log" | awk '{print $2}')
    read_wind=$(grep "Wind:" "$base_log" | awk '{print $2}')
    [[ -n "$read_angle" ]] && angle="$read_angle"
    [[ -n "$read_wind" ]] && wind="$read_wind"
fi

mkdir -p "$log_dir"

{
    echo "============================== LOG =============================="
    echo "Instance: $instance"
    echo "Run: $run"
    echo "Metaheuristic: $ALGO"
    echo "Local Search: $LS"
    echo "Execution started at: $(date)"

    { time ./exe/main "$instance" "$path" "$angle" "$wind" "$ALGO" "$LS"; } 2>&1

    echo "Execution ended at: $(date)"
    echo "================================================================="
} > "$log_file" 2>&1

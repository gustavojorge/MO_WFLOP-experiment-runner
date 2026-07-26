#!/bin/bash

ALGO="${ALGO:-nsga2_hybrid}"
LS="${LS:-apls}"

main_dir="../dataset/$ALGO_$LS"

if [[ -d "$main_dir" ]]; then
    echo "The directory structure for $ALGO_$LS has already been created."
    exit 0
fi

mkdir -p "$main_dir"

echo "------------- Creating the directory structure for $ALGO with LS=$LS... -------------"

for D in {A..J} {0..500} {501..505}; do
    for subdir in {1..20}; do
        mkdir -p "$main_dir/$D/$subdir"
    done
done

echo "---------- Directory structure created successfully for $ALGO_$LS! ----------"

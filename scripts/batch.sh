#!/bin/bash

# How to call the batch
# LS=$LS ALGO=$ALGO ./batch.sh

ALGO="${ALGO:-nsga2_hybrid}"
LS="${LS:-apls}"
export ALGO
export LS

script="./main.sh"
create_folders="./smaller_scripts/create_folders.sh"

echo "Running script 1 = create_folders.sh for ALGO=$ALGO LS=$LS"
bash "$create_folders"

nohup "$script" 501 502 504 503 505 A 61 87 95 113 &> "./logs/${ALGO}/batch_501-505_61-113_A.txt" &
nohup "$script" 115 161 164 185 216 226 &> "./logs/${ALGO}/batch_115-226.txt" &
nohup "$script" 246 278 324 342 347 352 355 359 &> "./logs/${ALGO}/batch_246-359.txt" &
nohup "$script" 363 431 441 464 482 495 &> "./logs/${ALGO}/batch_363-495.txt" &
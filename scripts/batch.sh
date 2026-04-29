#!/bin/bash

script="./main.sh"
create_folders="./smaller_scripts/create_folders.sh"

echo "Running script 1 = create_folders.sh"
bash "$create_folders"

nohup "$script" 501 502 504 503 505 A 61 87 95 113 115 161 164 185 216 226 246 278 324 342 347 352 355 359 363 431 441 464 482 495 &> "./logs/batch_small_instances.txt" &
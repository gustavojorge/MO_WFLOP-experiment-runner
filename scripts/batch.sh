#!/bin/bash

script="./main.sh"
create_folders="./smaller_scripts/create_folders.sh"

echo "Running script 1 = create_folders.sh"
bash "$create_folders"

nohup "$script" 501 502 503 504 505 A 216 347 352 464 &> "./logs/batch_small_instances.txt" &
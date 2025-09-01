#!/bin/bash

script="./main.sh"
create_folders="./smaller_scripts/create_folders.sh"

echo "Running script 1 = create_folders.sh"
bash "$create_folders"

nohup "$script" A &> "./logs/batch_A.txt" &
nohup "$script" 172 &> "./logs/batch_172.txt" &
nohup "$script" 456 &> "./logs/batch_456.txt" &
nohup "$script" 240 &> "./logs/batch_240.txt" &

# start=1
# end=500
# step=10

# for ((i=$start; i<=$end; i+=$step)); do
#   batch_start=$i
#   batch_end=$((i + step - 1))
  
#   if ((batch_end > end)); then
#     batch_end=$end
#   fi

#   batch=$(seq $batch_start $batch_end | tr '\n' ' ')

#   nohup "$script" $batch &> "./logs/batch_${batch_start}_to_${batch_end}.txt" &
# done

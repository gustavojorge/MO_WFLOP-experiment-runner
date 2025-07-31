# Metaheuristics/Metafeatures Execution Scripts

This repository provides the necessary scripts to compile and run multiple multi-objective metaheuristics, such as MOEA/D, NSGA-II, and CoMOLS/D. Besides, also contains the necessary scripts to compile and run metafeatures extraction methods.

---

## How to Execute

To run:

1. First, execute the compilation script:

```bash
./makeall.sh
```

This compiles all dependencies and generates the executables (e.g., `nsga2`, `moead`, `comolsd`, `adaptative_walk_decomposition`, `random_walk_decomposition`...) in the `/scripts/exe` directory, and also applies execution permissions to the scripts.

2. Then, from the `/scripts` directory, run:

```bash
./batch.sh
```

This script initiates the execution of instances in batches.

---

## Project Structure

```
.
├── makeall.sh
├── src/                       # Source code for each metaheuristic/metafeature
├── scripts/
│   ├── batch.sh               # Launches batches of instances
│   ├── main.sh                # Dispatches each instance to the correct script
│   ├── exe/                   # Compiled executables (e.g., nsga2, moead, comolsd)
│   ├── logs/                  # Log outputs
│   └── smaller_scripts/
│       ├── create_folders.sh  # Creates the output directory structure
│       └── nsga2.sh           # Executes, (e.g. the nsga2) executable with the correct instance and path
```

---

## Script Descriptions

### `makeall.sh`

Compiles all dependencies and generates the final executables (e.g., `nsga2`, `comolsd`, `moead`, `random_walk_decomposition`) which are stored in `/scripts/exe`.  
Also grants execution permissions for all scripts and executables.

### `scripts/batch.sh`

Executes the metaheuristic/metafeatures instances in batches:

- Initially runs 11 instances: A through J and instance 0.
- Then runs from instance 1 to 500 in steps of 10.
- Finally, runs instances 501 to 505.

Each batch is passed to `main.sh`.

```bash
nohup ./main.sh A B C D E F G H I J 0 &> ./logs/batch_A_to_J_plus_0.txt &
nohup ./main.sh 1 2 3 ... &> ./logs/batch_1_to_10.txt &
...
nohup ./main.sh 501 502 503 504 505 &> ./logs/batch_501_to_505.txt &
```

### `scripts/main.sh`

Receives a list of instances from `batch.sh` and for each instance:

- Calls the corresponding execution script (e.g., `nsga2.sh`) 20 times.
- Stores the logs of each run in `/scripts/logs`.

### `scripts/smaller_scripts/create_folders.sh`

Creates the folder structure for results inside `../dataset/nsga2/`.  
For each instance and for 20 runs, it creates the necessary directories.

Example of created structure:

```
dataset/
└── nsga2/
    ├── A/
    │   ├── 1/
    │   └── ...
    ├── 0/
    ├── 1/
    └── ...
```

It will be automatically called by `batch.sh`.

### `scripts/smaller_scripts/nsga2.sh` (or `comolsd.sh`, `moead.sh`, etc.)

Receives the instance and run number, calls the executable, and stores the output log in `../dataset/nsga2/<instance>/<run>/log.txt`.

```
dataset/nsga2/{instance}/{run}/log.txt
```

Example call:

```bash
./exe/nsga2 A dataset/nsga2/A/1/
```

---

## Example Workflow

```bash
# Step 1: Compile everything
./makeall.sh

# Step 2: Run all instances from the scripts directory
cd scripts
./batch.sh
```

Results will be saved in the `../dataset/nsga2/` directory.

---

## Notes

- All executions use `nohup` to run in the background, allowing large numbers of experiments to be executed asynchronously.

- Results are organized by metaheuristic, instance, and run.

- To access an especific metaheurist, just go to he branch with its name on it. Exemple: NSGA-II its in the branch \"nsga2-runner\", MOEA/D its in \"moead-runner\" and so on.",

---

## Supported Metaheuristics/Metafeatures

✅ NSGA-II  
✅ MOEA/D  
✅ CoMOLS/D  
✅ Adaptive Walk Decomposition

✅ Random Walk Decomposition

✅ Adaptive Walk Pareto

✅ Random Walk Pareto

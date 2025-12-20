# Distributed Computing Labs

A comprehensive collection of MPI-based parallel computing implementations developed for the Distributed Computing course. This repository contains implementations of fundamental parallel algorithms with performance analysis across multiple processor configurations.

## 📋 Table of Contents

- [Overview](#overview)
- [Labs Description](#labs-description)
- [Technologies Used](#technologies-used)
- [Project Structure](#project-structure)
- [Building and Running](#building-and-running)
- [Requirements](#requirements)

## <a name="overview"></a> 🎯 Overview

This repository demonstrates the implementation and analysis of various parallel algorithms using MPI (Message Passing Interface). Each lab compares serial and parallel implementations, measuring speedup and efficiency across different problem sizes and processor counts.

## <a name="labs-description"></a> 📚 Labs Description

### Lab 1: Parallel Matrix-Vector Multiplication

Implementation of parallel matrix-vector multiplication using row-wise decomposition. The matrix is distributed among processes in horizontal stripes, and each process computes partial results for its assigned rows. Data distribution uses `MPI_Scatterv` for non-uniform load balancing, and results are gathered using `MPI_Allgatherv`.

**Performance testing:** Matrices up to 10,000×10,000

**Files:**
- `SerialMV.cpp` - Serial implementation
- `ParallelMV.cpp` - Parallel implementation
- `testMV.cpp` - Automated testing suite

### Lab 2: Parallel Matrix Multiplication (Fox's Algorithm)

Implementation of Fox's algorithm for parallel matrix multiplication. The algorithm uses a 2D process grid topology where matrices are decomposed in a checkerboard pattern. In each iteration, blocks of matrix A are broadcast horizontally across process rows, while blocks of matrix B are shifted vertically in a circular manner. Each process performs local block multiplication and accumulates results.

**Performance testing:** Matrices up to 3,000×3,000  
**Note:** Requires perfect square number of processes (4, 9, 16, etc.)

**Files:**
- `SerialMM.cpp` - Serial implementation
- `ParallelMM.cpp` - Parallel Fox's algorithm
- `TestParallelMM.cpp` - Performance testing suite

### Lab 3: Parallel Gaussian Elimination

Implementation of parallel Gaussian elimination with partial pivoting for solving systems of linear equations. The algorithm distributes matrix rows among processes and iteratively selects pivot rows using `MPI_Allreduce` with `MPI_MAXLOC` operation. After pivot selection, the pivot row is broadcast to all processes for column elimination. Back substitution is performed in parallel from bottom to top.

**Performance testing:** Systems up to 3,000×3,000

**Files:**
- `SerialGauss.cpp` - Serial Gaussian elimination
- `ParallelGauss.cpp` - Parallel implementation
- `TestParallelGauss.cpp` - Automated testing

### Lab 4: Parallel Bubble Sort (Odd-Even Transposition Sort)

Implementation of parallel odd-even transposition sort. The algorithm distributes data among processes, performs local sorting, then alternates between odd and even phases. In odd phases, processes with odd ranks exchange data with right neighbors; in even phases, processes with even ranks exchange with right neighbors. After each exchange, processes merge received data with local data and keep the appropriate half based on their position.

**Performance testing:** Datasets up to 50,000 elements

**Files:**
- `SerialBubbleSort.cpp` - Serial implementation
- `ParallelBubbleSort.cpp` - Parallel implementation
- `TestParBubbleSort.cpp` - Performance analysis

### Lab 5: Parallel Floyd-Warshall Algorithm

Implementation of parallel all-pairs shortest path algorithm. The adjacency matrix is distributed row-wise among processes. In each iteration k, the process holding row k broadcasts it to all other processes. Each process then updates its local rows by checking if paths through vertex k provide shorter distances. The algorithm handles sparse graphs using -1 to represent infinity (no direct edge).

**Performance testing:** Graphs up to 1,000 vertices

**Files:**
- `SerialFloyd.cpp` - Serial Floyd-Warshall
- `ParallelFloyd.cpp` - Parallel implementation
- `TestParallelFloyd.cpp` - Performance benchmarking

### Lab 6: Parallel Gauss-Seidel Method

Implementation of parallel iterative Gauss-Seidel method for solving the Laplace equation on a 2D grid. The grid is distributed row-wise among processes with overlapping boundary rows for neighbor communication. Each iteration exchanges boundary data using `MPI_Sendrecv`, updates interior grid points using the five-point stencil formula, and computes local maximum deviation. Global convergence is determined using `MPI_Allreduce` with `MPI_MAX` operation. The algorithm iterates until the maximum change across all processes falls below the specified tolerance.

**Performance testing:** Grids up to 10,000×10,000

**Files:**
- `SerialGS.cpp` - Serial Gauss-Seidel implementation
- `ParallelGS.cpp` - Parallel implementation
- `TestParallelGS.cpp` - Automated performance testing
- `TestSerialGS.cpp` - Serial performance benchmarking

## <a name="technologies-used"></a> 🛠 Technologies Used

- **MPI (Message Passing Interface)** - Parallel programming framework
- **C/C++** - Implementation language
- **SLURM** - Job scheduling system for cluster execution
- **mpicxx** - MPI C++ compiler wrapper

## <a name="project-structure"></a> 📁 Project Structure

```
.
├── lab1/                          # Matrix-Vector Multiplication
│   ├── SerialMV.cpp
│   ├── ParallelMV.cpp
│   ├── testMV.cpp
│   └── tasks_slurm/              # SLURM job scripts
├── lab2/                          # Matrix Multiplication (Fox)
│   ├── SerialMM.cpp
│   ├── ParallelMM.cpp
│   ├── TestParallelMM.cpp
│   └── tasks_slurm/
├── lab3/                          # Gaussian Elimination
│   ├── SerialGauss.cpp
│   ├── ParallelGauss.cpp
│   ├── TestParallelGauss.cpp
│   └── tasks_slurm/
├── lab4/                          # Bubble Sort
│   ├── SerialBubbleSort.cpp
│   ├── ParallelBubbleSort.cpp
│   ├── TestParBubbleSort.cpp
│   └── tasks_slurm/
├── lab5/                          # Floyd-Warshall
│   ├── SerialFloyd.cpp
│   ├── ParallelFloyd.cpp
│   ├── TestParallelFloyd.cpp
│   └── tasks_slurm/
└── lab6/                          # Gauss-Seidel Method
    ├── SerialGS.cpp
    ├── ParallelGS.cpp
    ├── TestParallelGS.cpp
    ├── TestSerialGS.cpp
    └── tasks_slurm/
```

## <a name="building-and-running"></a> 🚀 Building and Running

### Compilation

```bash
# Navigate to any lab directory
cd lab1/

# Compile serial version
g++ -o serial SerialMV.cpp

# Compile parallel version
mpicxx -o parallel ParallelMV.cpp

# Compile test version
mpicxx -o testmv testMV.cpp
```

### Local Execution

```bash
# Run serial version
./serial

# Run parallel version with 4 processes
mpirun -np 4 ./parallel

# Run test suite
mpirun -np 4 ./testmv
```

### SLURM Cluster Execution

```bash
cd tasks_slurm/

# Submit job with 2 processes
sbatch run_2.slurm

# Submit job with 4 processes
sbatch run_4.slurm

# Submit job with 8 processes
sbatch run_8.slurm

# Check job status
squeue -u $USER

# View output
cat output_4.txt
```


## <a name="requirements"></a> 📋 Requirements

### Software Dependencies:
- MPI implementation (OpenMPI or MPICH)
- GCC/G++ compiler (C++11 or later)
- SLURM (for cluster execution)

### Installation (Ubuntu/Debian):
```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libopenmpi-dev
```

### Installation (macOS):
```bash
brew install open-mpi
```

### Verification:
```bash
# Check MPI installation
mpirun --version
mpicxx --version

# Test MPI
mpirun -np 2 hostname
```

## 🔍 Key Algorithms and Techniques


### MPI Communication Patterns:
- Point-to-point: `MPI_Send`, `MPI_Recv`, `MPI_Sendrecv`
- Collective: `MPI_Bcast`, `MPI_Scatter`, `MPI_Gather`, `MPI_Allreduce`
- Advanced: `MPI_Scatterv`, `MPI_Gatherv`, `MPI_Allgatherv`

### Load Balancing:
- Dynamic row distribution for non-uniform process counts
- Remainder distribution among processes

## 📝 Notes

- Lab 2 (Fox's Algorithm) requires a perfect square number of processes
- Random data initialization uses time-based seeding
- All implementations include correctness verification against serial versions
- SLURM scripts are configured for the `scit5` partition

## 📄 License

This project is developed for educational purposes as part of the Distributed Computing course.

---

**Course**: Distributed Computing  
**Academic Year**: 2025-2026  
**Institution**: KNU, FCSC

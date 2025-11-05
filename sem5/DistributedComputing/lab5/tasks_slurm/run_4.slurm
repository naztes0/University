#!/bin/bash
#SBATCH -J floyd_4
#SBATCH -o output_4.txt
#SBATCH -e error_4.txt
#SBATCH -n 4
#SBATCH -t 00:5:00
#SBATCH -p scit5

mpirun -np 4 ./testparfloyd

#!/bin/bash
#SBATCH -J floyd_2
#SBATCH -o output_2.txt
#SBATCH -e error_2.txt
#SBATCH -n 2
#SBATCH -t 00:5:00
#SBATCH -p scit5

mpirun -np 2 ./testparfloyd

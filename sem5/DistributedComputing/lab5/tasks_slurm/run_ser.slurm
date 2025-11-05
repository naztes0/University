#!/bin/bash
#SBATCH -J ser_floyd
#SBATCH -o output_ser.txt
#SBATCH -e error_ser.txt
#SBATCH -t 00:02:00
#SBATCH -n 1
#SBATCH -p scit5

./testserfloyd


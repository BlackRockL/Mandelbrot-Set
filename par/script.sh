#!/bin/bash

#$ -q BrainLab.owner
#$ -cwd
#$ -N pa2
#$ -pe orte-rr 50
#$ -M nitishn@gmail.com
#$ -m s
#$ -o pa2.o
#$ -e pa2.e

export PATH=/usr/lib64/openmpi-1.5.3/bin:$PATH
export LD_LIBRARY_PATH=/usr/lib64/openmpi-1.5.3/lib:$LD_LIBRARY_PATH

for i in $(seq 50 2 50)
do
	for j in $(seq 1 1 1)
		do
			mpirun -np $i ./pa2 >> dump/output
	done
done


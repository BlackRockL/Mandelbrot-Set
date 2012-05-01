#!/bin/bash

#$ -q BrainLab.owner
#$ -cwd
#$ -N pa2
#$ -pe orte-rr 4 
#$ -M nitishn@gmail.com
#$ -m s
#$ -o pa2.o
#$ -e pa2.e

export PATH=/usr/lib64/openmpi-1.5.3/bin:$PATH
export LD_LIBRARY_PATH=/usr/lib64/openmpi-1.5.3/lib:$LD_LIBRARY_PATH

#for i in $(seq 2 2 20)
#do
#	for j in $(seq 1 1 5)
#		do
			mpirun -np $NSLOTS ./pa2 >> output
#	done
#done


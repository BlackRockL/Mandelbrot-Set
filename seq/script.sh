#!/bin/bash

#$ -q BrainLab.owner
#$ -cwd
#$ -pe orte 1 
#$ -M nitishn@gmail.com
#$ -m s
#$ -o myJob.o
#$ -e myJob.e

for i in {1..20}
do
	for j in {1..5}
	do
		./seq$i >> out$i
	done
done

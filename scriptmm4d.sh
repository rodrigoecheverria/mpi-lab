#!/bin/bash
#$ -N mm_4d
#$ -q aolin.q
#$ -v SGE_QMASTER_PORT
#$ -cwd
#$ -l h_rt=3600
module load openmpi/1.6.3

mpirun -np 4 $HOME/code/mpilab2/mul.o 8000 16000
exit 0


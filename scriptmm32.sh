#!/bin/bash
#$ -N mm_32
#$ -q aolin.q
#$ -v SGE_QMASTER_PORT
#$ -cwd
#$ -l h_rt=600
module load openmpi/1.6.3
mpirun -np 32 $HOME/code/mpilab2/mul.o 1000 2000
mpirun -np 32 $HOME/code/mpilab2/mul.o 2000 4000
mpirun -np 32 $HOME/code/mpilab2/mul.o 4000 8000
mpirun -np 32 $HOME/code/mpilab2/mul.o 8000 16000
exit 0


#!/bin/bash
#$ -N jac_32_24
#$ -q aolin.q
#$ -v SGE_QMASTER_PORT
#$ -cwd
#$ -l h_rt=3600
module load openmpi/1.6.3

mpirun -np 32 $HOME/code/mpilab2/jac.o 24 2
mpirun -np 32 $HOME/code/mpilab2/jac.o 24 4
mpirun -np 32 $HOME/code/mpilab2/jac.o 24 8
mpirun -np 32 $HOME/code/mpilab2/jac.o 24 16

exit 0


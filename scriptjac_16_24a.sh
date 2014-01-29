#!/bin/bash
#$ -N jac_16_24a
#$ -q aolin.q
#$ -v SGE_QMASTER_PORT
#$ -cwd
#$ -l h_rt=3600
module load openmpi/1.6.3

mpirun -np 16 $HOME/code/mpilab2/jac.o 24 2
mpirun -np 16 $HOME/code/mpilab2/jac.o 24 4
mpirun -np 16 $HOME/code/mpilab2/jac.o 24 8


exit 0


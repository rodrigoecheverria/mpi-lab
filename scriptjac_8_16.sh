#!/bin/bash
#$ -N jac_8_16
#$ -q aolin.q
#$ -v SGE_QMASTER_PORT
#$ -cwd
#$ -l h_rt=3600
module load openmpi/1.6.3
mpirun -np 8 $HOME/code/mpilab2/jac.o 16 2
mpirun -np 8 $HOME/code/mpilab2/jac.o 16 4
mpirun -np 8 $HOME/code/mpilab2/jac.o 16 8
mpirun -np 8 $HOME/code/mpilab2/jac.o 16 16
exit 0

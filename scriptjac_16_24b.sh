#!/bin/bash
#$ -N jac_16_24b
#$ -q aolin.q
#$ -v SGE_QMASTER_PORT
#$ -cwd
#$ -l h_rt=3600
module load openmpi/1.6.3

mpirun -np 16 $HOME/code/mpilab2/jac.o 24 16


exit 0


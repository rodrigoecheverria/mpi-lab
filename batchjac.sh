#!/bin/bash
qsub scriptjac_4_1.sh
qsub scriptjac_8_1.sh
qsub scriptjac_16_1.sh
qsub scriptjac_32_1.sh

qsub scriptjac_4_8.sh
qsub scriptjac_8_8.sh
qsub scriptjac_16_8.sh
qsub scriptjac_32_8.sh

qsub scriptjac_4_16.sh
qsub scriptjac_8_16.sh
qsub scriptjac_16_16.sh
qsub scriptjac_32_16.sh

qsub scriptjac_4_24.sh
qsub scriptjac_8_24.sh
qsub scriptjac_16_24a.sh
qsub scriptjac_16_24b.sh
qsub scriptjac_32_24.sh

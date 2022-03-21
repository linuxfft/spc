//
// Created by 华为 on 2022/3/11.
//

#ifndef SPC_SPC_H
#define SPC_SPC_H

#include "common.h"
#include "error.h"

double cpk(double *data, size_t length, double usl, double lsl);

ST_RET xbarSbar(double **data, size_t n_col, size_t n_row, SPC_RET **spc_ret);


#endif //SPC_SPC_H

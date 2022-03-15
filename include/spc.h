//
// Created by 华为 on 2022/3/11.
//

#ifndef SPC_SPC_H
#define SPC_SPC_H

#include "common.h"
#include "error.h"

float cpk(float *data, size_t length, float usl, float lsl);

int xbarSbar(float **data, size_t n_col, size_t n_row, SPC_RET *spc_ret);


#endif //SPC_SPC_H

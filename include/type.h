//
// Created by 华为 on 2022/3/14.
//

#ifndef SPC_TYPE_H
#define SPC_TYPE_H

typedef struct {
    int ret;
    double info;
} CALC_RET;

typedef struct {
    double *data;
    size_t length;
    double center;
    double lower;
    double upper;
} SPC_RET;

typedef struct {
    double *x_data;
    size_t x_size;
    double *y_data;
    size_t y_size;
} PLOT_RET;

#endif //SPC_TYPE_H

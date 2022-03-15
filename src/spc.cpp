//
// Created by 华为 on 2022/3/11.
//

#include "spc.h"
#include "table.h"
#include "stats.h"
#include "NumCpp.hpp"

using namespace nc;

/*!
 *
 * @param data : 二维浮点指针,
 * @param n_col: data指针的维度，数据长度,列数
 * @param n_row: data的行数
 * @return
 */

int xbarSbar(double **data, size_t n_col, size_t n_row, SPC_RET **spc_ret) {
    if (n_row < 2 || n_row > 10) {
        return ERROR_DATA_SIZE;
    }

    if (*spc_ret == NULL) {
        return ERROR_NULL_PTR;
    }

    if (data == NULL) {
        return ERROR_NULL_PTR;
    }

    double *S, *X = NULL;
    S = (double *) calloc(n_row, sizeof(double));
    X = (double *) calloc(n_row, sizeof(double));

    CALC_RET s_ret, x_ret;
    int ret = ERROR_NO_ERROR;
    double x_mean, s_std;

    for (int i = 0; i < n_row; ++i) {
        double *d = data[i];
        if (d == NULL) {
            return ERROR_NULL_PTR;
        }
        CalcMean(d, n_col, &x_ret);
        if (x_ret.ret == ERROR_NO_ERROR) {
            x_mean = x_ret.data;
            X[i] = x_mean;
        } else {
            return ERROR_NO_ERROR;
        }
        CalcStandardDeviation(data[i], n_col, 1, x_mean, &s_ret);
        if (s_ret.ret == ERROR_NO_ERROR) {
            s_std = s_ret.data;
            S[i] = s_std;
        }
    }

    CALC_RET sbar_ret, xbar_ret;

    CalcMean(S, n_row, &sbar_ret);
    CalcMean(X, n_row, &xbar_ret);

    double lclx = xbar_ret.data - A3[n_col] * sbar_ret.data;
    double uclx = xbar_ret.data + A3[n_col] * sbar_ret.data;

    memcpy_spc_ret(spc_ret, X, n_row);

    SPC_RET *pRet = *spc_ret;

    pRet->center = nc::round(sbar_ret.data, 2);
    pRet->lower = nc::round(lclx, 2);
    pRet->upper = nc::round(uclx, 2);

    return ERROR_NO_ERROR;
}


/*!
 *
 * @param data: 一维原始数据
 * @param length: 数据长度
 * @param usl
 * @param lsl
 * @return CPK数值
 */

float cpk(double *data, size_t length, double usl, double lsl) {
    float Cpk = 0.0;
    if (data == NULL) {
        return Cpk;
    }
    if (length <= 0) {
        return Cpk;
    }
    if (usl <= 0.0 || lsl <= 0.0) {
        return Cpk;
    }
    CALC_RET ret;
    ST_RET stRet;
    stRet = CalcMean(data, length, &ret);
    float mean = ret.data;

    stRet = CalcStandardDeviation(data, length, 1, mean, &ret);

    float sigma = ret.data;

    float Cpu = float(usl - mean) / (3 * sigma);
    float Cpl = float(mean - lsl) / (3 * sigma);
    Cpk = fmin(Cpu, Cpl);

    return Cpk;
}

/*!
 *
 * @param data: 一维原始数据
 * @param length: 数据长度
 * @param usl
 * @param lsl
 * @return CMK数值
 */
double cmk(double *data, size_t length, double usl, double lsl) {
    double Cmk = 0.0;
    if (data == NULL) {
        return Cmk;
    }
    if (length <= 0) {
        return Cmk;
    }
    if (usl <= 0.0 || lsl <= 0.0) {
        return Cmk;
    }
    CALC_RET ret;
    ST_RET stRet;
    stRet = CalcAvg(data, length, &ret);
    double avg = ret.data;

    if (usl < avg or lsl > avg) {
        return ERROR_OVER_FLOW;
    }

    stRet = CalcStandardDeviation(data, length, 1, 0.0, &ret);

    double sigma = ret.data;

    double a = (usl - avg) / (3 * sigma);
    double b = (avg - lsl) / (3 * sigma);
    Cmk = fmin(a, b);

    return Cmk;
}

/*!
 *
 * @param data: 一维原始数据
 * @param length: 数据长度
 * @param usl
 * @param lsl
 * @return CP数值
 */
double cp(double *data, size_t length, double usl, double lsl) {
    double Cp = 0.0;
    if (data == NULL) {
        return Cp;
    }
    if (length <= 0) {
        return Cp;
    }
    if (usl <= 0.0 || lsl <= 0.0) {
        return Cp;
    }

    CALC_RET ret;
    ST_RET stRet;
    stRet = CalcStandardDeviation(data, length, 1, 0.0, &ret);

    double sigma = ret.data;

    sigma = ret.data;
    Cp = (usl - lsl) / 6 * sigma;

    return Cp;
}

/*!
 *
 * @param data: 一维原始数据
 * @param length: 数据长度
 * @param usl
 * @param lsl
 * @return CR数值
 */
double cr(double *data, size_t length, double usl, double lsl) {
    double Cr = 0.0;
    double Cp = cp(data, length, usl, lsl);
    if (Cp <= 0.0) {
        return Cr;
    }
    Cr = 1 / Cp;
    return Cr;
}



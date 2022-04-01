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


/*! *@description
 * 均值极差图。
 * 连续每n的数据的均值的变化图
 * 可以直观反应设备的稳定性，以及性能的实时变化。
 */
//todo 考虑传入一个一维数组data,在函数内部进行拆分成二维数组
ST_RET xbarSbar(double **data, size_t n_col, size_t n_row, SPC_RET **spc_ret) {
    if (n_col < 2 || n_col > 10) {
        return ERROR_DATA_SIZE;
    }

    if (NULL == *spc_ret) {
        return ERROR_NULL_PTR;
    }

    if (NULL == data) {
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
    memset(spc_ret, 0, sizeof(SPC_RET));
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

/*! *@description
 * cpk:制程能力指数，制程水准的量化反应，实质作用是反应制程合格率的高低。
 * usl:即规格上限。指的是控制目标参数的最大合格上限值
 * lsl:即规格下限。指的是控制目标参数的最小合格下限值
 * ps: cpk的计算方式为样本均值与规格上下限的距离最小值，与样本方差的比值。
 * cpk越小，代表样本约接近上限或者下限，并且离散程度越高。
 * cpk越大，代表样本约接近上限下限之间的中线，并且离散程度越低。
 * 因此 cpk越大代表设备加工能力越优且稳定
 */
double cpk(double *data, size_t length, double usl, double lsl) {
    double Cpk = 0.0;
    if (NULL == data) {
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
    double mean = ret.data;

    stRet = CalcStandardDeviation(data, length, 1, mean, &ret);

    double sigma = ret.data;

    double Cpu = (usl - mean) / (3 * sigma);
    double Cpl = (mean - lsl) / (3 * sigma);
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

/*! *@description
 * cmk的作用与上述cpk的作用类似。
 * 由于仅考虑设备本身的影响，因此在采样时对其他因素要严加控制，尽量避免其他因素的干扰。
 * 由于仅考虑设备本身的影响，因此在采样时对其他因素要严加控制，尽量避免其他因素的干扰，计算公式与Ppk相同，只是取样不同。
 */

double cmk(double *data, size_t length, double usl, double lsl) {
    double Cmk = 0.0;
    if (NULL == data) {
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


/*! *@description
 * cp又称为过程能力；他只统计稳定的过程。
 * 是过程在受控状态下的实际加工能力，不考虑过程的中心是否有偏移。
 * cp值越大过程的离散程度越小，加工能力越稳定。但它无法体现过程的中心是否偏移。
*/
double cp(double *data, size_t length, double usl, double lsl) {
    double Cp = 0.0;
    if (NULL == data) {
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

/*! *@description
 * cr是cp的倒数。
 * cr值越大过程的离散程度越大，加工能力越不稳定
 * */

double cr(double *data, size_t length, double usl, double lsl) {
    double Cr = 0.0;
    double Cp = cp(data, length, usl, lsl);
    if (Cp <= 0.0) {
        return Cr;
    }
    Cr = 1 / Cp;
    return Cr;
}


/*!
 *
 * @param data: 一维原始数据
 * @param length: 数据长度
 * @return 标准差
 */
double calc_std(double *data, size_t length) {
    double Std = 0.0;
    if (NULL == data) {
        return Std;
    }
    if (length <= 0) {
        return Std;
    }
    ST_RET stRet;
    CALC_RET ret;

    stRet = CalcMean(data, length, &ret);
    double mean = ret.data;
    stRet = CalcStandardDeviation(data, length, 1, mean, &ret);

    double sigma = ret.data;

    return sigma;
}
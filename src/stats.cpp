//
// Created by 华为 on 2022/3/14.
//

#include "stats.h"
#include "error.h"
#include "strings.h"
#include "NumCpp.hpp"

using namespace nc;

/*! *@description
 * 求样本的标准差
 */
ST_RET CalcStandardDeviation(double *data, size_t length, int ddof, double mean, CALC_RET *ret) {
    float sum = 0.0, sDeviation = 0.0;
    int i, sRet = ERROR_NO_ERROR;

    if (data == NULL) {
        return ERROR_NULL_PTR;
    }

    if (length <= 0) {
        return ERROR_DATA_SIZE;
    }

    if (ret == NULL) {
        return ERROR_NULL_PTR;
    }

    memset(ret, 0, sizeof(CALC_RET));
    ret->ret = sRet;

    if (length <= ddof) {
        sRet = ERROR_DATA_SIZE;
        return sRet;
    }

    if (mean == 0.0) {
        // 需要重新计算
        for (i = 0; i < length; i++) {
            sum += data[i];
        }
        // Calculating mean
        mean = sum / length;
    }


    for (i = 0; i < length; ++i) {
        sDeviation += pow(data[i] - mean, 2);
    }

    ret->data = nc::round(sqrt(sDeviation / length - ddof), 2);
    return sRet;
}

/*! *@description
 * 求样本的平均值
 */
ST_RET CalcAvg(double *data, size_t length, CALC_RET *ret) {
    float sum = 0.0;
    ST_RET sRet = ERROR_NO_ERROR;
    if (data == NULL) {
        return ERROR_NULL_PTR;
    }

    if (length <= 0) {
        return ERROR_DATA_SIZE;
    }

    if (ret == NULL) {
        return ERROR_NULL_PTR;
    }

    memset(ret, 0, sizeof(CALC_RET));
    ret->ret = sRet;

    for (int i = 0; i < length; i++) {
        sum += data[i];
    }
    // Calculating Avg
    ret->data = nc::round(sum / length, 2);

    return ERROR_NO_ERROR;
}

/*! *@description
 * 求样本的平均值，作用同CalcAvg
 */
ST_RET CalcMean(double *data, size_t length, CALC_RET *ret) {
    double sum = 0.0;
    ST_RET sRet = ERROR_NO_ERROR;
    if (data == NULL) {
        return ERROR_NULL_PTR;
    }

    if (length <= 0) {
        return ERROR_DATA_SIZE;
    }

    if (ret == NULL) {
        return ERROR_NULL_PTR;
    }

    memset(ret, 0, sizeof(CALC_RET));
    ret->ret = sRet;

    for (int i = 0; i < length; i++) {
        sum += data[i];
    }
    // Calculating mean
    ret->data = nc::round(sum / length, 2);

    return ERROR_NO_ERROR;
}

/*! *@description
 * 直方图数据 y轴为样本在各个范围内的比例，x轴为样本的统计范围
 */
ST_RET CalcHistogram(double *data, size_t length, float usl, float lsl, int step, bool density, PLOT_RET **ret) {
    ST_RET sRet = ERROR_NO_ERROR;
    if (data == NULL) {
        return ERROR_NULL_PTR;
    }
    if (usl <= 0 || lsl <= 0) {
        return ERROR_OVER_FLOW;
    }

    if (usl <= lsl) {
        return ERROR_OVER_FLOW;
    }

    if (*ret == NULL) {
        return ERROR_NULL_PTR;
    }

    memset(ret, 0, sizeof(PLOT_RET));

    CALC_RET dRet;

    sRet = CalcMean(data, length, &dRet);
    double mean = dRet.data;

    sRet = CalcStandardDeviation(data, length, 1, mean, &dRet);

    double sigma = dRet.data;

    double data_min = mean - 3 * sigma;
    double data_max = mean + 3 * sigma;
    double fStep = (double) step;
    NdArray<double> bins = NdArray<double>();
    if (usl && lsl) {
        bins = nc::arange((double) lsl, usl + fStep, fStep);
    } else {
        bins = nc::arange(data_min - fStep, data_max + fStep, fStep);
    }
    NdArray<double> d = NdArray<double>(data, length);
    NdArray<nc::uint32> y_line = nc::histogram(d, bins);

    int y_size = y_line.size();
    double *y = (double *) calloc(y_size, sizeof(double));

    int eff_length = 0;
    for (int i = 0; i < y_size; ++i) {
        nc::uint32 x = y_line[i];
        if (nc::isnan(x)) {
            y_line[i] = 0; //无效值置位为0
        }
        y[i] = y_line[i] / length;
        eff_length += y_line[i];
    }

    memcpy_plot_ret(ret, bins.data(), bins.size(), y, y_size);

    return ERROR_NO_ERROR;
}

double normFun(double x, double mu, double sigma) {
    double pdf = nc::exp(-(nc::square(x - mu)) / (2 * nc::square(sigma))) / (sigma * nc::sqrt(2 * constants::pi));
    return pdf;
}

/*! *@description
 * 标准正态分布直方图数据 y轴为以样本的均值和方差对应的正态分布在各个范围内的比例，x轴为样本的统计范围
 */
ST_RET CalcNormalDist(double *data, size_t length, float usl, float lsl, int step, bool density, PLOT_RET **ret) {
    ST_RET sRet = ERROR_NO_ERROR;
    if (data == NULL) {
        return ERROR_NULL_PTR;
    }
    if (usl <= 0 || lsl <= 0) {
        return ERROR_OVER_FLOW;
    }

    if (usl <= lsl) {
        return ERROR_OVER_FLOW;
    }

    if (*ret == NULL) {
        return ERROR_NULL_PTR;
    }

    memset(ret, 0, sizeof(PLOT_RET));

    CALC_RET dRet;

    sRet = CalcMean(data, length, &dRet);
    double mean = dRet.data;

    sRet = CalcStandardDeviation(data, length, 1, mean, &dRet);

    double sigma = dRet.data;

    auto shape = nc::Shape(1, 100);

    NdArray<double> normal_data = nc::random::normal(shape, mean, sigma);


    sRet = CalcHistogram(normal_data.data(), normal_data.size(), usl, lsl, step, density, ret);

    PLOT_RET *pRet = *ret;

    if (pRet != NULL) {
        for (int i = 0; i <= pRet->lXData; ++i) {
            double a = pRet->pXData[i];
            double b = pRet->pXData[i + 1];
            double d = normFun((a + b) / 2, mean, sigma) * step;
            if (!density)d = d * 100;
            pRet->pYData[i] = d;
        }
    }

    return ERROR_NO_ERROR;

}

//
// Created by 华为 on 2022/3/14.
//
#include "spc.h"
#include "stats.h"
#include "error.h"
#include "type.h"
#include <gtest/gtest.h>


double mock_data[][10] = {{5., 5.,      10.,     12.,     5.,      5.,      10.,     12.,     5.,      5.},
                          {0,  9.6476,  11.8961, 13.8641, 15.7293, 17.5038, 19.2113, 20.8665, 22.4796, 24.0579},
                          {0,  10.0830, 12.3505, 14.3359, 16.2170, 18.0063, 19.7276, 21.3960, 23.0217, 24.6119},
                          {0,  10.3114, 12.5845, 14.5760, 16.4629, 18.2578, 19.9845, 21.6581, 23.2887, 24.8838},
                          {0,  10.4405, 12.7143, 14.7077, 16.5965, 18.3935, 20.1223, 21.7980, 23.4307, 25.0278},
                          {0,  10.5581, 12.8297, 14.8234, 16.7127, 18.5105, 20.2403, 21.9171, 23.5510, 25.1493},
                          {0,  10.5152, 12.7880, 14.7818, 16.6711, 18.4687, 20.1982, 21.8747, 23.5082, 25.1062},
                          {0,  10.5816, 12.8524, 14.8460, 16.7352, 18.5331, 20.2631, 21.9401, 23.5742, 25.1728},
                          {0,  10.5932, 12.8635, 14.8570, 16.7463, 18.5442, 20.2743, 21.9515, 23.5858, 25.1846}};

// 求平均值测试.
TEST(SPCTest, TestMean) {
    CALC_RET ret;
    printf("test\n");
    int r = CalcMean(mock_data[0], 10, &ret);
    printf("计算出十个数据[5.,5.,10.,12.,5.,5.,10.,12.,5.,5.]的平均值为：%f", ret.data);
    EXPECT_EQ(r, ERROR_NO_ERROR);
}
//求标准差测试
TEST(SPCTest, TestStandardDeviation) {
    CALC_RET ret;
    int r = CalcStandardDeviation(mock_data[0], 10, 1, 0.0, &ret);
    printf("计算出十个数据[5.,5.,10.,12.,5.,5.,10.,12.,5.,5.]的标准差为：%f", ret.data);
    EXPECT_EQ(r, ERROR_NO_ERROR);
}

//求标准正态分布函数输出
//python下函数求出值 0.398942280401
TEST(SPCTest, TestnormFun) {
    double d = normFun(5, 5, 1);
    printf("测试一下标准正态分布函数输出：%f", d);
    EXPECT_EQ(d, 0.3989422804014327);
}

//求cpk函数输出
//python下函数求出值0.376941059635
//与python产生差异的原因在于标准差计算时的公式区别(分母为n还是n-1的区别)
TEST(SPCTest, Testcpk) {
    double d = cpk(mock_data[0], 10, 14, 4);
    printf("测试一下cpk函数输出：%f", d);
    EXPECT_EQ(d, 0.35751839307786898);
}

//求直方图函数输出
//python下函数histogram(b, 14, 4, 1)求出值
//(array([ 4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14]),
// array([0. , 0.6, 0. , 0. , 0. , 0. , 0.2, 0. , 0.2, 0. ]), 10)
TEST(SPCTest, TestHistogram) {
    PLOT_RET *ret = NULL;
    int r = CalcHistogram(mock_data[0], 10, 14, 4, 1, 1, &ret);
    printf("测试一下直方图函数输出：\n");
    PrintArray(ret->pXData, ret->lXData);
    PrintArray(ret->pYData, ret->lYData);
    free_plot_ret(&ret);
    EXPECT_EQ(r, ERROR_NO_ERROR);
}

//求正态分布直方图函数输出
//(array([ 4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14]),
//        array([0.08333197, 0.10867035, 0.12687296, 0.13261286, 0.12409686,
//0.10396676, 0.07798064, 0.05236459, 0.0314809 , 0.01694397]))
TEST(SPCTest, TestNormalDist) {
    PLOT_RET *ret = NULL;
    int r = CalcNormalDist(mock_data[0], 10, 14, 4, 1, 1, &ret);
    printf("测试一下正态分布直方图函数输出：\n");
    PrintArray(ret->pXData, ret->lXData);
    PrintArray(ret->pYData, ret->lYData);
    free_plot_ret(&ret);
    EXPECT_EQ(r, ERROR_NO_ERROR);
}

//求xbar_rbar函数输出
//A = covert2dArray([5., 5., 10., 12., 5., 5., 10., 12., 5., 5.], 2)
//B = xbar_rbar(A, 2)
//{'upper': 8.9, 'lower': 5.9, 'data': [5.0, 11.0, 5.0, 11.0, 5.0], 'center': 7.4}
TEST(SPCTest, TestxbarSbar) {
    SPC_RET *ret;
    double A[][2] = {{5.,  5.},
                     {10., 12.},
                     {5.,  5.},
                     {10., 12.},
                     {5.,  5.}};
    double *data[5] = {NULL};
    for (int i = 0; i < 5; ++i) {
        data[i] = A[i];
    }
    int r = xbarSbar(data, 2, 5, &ret);
    printf("测试一下barSbar函数输出：\n");
    PrintArray(ret->pData, 5);
    printf("upper:%f,lower:%f,center:%f", ret->lower, ret->upper, ret->center);
    free_spc_ret(&ret);
    EXPECT_EQ(r, ERROR_NO_ERROR);
}
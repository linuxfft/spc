//
// Created by 华为 on 2022/3/15.
//

#include "type.h"
#include <math.h>
#include "string.h"
#include "memory.h"
#include "error.h"


ST_RET realloc_mem(double **ptr, size_t size) {
    double *p = (double *) calloc(size, sizeof(double));
    if (*ptr != NULL) {
        free(*ptr);
    }
    *ptr = p;
    return ERROR_NO_ERROR;
}


SPC_RET *create_spc_ret(size_t length) {
    if (length <= 0) return NULL;
    SPC_RET *ret = (SPC_RET *) calloc(1, sizeof(SPC_RET));
    ret->pData = (double *) calloc(length, sizeof(double));
    ret->lData = length;
    return ret;
}

ST_RET memcpy_spc_ret(SPC_RET **ret, const double *p, size_t size) {
    SPC_RET *pRet = *ret;
    if (pRet == NULL) {
        //如果为空的话创建
        pRet = create_spc_ret(size);
    }
    if (pRet->lData < size) {
        realloc_mem(&(pRet->pData), size);
    }

    memcpy(pRet->pData, p, size * sizeof(double));
    pRet->lData = size;
    *ret = pRet;
    return ERROR_NO_ERROR;
}

void free_spc_ret(SPC_RET **ret) {
    SPC_RET *p = *ret;
    if (p == NULL) return;
    if (NULL == p->pData) return;
    free(p->pData);
    p->pData = NULL;
    p->lData = 0;
    free(p);
    p = NULL;
}

PLOT_RET *create_plot_ret(size_t x_size, size_t y_size) {
    if (x_size <= 0 || y_size <= 0) {
        return NULL;
    }
    PLOT_RET *ret = (PLOT_RET *) calloc(1, sizeof(PLOT_RET));
    ret->pXData = (double *) calloc(x_size, sizeof(double));
    ret->lXData = x_size;
    ret->pYData = (double *) calloc(y_size, sizeof(double));
    ret->lYData = y_size;
    return ret;
}


ST_RET memcpy_plot_ret(PLOT_RET **ret, const double *pX, size_t lX, const double *pY, size_t lY) {
    PLOT_RET *pRet = *ret;
    if (pRet == NULL) {
        //如果为空的话创建
        pRet = create_plot_ret(lX, lY);
    }
    // ret已经被初始化
    if (pRet->lXData < lX) {
        realloc_mem(&(pRet->pXData), lX);
    }
    if (pRet->lYData < lY) {
        realloc_mem(&(pRet->pYData), lY);
    }

    memcpy(pRet->pXData, pX, lX * sizeof(double));
    pRet->lXData = lX;

    memcpy(pRet->pYData, pY, lY * sizeof(double));
    pRet->lYData = lY;
    *ret = pRet;
    return ERROR_NO_ERROR;
}


void free_plot_ret(PLOT_RET **ret) {
    PLOT_RET *p = *ret;
    if (p == NULL) return;
    if (NULL == p->pXData || NULL == p->pYData) return;
    free(p->pXData);
    free(p->pYData);
    p->pXData = NULL;
    p->pYData = NULL;
    p->lXData = 0;
    p->lYData = 0;
    free(p);
    p = NULL;
}


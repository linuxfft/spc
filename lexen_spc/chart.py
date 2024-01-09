# -*- coding: utf-8 -*-
from __future__ import division  # 精确除以
import numpy as np
from .table import A2, A3, D3, D4, B3, B4, d2
from typing import List, Optional, Dict, Any, Union
import logging

_logger = logging.getLogger(__name__)


def covert2dArray(data, size):
    # type: (List[float], int) ->  Optional[np.ndarray]
    b = len(data) % size
    if b != 0:
        _logger.error(u"数据长度不正确!!! 自动截取")
    l = int(len(data) / size)
    offset = int(len(data) - b)
    ret = np.array(data[:offset]).reshape(l, size)
    return ret


def xbar(data, size):
    X = []
    array_2d_data = covert2dArray(data, size)
    for xs in array_2d_data:
        assert len(xs) == size
        X.append(round(np.mean(xs), 2))
    xbar = np.mean(X)
    sigma = np.std(data, ddof=1)
    lclx = xbar - 3 * sigma
    uclx = xbar + 3 * sigma
    ret = {
        "data": X,
        "center": round(xbar, 2),
        "lower": round(lclx, 2),
        "upper": round(uclx, 2),
    }
    return ret


def xbar_sbar(data, size, newdata):
    # type: (np.ndarray, int, Any) ->  Optional[Dict]
    assert size >= 2
    assert size <= 10
    newvalues = None

    assert data.ndim == 2

    X, S = [], []
    for xs in data:
        assert len(xs) == size
        S.append(np.std(xs, ddof=1))
        X.append(round(np.mean(xs), 2))

    if newdata:
        newvalues = [np.mean(xs) for xs in newdata]

    sbar = np.mean(S)
    xbar = np.mean(X)

    lclx = xbar - A3[size] * sbar
    uclx = xbar + A3[size] * sbar

    ret = {
        "data": X,
        "center": round(xbar, 2),
        "lower": round(lclx, 2),
        "upper": round(uclx, 2),
    }

    return ret


def xbar_rbar(data, size, newdata=None):
    # type: (np.ndarray, int, Any) ->  Optional[Dict]
    """

    :rtype: 元组，数据，中线，下线，上线
    """
    assert size >= 2
    assert size <= 10
    assert data.ndim == 2

    newvalues = None

    R, X = [], []  # values
    for xs in data:
        assert len(xs) == size
        R.append(max(xs) - min(xs))
        X.append(round(np.mean(xs), 2))

    if newdata:
        newvalues = [np.mean(xs) for xs in newdata]

    Rbar = np.mean(R)  # center
    Xbar = np.mean(X)

    lcl = Xbar - A2[size] * Rbar
    ucl = Xbar + A2[size] * Rbar
    if np.isnan(Xbar):
        Xbar = 0
    if np.isnan(lcl):
        lcl = 0
    if np.isnan(ucl):
        ucl = 0
    ret = {
        "data": X,
        "center": round(Xbar, 2),
        "lower": round(lcl, 2),
        "upper": round(ucl, 2),
    }

    return ret


def rbar(data, size, newdata=None):
    # type: (np.ndarray, int, Any) ->  Optional[Dict]
    """

    :rtype: 元组，数据，中线，下线，上线
    """
    assert size >= 2
    assert size <= 10
    assert data.ndim == 2

    newvalues = None

    R = []  # values
    for xs in data:
        assert len(xs) == size
        R.append(round(max(xs) - min(xs), 2))

    if newdata:
        newvalues = [max(xs) - min(xs) for xs in newdata]

    Rbar = np.mean(R)  # center

    lcl = D3[size] * Rbar
    ucl = D4[size] * Rbar

    ret = {
        "data": R,
        "center": round(Rbar, 2),
        "lower": round(lcl, 2),
        "upper": round(ucl, 2),
    }

    return ret


def u(data, size, newdata):
    # type: (np.ndarray, int, Any) ->  Optional[Dict]
    """

    SPC U-charts
    :param data:
    :param size:
    :param newdata:
    :return:
    """
    sizes, data = data.T
    if size == 1:
        sizes, data = data, sizes

    data2 = sizes / data
    ubar = np.sum(sizes) / np.sum(data)

    lcl, ucl = [], []
    for i in data:
        lcl.append(ubar - 3 * np.sqrt(ubar / i))
        ucl.append(ubar + 3 * np.sqrt(ubar / i))

    ret = {
        "data": round(data2, 2),
        "center": round(ubar, 2),
        "lower": round(lcl, 2),
        "upper": round(ucl, 2),
    }

    return ret


def np_chart(data, size, newdata):
    # type: (np.ndarray, int, Any) ->  Optional[Dict]

    sizes, data = data.T
    if size == 1:
        sizes, data = data, sizes

    # the samples must have the same size for this charts
    assert np.mean(sizes) == sizes[0]

    p = np.mean([float(d) / sizes[0] for d in data])
    pbar = np.mean(data)

    lcl = pbar - 3 * np.sqrt(pbar * (1 - p))
    ucl = pbar + 3 * np.sqrt(pbar * (1 - p))

    ret = {
        "data": data,
        "center": round(pbar, 2),
        "lower": round(lcl, 2),
        "upper": round(ucl, 2),
    }

    return ret


def sbar(data, size, newdata):
    # type: (np.ndarray, int, Any) ->  Optional[Dict]
    """

    :rtype: 元组，数据，中线，下线，上线
    """
    assert size >= 2
    assert size <= 10
    assert data.ndim == 2
    newvalues = None

    S = []
    for xs in data:
        assert len(xs) == size
        S.append(round(np.std(xs, ddof=1), 2))

    if newdata:
        newvalues = [np.std(xs, ddof=1) for xs in newdata]

    sbar = np.mean(S)

    lcls = B3[size] * sbar
    ucls = B4[size] * sbar

    ret = {
        "data": S,
        "center": round(sbar, 2),
        "lower": round(lcls, 2),
        "upper": round(ucls, 2),
    }

    return ret


def cpk(data, usl, lsl):
    # type: (List[float], float, float) -> Optional[float]
    if usl is None or lsl is None:
        return None
    if not data:
        return None
    sigma = np.std(data)
    m = np.mean(data)
    if usl < m or m < lsl:
        return None
    Cpu = float(usl - m) / (3 * sigma)
    Cpl = float(m - lsl) / (3 * sigma)
    Cpk = np.min([Cpu, Cpl])
    return round(Cpk, 2)


def cmk(data, usl, lsl):
    # type: (List[float], float, float) -> Optional[float]
    if usl is None or lsl is None:
        return None
    if not data:
        return None
    avg = np.average(data)
    sigma = np.std(data)
    if usl < avg or lsl > avg:
        return None
    a = (usl - avg) / (3 * sigma)
    b = (avg - lsl) / (3 * sigma)
    cmk = np.min([a, b])
    return round(cmk, 2)


def cr(data, usl, lsl):
    # type: (List[float], float, float) -> Optional[float]
    if usl is None or lsl is None:
        return None
    if not data:
        return None
    sigma = np.std(data)
    ret = 6 * sigma / (usl - lsl)
    return round(ret, 2)


def pci(data: Union[list, np.array], ret_type: str = "pp", sigma: Optional[float] = None,
        usl: Optional[float] = None, lsl: Optional[float] = None) -> dict:
    ret = {}
    if len(data) <= 1:
        return ret
    m = np.mean(data)
    if not sigma:
        sigma = np.std(data)
    if usl is None or lsl is None:
        return ret
    Pp = round((usl - lsl) / (6 * sigma), 2)
    Ppu = round(float(usl - m) / (3 * sigma), 2)
    Ppl = round(float(m - lsl) / (3 * sigma), 2)
    Ppk = np.min([Ppu, Ppl])
    ret = {
        f"{ret_type}": Pp,
        f"{ret_type}l": Ppl,
        f"{ret_type}u": Ppu,
        f"{ret_type}k": Ppk,
    }
    return ret


def cp_sigma(data: np.array, size: int = 1) -> float:
    if len(data) <= 1 or size < 1:
        return 0.0
    if size == 1:
        # 计算MR
        MR = [abs(data[i + 1] - data[i]) for i in range(len(data) - 1)]
        sigma = np.mean(MR) / d2[2]
    else:
        R = [round(max(xs) - min(xs), 2) for xs in data]
        sigma = np.mean(R) / d2[size]
    return sigma

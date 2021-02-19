# -*- coding: utf-8 -*-
from __future__ import division  # 精确除以
import logging
from typing import List, Optional, Union, Set
import numpy as np
import sys
major_version = sys.version_info.major

_logger = logging.getLogger(__name__)


def normal(data, usl, lsl, step=1, density=True):
    # type: (List[float], float, float, int, bool) -> tuple
    if usl < lsl:
        _logger.error("histogram usl Must Be Greater Than lsl")
        return None, None
    if not isinstance(step, int):
        _logger.error("histogram Step Must Be Int Type")
        return None, None
    if not data:
        _logger.error("histogram Data Must Be Int Type")
        return None, None
    sigma = np.std(data)
    mean = np.mean(data)
    d = np.random.normal(mean, sigma, 100)
    dd = d.tolist()
    x_line, y_line = histogram(
        dd, usl=usl, lsl=lsl, step=step, density=density)
    return x_line, y_line


def histogram(data, usl, lsl, step=1, density=True):
    # type: (Union[List[float], object], float, float, int, bool) -> tuple
    if usl < lsl:
        _logger.error("histogram usl Must Be Greater Than lsl")
        return None, None
    if not isinstance(step, int):
        _logger.error("histogram Step Must Be Int Type")
        return None, None
    if not data:
        _logger.error("histogram Data Must Be Int Type")
        return None, None
    sigma = np.std(data)
    mean = np.mean(data)
    data_min = mean - 3 * sigma
    data_max = mean + 3 * sigma
    if usl and lsl:
        bins = np.arange(lsl, usl, step)
    else:
        bins = np.arange(data_min - step, data_max + step, step)
    y_line, x_line = np.histogram(data, bins, density=density)
    return x_line, y_line

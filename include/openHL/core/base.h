#pragma once

#include "openHL/core/hldef.h"
#include "openHL/core/hlstd.h"
#include <assert.h>

typedef enum
{
    NORM_INF,
    NORM_L1,
    NORM_L2,
    NORM_MINMAX
} NormTypes;
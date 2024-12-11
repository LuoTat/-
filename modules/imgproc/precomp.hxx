#pragma once

#include "openHL/imgproc.hxx"
#include "openHL/core/utility.hxx"

#include "openHL/imgproc/imgproc_c.h"
#include "openHL/core/private.hxx"
#include "openHL/imgproc/hal/hal.hxx"


#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>

#include "filterengine.hxx"


extern const uchar icvSaturate8u_cv[];
#define HL_FAST_CAST_8U(t)   ((-256 <= (t) && (t) <= 512) ? icvSaturate8u_cv[(t) + 256] : 0)
#define HL_CALC_MIN_8U(a, b) (a) -= HL_FAST_CAST_8U((a) - (b))
#define HL_CALC_MAX_8U(a, b) (a) += HL_FAST_CAST_8U((b) - (a))
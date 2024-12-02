#pragma once

#include "openHL/core/types_c.h"

#define HL_AUTOSTEP    0x7fffffff

#define HL_C           1
#define HL_L1          2
#define HL_L2          4
#define HL_NORM_MASK   7
#define HL_RELATIVE    8
#define HL_DIFF        16
#define HL_MINMAX      32

#define HL_DIFF_C      (HL_DIFF | HL_C)
#define HL_DIFF_L1     (HL_DIFF | HL_L1)
#define HL_DIFF_L2     (HL_DIFF | HL_L2)
#define HL_RELATIVE_C  (HL_RELATIVE | HL_C)
#define HL_RELATIVE_L1 (HL_RELATIVE | HL_L1)
#define HL_RELATIVE_L2 (HL_RELATIVE | HL_L2)

extern "C" const char* hlErrorStr(int status);
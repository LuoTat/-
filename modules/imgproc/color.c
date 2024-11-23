#include "precomp.h"
#include "color.h"

#include <stdio.h>

void cvtColor(const Mat* src, Mat* dst, int code)
{
    assert(!empty(src));

    switch (code)
    {
        case COLOR_BGR2GRAY :
            cvtColorBGR2Gray(src, dst);
            break;
        default :
            break;
    }
}
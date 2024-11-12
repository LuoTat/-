#include "color.h"

void cvtColor(const Mat* src, Mat* dst, enum ColorConversionCodes code)
{
    switch (code)
    {
        case COLOR_BGR2GRAY :
            cvtColorBGR2Gray(src, dst);
            break;
        case COLOR_BGR2GRAY_INVERTED :
            cvtColorBGR2Gray_Inverted(src, dst);
            break;
        default :
            break;
    }
}
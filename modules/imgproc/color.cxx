#include "precomp.hxx"
#include "color.hxx"

namespace hl
{

void cvtColor(const Mat& _src, Mat& _dst, int code, int dcn)
{
    HL_Assert(!_src.empty());

    if (dcn <= 0)
        dcn = dstChannels(code);

    switch (code)
    {
        case COLOR_BGR2GRAY :
        case COLOR_RGB2GRAY :
        case COLOR_BGRA2GRAY :
        case COLOR_RGBA2GRAY :
            cvtColorBGR2Gray(_src, _dst, swapBlue(code));
            break;
        default :
            break;
    }
}

}    // namespace hl
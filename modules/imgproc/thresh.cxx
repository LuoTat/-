#include "precomp.hxx"

namespace hl
{

template <typename T>
inline static T threshBinary(const T& src, const T& thresh, const T& maxval)
{
    return src > thresh ? maxval : 0;
}

template <typename T>
inline static T threshBinaryInv(const T& src, const T& thresh, const T& maxval)
{
    return src <= thresh ? maxval : 0;
}

template <typename T>
inline static T threshTrunc(const T& src, const T& thresh)
{
    return std::min(src, thresh);
}

template <typename T>
inline static T threshToZero(const T& src, const T& thresh)
{
    return src > thresh ? src : 0;
}

template <typename T>
inline static T threshToZeroInv(const T& src, const T& thresh)
{
    return src <= thresh ? src : 0;
}

template <typename T>
static void threshGeneric(Size roi, const T* src, size_t src_step, T* dst, size_t dst_step, T thresh, T maxval, int type)
{
    int i = 0, j;
    switch (type)
    {
        case THRESH_BINARY :
            for (; i < roi.height; i++, src += src_step, dst += dst_step)
                for (j = 0; j < roi.width; j++)
                    dst[j] = threshBinary<T>(src[j], thresh, maxval);
            return;

        case THRESH_BINARY_INV :
            for (; i < roi.height; i++, src += src_step, dst += dst_step)
                for (j = 0; j < roi.width; j++)
                    dst[j] = threshBinaryInv<T>(src[j], thresh, maxval);
            return;

        case THRESH_TRUNC :
            for (; i < roi.height; i++, src += src_step, dst += dst_step)
                for (j = 0; j < roi.width; j++)
                    dst[j] = threshTrunc<T>(src[j], thresh);
            return;

        case THRESH_TOZERO :
            for (; i < roi.height; i++, src += src_step, dst += dst_step)
                for (j = 0; j < roi.width; j++)
                    dst[j] = threshToZero<T>(src[j], thresh);
            return;

        case THRESH_TOZERO_INV :
            for (; i < roi.height; i++, src += src_step, dst += dst_step)
                for (j = 0; j < roi.width; j++)
                    dst[j] = threshToZeroInv<T>(src[j], thresh);
            return;

        default :
            HL_Error(hl::Error::StsBadArg, "");
            return;
    }
}

static void thresh_8u(const Mat& _src, Mat& _dst, uchar thresh, uchar maxval, int type)
{
    Size roi         = _src.size();
    roi.width       *= _src.channels();
    size_t src_step  = _src.step;
    size_t dst_step  = _dst.step;

    if (_src.isContinuous() && _dst.isContinuous())
    {
        roi.width  *= roi.height;
        roi.height  = 1;
        src_step = dst_step = roi.width;
    }

    int          j   = 0;
    const uchar* src = _src.ptr();
    uchar*       dst = _dst.ptr();

    int j_scalar     = j;
    if (j_scalar < roi.width)
    {
        const int thresh_pivot = thresh + 1;
        uchar     tab[256]     = {0};
        switch (type)
        {
            case THRESH_BINARY :
                memset(tab, 0, thresh_pivot);
                if (thresh_pivot < 256)
                {
                    memset(tab + thresh_pivot, maxval, 256 - thresh_pivot);
                }
                break;
            case THRESH_BINARY_INV :
                memset(tab, maxval, thresh_pivot);
                if (thresh_pivot < 256)
                {
                    memset(tab + thresh_pivot, 0, 256 - thresh_pivot);
                }
                break;
            case THRESH_TRUNC :
                for (int i = 0; i <= thresh; i++)
                    tab[i] = (uchar)i;
                if (thresh_pivot < 256)
                {
                    memset(tab + thresh_pivot, thresh, 256 - thresh_pivot);
                }
                break;
            case THRESH_TOZERO :
                memset(tab, 0, thresh_pivot);
                for (int i = thresh_pivot; i < 256; i++)
                    tab[i] = (uchar)i;
                break;
            case THRESH_TOZERO_INV :
                for (int i = 0; i <= thresh; i++)
                    tab[i] = (uchar)i;
                if (thresh_pivot < 256)
                {
                    memset(tab + thresh_pivot, 0, 256 - thresh_pivot);
                }
                break;
        }

        src = _src.ptr();
        dst = _dst.ptr();
        for (int i = 0; i < roi.height; i++, src += src_step, dst += dst_step)
        {
            for (j = j_scalar; j <= roi.width - 4; j += 4)
            {
                uchar t0   = tab[src[j]];
                uchar t1   = tab[src[j + 1]];

                dst[j]     = t0;
                dst[j + 1] = t1;

                t0         = tab[src[j + 2]];
                t1         = tab[src[j + 3]];

                dst[j + 2] = t0;
                dst[j + 3] = t1;
            }
            for (; j < roi.width; j++)
                dst[j] = tab[src[j]];
        }
    }
}

static void thresh_16u(const Mat& _src, Mat& _dst, ushort thresh, ushort maxval, int type)
{
    Size roi         = _src.size();
    roi.width       *= _src.channels();
    size_t src_step  = _src.step / _src.elemSize1();
    size_t dst_step  = _dst.step / _dst.elemSize1();

    if (_src.isContinuous() && _dst.isContinuous())
    {
        roi.width  *= roi.height;
        roi.height  = 1;
        src_step = dst_step = roi.width;
    }

    const ushort* src = _src.ptr<ushort>();
    ushort*       dst = _dst.ptr<ushort>();
    threshGeneric<ushort>(roi, src, src_step, dst, dst_step, thresh, maxval, type);
}

static void thresh_16s(const Mat& _src, Mat& _dst, short thresh, short maxval, int type)
{
    Size roi               = _src.size();
    roi.width             *= _src.channels();
    const short* src       = _src.ptr<short>();
    short*       dst       = _dst.ptr<short>();
    size_t       src_step  = _src.step / sizeof(src[0]);
    size_t       dst_step  = _dst.step / sizeof(dst[0]);

    if (_src.isContinuous() && _dst.isContinuous())
    {
        roi.width  *= roi.height;
        roi.height  = 1;
        src_step = dst_step = roi.width;
    }

    threshGeneric<short>(roi, src, src_step, dst, dst_step, thresh, maxval, type);
}

static void thresh_32f(const Mat& _src, Mat& _dst, float thresh, float maxval, int type)
{
    Size roi               = _src.size();
    roi.width             *= _src.channels();
    const float* src       = _src.ptr<float>();
    float*       dst       = _dst.ptr<float>();
    size_t       src_step  = _src.step / sizeof(src[0]);
    size_t       dst_step  = _dst.step / sizeof(dst[0]);

    if (_src.isContinuous() && _dst.isContinuous())
    {
        roi.width  *= roi.height;
        roi.height  = 1;
    }

    threshGeneric<float>(roi, src, src_step, dst, dst_step, thresh, maxval, type);
}

static void thresh_64f(const Mat& _src, Mat& _dst, double thresh, double maxval, int type)
{
    Size roi                = _src.size();
    roi.width              *= _src.channels();
    const double* src       = _src.ptr<double>();
    double*       dst       = _dst.ptr<double>();
    size_t        src_step  = _src.step / sizeof(src[0]);
    size_t        dst_step  = _dst.step / sizeof(dst[0]);

    if (_src.isContinuous() && _dst.isContinuous())
    {
        roi.width  *= roi.height;
        roi.height  = 1;
    }

    threshGeneric<double>(roi, src, src_step, dst, dst_step, thresh, maxval, type);
}

template <typename T, size_t BinsOnStack = 0u>
static double getThreshVal_Otsu(const Mat& _src, const Size& size)
{
    const int N = std::numeric_limits<T>::max() + 1;
    int       i, j;

    AutoBuffer<int, 4 * BinsOnStack> hBuf(4 * N);

    memset(hBuf.data(), 0, hBuf.size() * sizeof(int));
    int* h             = hBuf.data();
    int* h_unrolled[3] = {h + N, h + 2 * N, h + 3 * N};
    for (i = 0; i < size.height; i++)
    {
        const T* src = _src.ptr<T>(i, 0);
        for (j = 0; j <= size.width - 4; j += 4)
        {
            int v0 = src[j], v1 = src[j + 1];
            h[v0]++;
            h_unrolled[0][v1]++;
            v0 = src[j + 2];
            v1 = src[j + 3];
            h_unrolled[1][v0]++;
            h_unrolled[2][v1]++;
        }
        for (; j < size.width; j++)
            h[src[j]]++;
    }

    double mu = 0, scale = 1. / (size.width * size.height);
    for (i = 0; i < N; i++)
    {
        h[i] += h_unrolled[0][i] + h_unrolled[1][i] + h_unrolled[2][i];
        mu   += i * (double)h[i];
    }

    mu         *= scale;
    double mu1 = 0, q1 = 0;
    double max_sigma = 0, max_val = 0;

    for (i = 0; i < N; i++)
    {
        double p_i, q2, mu2, sigma;

        p_i  = h[i] * scale;
        mu1 *= q1;
        q1  += p_i;
        q2   = 1. - q1;

        if (std::min(q1, q2) < FLT_EPSILON || std::max(q1, q2) > 1. - FLT_EPSILON)
            continue;

        mu1   = (mu1 + i * p_i) / q1;
        mu2   = (mu - q1 * mu1) / q2;
        sigma = q1 * q2 * (mu1 - mu2) * (mu1 - mu2);
        if (sigma > max_sigma)
        {
            max_sigma = sigma;
            max_val   = i;
        }
    }
    return max_val;
}

static double getThreshVal_Otsu_8u(const Mat& _src)
{
    Size size = _src.size();
    int  step = (int)_src.step;
    if (_src.isContinuous())
    {
        size.width  *= size.height;
        size.height  = 1;
        step         = size.width;
    }

    HL_UNUSED(step);
    return getThreshVal_Otsu<uchar, 256u>(_src, size);
}

static double getThreshVal_Otsu_16u(const Mat& _src)
{
    Size size = _src.size();
    if (_src.isContinuous())
    {
        size.width  *= size.height;
        size.height  = 1;
    }

    return getThreshVal_Otsu<ushort>(_src, size);
}

static double getThreshVal_Triangle_8u(const Mat& _src)
{
    Size size = _src.size();
    int  step = (int)_src.step;
    if (_src.isContinuous())
    {
        size.width  *= size.height;
        size.height  = 1;
        step         = size.width;
    }

    const int N = 256;
    int       i, j, h[N] = {0};
    int       h_unrolled[3][N] = {};
    for (i = 0; i < size.height; i++)
    {
        const uchar* src = _src.ptr() + step * i;
        for (j = 0; j <= size.width - 4; j += 4)
        {
            int v0 = src[j], v1 = src[j + 1];
            h[v0]++;
            h_unrolled[0][v1]++;
            v0 = src[j + 2];
            v1 = src[j + 3];
            h_unrolled[1][v0]++;
            h_unrolled[2][v1]++;
        }
        for (; j < size.width; j++)
            h[src[j]]++;
    }

    int  left_bound = 0, right_bound = 0, max_ind = 0, max = 0;
    int  temp;
    bool isflipped = false;

    for (i = 0; i < N; i++)
    {
        h[i] += h_unrolled[0][i] + h_unrolled[1][i] + h_unrolled[2][i];
    }

    for (i = 0; i < N; i++)
    {
        if (h[i] > 0)
        {
            left_bound = i;
            break;
        }
    }
    if (left_bound > 0)
        left_bound--;

    for (i = N - 1; i > 0; i--)
    {
        if (h[i] > 0)
        {
            right_bound = i;
            break;
        }
    }
    if (right_bound < N - 1)
        right_bound++;

    for (i = 0; i < N; i++)
    {
        if (h[i] > max)
        {
            max     = h[i];
            max_ind = i;
        }
    }

    if (max_ind - left_bound < right_bound - max_ind)
    {
        isflipped = true;
        i = 0, j = N - 1;
        while (i < j)
        {
            temp = h[i];
            h[i] = h[j];
            h[j] = temp;
            i++;
            j--;
        }
        left_bound = N - 1 - right_bound;
        max_ind    = N - 1 - max_ind;
    }

    double thresh = left_bound;
    double a, b, dist = 0, tempdist;

    /*
     * We do not need to compute precise distance here. Distance is maximized, so some constants can
     * be omitted. This speeds up a computation a bit.
     */
    a = max;
    b = left_bound - max_ind;
    for (i = left_bound + 1; i <= max_ind; i++)
    {
        tempdist = a * i + b * h[i];
        if (tempdist > dist)
        {
            dist   = tempdist;
            thresh = i;
        }
    }
    thresh--;

    if (isflipped)
        thresh = N - 1 - thresh;

    return thresh;
}

class ThresholdRunner: public ParallelLoopBody
{
public:
    ThresholdRunner(Mat _src, Mat _dst, double _thresh, double _maxval, int _thresholdType)
    {
        src           = _src;
        dst           = _dst;

        thresh        = _thresh;
        maxval        = _maxval;
        thresholdType = _thresholdType;
    }

    void operator()(const Range& range) const override
    {
        int row0      = range.start;
        int row1      = range.end;

        Mat srcStripe = src.rowRange(row0, row1);
        Mat dstStripe = dst.rowRange(row0, row1);

        if (srcStripe.depth() == HL_8U)
        {
            thresh_8u(srcStripe, dstStripe, (uchar)thresh, (uchar)maxval, thresholdType);
        }
        else if (srcStripe.depth() == HL_16S)
        {
            thresh_16s(srcStripe, dstStripe, (short)thresh, (short)maxval, thresholdType);
        }
        else if (srcStripe.depth() == HL_16U)
        {
            thresh_16u(srcStripe, dstStripe, (ushort)thresh, (ushort)maxval, thresholdType);
        }
        else if (srcStripe.depth() == HL_32F)
        {
            thresh_32f(srcStripe, dstStripe, (float)thresh, (float)maxval, thresholdType);
        }
        else if (srcStripe.depth() == HL_64F)
        {
            thresh_64f(srcStripe, dstStripe, thresh, maxval, thresholdType);
        }
    }

private:
    Mat src;
    Mat dst;

    double thresh;
    double maxval;
    int    thresholdType;
};

}    // namespace hl

double hl::threshold(const Mat& _src, Mat& _dst, double thresh, double maxval, int type)
{
    Mat src = _src;

    _dst.create(src.size(), src.type());
    Mat dst               = _dst;

    int automatic_thresh  = (type & ~hl::THRESH_MASK);
    type                 &= THRESH_MASK;

    HL_Assert(automatic_thresh != (hl::THRESH_OTSU | hl::THRESH_TRIANGLE));
    if (automatic_thresh == hl::THRESH_OTSU)
    {
        int src_type = src.type();
        HL_CheckType(src_type == HL_8UC1 || src_type == HL_16UC1, "THRESH_OTSU mode");

        thresh = src.type() == HL_8UC1 ? getThreshVal_Otsu_8u(src) : getThreshVal_Otsu_16u(src);
    }
    else if (automatic_thresh == hl::THRESH_TRIANGLE)
    {
        HL_Assert(src.type() == HL_8UC1);
        thresh = getThreshVal_Triangle_8u(src);
    }

    if (src.depth() == HL_8U)
    {
        int ithresh = hlFloor(thresh);
        thresh      = ithresh;
        int imaxval = hlRound(maxval);
        if (type == THRESH_TRUNC)
            imaxval = ithresh;
        imaxval = saturate_cast<uchar>(imaxval);

        if (ithresh < 0 || ithresh >= 255)
        {
            if (type == THRESH_BINARY || type == THRESH_BINARY_INV || ((type == THRESH_TRUNC || type == THRESH_TOZERO_INV) && ithresh < 0) || (type == THRESH_TOZERO && ithresh >= 255))
            {
                int v = type == THRESH_BINARY ? (ithresh >= 255 ? 0 : imaxval) : type == THRESH_BINARY_INV ? (ithresh >= 255 ? imaxval : 0)
                                                                                                           :
                                                                                                           /*type == THRESH_TRUNC ? imaxval :*/ 0;
                dst.setTo(v);
            }
            else
                src.copyTo(dst);
            return thresh;
        }

        thresh = ithresh;
        maxval = imaxval;
    }
    else if (src.depth() == HL_16S)
    {
        int ithresh = hlFloor(thresh);
        thresh      = ithresh;
        int imaxval = hlRound(maxval);
        if (type == THRESH_TRUNC)
            imaxval = ithresh;
        imaxval = saturate_cast<short>(imaxval);

        if (ithresh < SHRT_MIN || ithresh >= SHRT_MAX)
        {
            if (type == THRESH_BINARY || type == THRESH_BINARY_INV || ((type == THRESH_TRUNC || type == THRESH_TOZERO_INV) && ithresh < SHRT_MIN) || (type == THRESH_TOZERO && ithresh >= SHRT_MAX))
            {
                int v = type == THRESH_BINARY ? (ithresh >= SHRT_MAX ? 0 : imaxval) : type == THRESH_BINARY_INV ? (ithresh >= SHRT_MAX ? imaxval : 0)
                                                                                                                :
                                                                                                                /*type == THRESH_TRUNC ? imaxval :*/ 0;
                dst.setTo(v);
            }
            else
                src.copyTo(dst);
            return thresh;
        }
        thresh = ithresh;
        maxval = imaxval;
    }
    else if (src.depth() == HL_16U)
    {
        int ithresh = hlFloor(thresh);
        thresh      = ithresh;
        int imaxval = hlRound(maxval);
        if (type == THRESH_TRUNC)
            imaxval = ithresh;
        imaxval       = saturate_cast<ushort>(imaxval);

        int ushrt_min = 0;
        if (ithresh < ushrt_min || ithresh >= (int)USHRT_MAX)
        {
            if (type == THRESH_BINARY || type == THRESH_BINARY_INV || ((type == THRESH_TRUNC || type == THRESH_TOZERO_INV) && ithresh < ushrt_min) || (type == THRESH_TOZERO && ithresh >= (int)USHRT_MAX))
            {
                int v = type == THRESH_BINARY ? (ithresh >= (int)USHRT_MAX ? 0 : imaxval) : type == THRESH_BINARY_INV ? (ithresh >= (int)USHRT_MAX ? imaxval : 0)
                                                                                                                      :
                                                                                                                      /*type == THRESH_TRUNC ? imaxval :*/ 0;
                dst.setTo(v);
            }
            else
                src.copyTo(dst);
            return thresh;
        }
        thresh = ithresh;
        maxval = imaxval;
    }
    else if (src.depth() == HL_32F)
        ;
    else if (src.depth() == HL_64F)
        ;
    else
        HL_Error(hl::Error::StsUnsupportedFormat, "");

    parallel_for_(Range(0, dst.rows),
                  ThresholdRunner(src, dst, thresh, maxval, type),
                  dst.total() / (double)(1 << 16));
    return thresh;
}

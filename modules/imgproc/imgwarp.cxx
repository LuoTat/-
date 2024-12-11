#include "precomp.hxx"
#include "openHL/core/hal/intrin.hxx"
#include "openHL/core/softfloat.hxx"

using namespace hl;

namespace hl
{

/************** interpolation formulas and tables ***************/

const int INTER_REMAP_COEF_BITS  = 15;
const int INTER_REMAP_COEF_SCALE = 1 << INTER_REMAP_COEF_BITS;

static uchar NNDeltaTab_i[INTER_TAB_SIZE2][2];

static float BilinearTab_f[INTER_TAB_SIZE2][2][2];
static short BilinearTab_i[INTER_TAB_SIZE2][2][2];

static float BicubicTab_f[INTER_TAB_SIZE2][4][4];
static short BicubicTab_i[INTER_TAB_SIZE2][4][4];

static float Lanczos4Tab_f[INTER_TAB_SIZE2][8][8];
static short Lanczos4Tab_i[INTER_TAB_SIZE2][8][8];

inline static void interpolateLinear(float x, float* coeffs)
{
    coeffs[0] = 1.f - x;
    coeffs[1] = x;
}

inline static void interpolateCubic(float x, float* coeffs)
{
    const float A = -0.75f;

    coeffs[0]     = ((A * (x + 1) - 5 * A) * (x + 1) + 8 * A) * (x + 1) - 4 * A;
    coeffs[1]     = ((A + 2) * x - (A + 3)) * x * x + 1;
    coeffs[2]     = ((A + 2) * (1 - x) - (A + 3)) * (1 - x) * (1 - x) + 1;
    coeffs[3]     = 1.f - coeffs[0] - coeffs[1] - coeffs[2];
}

inline static void interpolateLanczos4(float x, float* coeffs)
{
    const static double s45     = 0.70710678118654752440084436210485;
    const static double cs[][2] = {{1, 0}, {-s45, -s45}, {0, 1}, {s45, -s45}, {-1, 0}, {s45, s45}, {0, -1}, {-s45, s45}};

    if (x < FLT_EPSILON)
    {
        for (int i = 0; i < 8; i++)
            coeffs[i] = 0;
        coeffs[3] = 1;
        return;
    }

    float  sum = 0;
    double y0 = -(x + 3) * HL_PI * 0.25, s0 = std::sin(y0), c0 = std::cos(y0);
    for (int i = 0; i < 8; i++)
    {
        double y   = -(x + 3 - i) * HL_PI * 0.25;
        coeffs[i]  = (float)((cs[i][0] * s0 + cs[i][1] * c0) / (y * y));
        sum       += coeffs[i];
    }

    sum = 1.f / sum;
    for (int i = 0; i < 8; i++)
        coeffs[i] *= sum;
}

static void initInterTab1D(int method, float* tab, int tabsz)
{
    float scale = 1.f / tabsz;
    if (method == INTER_LINEAR)
    {
        for (int i = 0; i < tabsz; i++, tab += 2)
            interpolateLinear(i * scale, tab);
    }
    else if (method == INTER_CUBIC)
    {
        for (int i = 0; i < tabsz; i++, tab += 4)
            interpolateCubic(i * scale, tab);
    }
    else if (method == INTER_LANCZOS4)
    {
        for (int i = 0; i < tabsz; i++, tab += 8)
            interpolateLanczos4(i * scale, tab);
    }
    else
        HL_Error(hl::Error::StsBadArg, "Unknown interpolation method");
}

const static void* initInterTab2D(int method, bool fixpt)
{
    static bool inittab[INTER_MAX + 1] = {false};
    float*      tab                    = 0;
    short*      itab                   = 0;
    int         ksize                  = 0;
    if (method == INTER_LINEAR)
        tab = BilinearTab_f[0][0], itab = BilinearTab_i[0][0], ksize = 2;
    else if (method == INTER_CUBIC)
        tab = BicubicTab_f[0][0], itab = BicubicTab_i[0][0], ksize = 4;
    else if (method == INTER_LANCZOS4)
        tab = Lanczos4Tab_f[0][0], itab = Lanczos4Tab_i[0][0], ksize = 8;
    else
        HL_Error(hl::Error::StsBadArg, "Unknown/unsupported interpolation type");

    if (!inittab[method])
    {
        AutoBuffer<float> _tab(8 * INTER_TAB_SIZE);
        int               i, j, k1, k2;
        initInterTab1D(method, _tab.data(), INTER_TAB_SIZE);
        for (i = 0; i < INTER_TAB_SIZE; i++)
            for (j = 0; j < INTER_TAB_SIZE; j++, tab += ksize * ksize, itab += ksize * ksize)
            {
                int isum                                = 0;
                NNDeltaTab_i[i * INTER_TAB_SIZE + j][0] = j < INTER_TAB_SIZE / 2;
                NNDeltaTab_i[i * INTER_TAB_SIZE + j][1] = i < INTER_TAB_SIZE / 2;

                for (k1 = 0; k1 < ksize; k1++)
                {
                    float vy = _tab[i * ksize + k1];
                    for (k2 = 0; k2 < ksize; k2++)
                    {
                        float v               = vy * _tab[j * ksize + k2];
                        tab[k1 * ksize + k2]  = v;
                        isum += itab[k1 * ksize + k2] = saturate_cast<short>(v * INTER_REMAP_COEF_SCALE);
                    }
                }

                if (isum != INTER_REMAP_COEF_SCALE)
                {
                    int diff   = isum - INTER_REMAP_COEF_SCALE;
                    int ksize2 = ksize / 2, Mk1 = ksize2, Mk2 = ksize2, mk1 = ksize2, mk2 = ksize2;
                    for (k1 = ksize2; k1 < ksize2 + 2; k1++)
                        for (k2 = ksize2; k2 < ksize2 + 2; k2++)
                        {
                            if (itab[k1 * ksize + k2] < itab[mk1 * ksize + mk2])
                                mk1 = k1, mk2 = k2;
                            else if (itab[k1 * ksize + k2] > itab[Mk1 * ksize + Mk2])
                                Mk1 = k1, Mk2 = k2;
                        }
                    if (diff < 0)
                        itab[Mk1 * ksize + Mk2] = (short)(itab[Mk1 * ksize + Mk2] - diff);
                    else
                        itab[mk1 * ksize + mk2] = (short)(itab[mk1 * ksize + mk2] - diff);
                }
            }
        tab             -= INTER_TAB_SIZE2 * ksize * ksize;
        itab            -= INTER_TAB_SIZE2 * ksize * ksize;
        inittab[method]  = true;
    }
    return fixpt ? (const void*)itab : (const void*)tab;
}

template <typename ST, typename DT>
struct Cast
{
    typedef ST type1;
    typedef DT rtype;

    DT operator()(ST val) const { return saturate_cast<DT>(val); }
};

template <typename ST, typename DT, int bits>
struct FixedPtCast
{
    typedef ST type1;
    typedef DT rtype;

    enum
    {
        SHIFT = bits,
        DELTA = 1 << (bits - 1)
    };

    DT operator()(ST val) const { return saturate_cast<DT>((val + DELTA) >> SHIFT); }
};

inline static int clip(int x, int a, int b)
{
    return x >= a ? (x < b ? x : b - 1) : a;
}

/****************************************************************************************\
*                       General warping (affine, perspective, remap)                     *
\****************************************************************************************/


template <typename T, bool isRelative>
static void remapNearest(const Mat& _src, Mat& _dst, const Mat& _xy, int borderType, const Scalar& _borderValue, const Point& _offset)
{
    Size      ssize = _src.size(), dsize = _dst.size();
    const int cn = _src.channels();
    const T*  S0 = _src.ptr<T>();
    T         cval[HL_CN_MAX];
    size_t    sstep = _src.step / sizeof(S0[0]);

    for (int k = 0; k < cn; k++)
        cval[k] = saturate_cast<T>(_borderValue[k & 3]);

    unsigned width1 = ssize.width, height1 = ssize.height;

    if (_dst.isContinuous() && _xy.isContinuous() && !isRelative)
    {
        dsize.width  *= dsize.height;
        dsize.height  = 1;
    }

    for (int dy = 0; dy < dsize.height; dy++)
    {
        T*           D     = _dst.ptr<T>(dy);
        const short* XY    = _xy.ptr<short>(dy);
        const int    off_y = isRelative ? (_offset.y + dy) : 0;
        if (cn == 1)
        {
            for (int dx = 0; dx < dsize.width; dx++)
            {
                const int off_x = isRelative ? (_offset.x + dx) : 0;
                int       sx = XY[dx * 2] + off_x, sy = XY[dx * 2 + 1] + off_y;
                if ((unsigned)sx < width1 && (unsigned)sy < height1)
                    D[dx] = S0[sy * sstep + sx];
                else
                {
                    if (borderType == BORDER_REPLICATE)
                    {
                        sx    = clip(sx, 0, ssize.width);
                        sy    = clip(sy, 0, ssize.height);
                        D[dx] = S0[sy * sstep + sx];
                    }
                    else if (borderType == BORDER_CONSTANT)
                        D[dx] = cval[0];
                    else if (borderType != BORDER_TRANSPARENT)
                    {
                        sx    = borderInterpolate(sx, ssize.width, borderType);
                        sy    = borderInterpolate(sy, ssize.height, borderType);
                        D[dx] = S0[sy * sstep + sx];
                    }
                }
            }
        }
        else
        {
            for (int dx = 0; dx < dsize.width; dx++, D += cn)
            {
                const int off_x = isRelative ? (_offset.x + dx) : 0;
                int       sx = XY[dx * 2] + off_x, sy = XY[dx * 2 + 1] + off_y;
                const T*  S;
                if ((unsigned)sx < width1 && (unsigned)sy < height1)
                {
                    if (cn == 3)
                    {
                        S    = S0 + sy * sstep + sx * 3;
                        D[0] = S[0], D[1] = S[1], D[2] = S[2];
                    }
                    else if (cn == 4)
                    {
                        S    = S0 + sy * sstep + sx * 4;
                        D[0] = S[0], D[1] = S[1], D[2] = S[2], D[3] = S[3];
                    }
                    else
                    {
                        S = S0 + sy * sstep + sx * cn;
                        for (int k = 0; k < cn; k++)
                            D[k] = S[k];
                    }
                }
                else if (borderType != BORDER_TRANSPARENT)
                {
                    if (borderType == BORDER_REPLICATE)
                    {
                        sx = clip(sx, 0, ssize.width);
                        sy = clip(sy, 0, ssize.height);
                        S  = S0 + sy * sstep + sx * cn;
                    }
                    else if (borderType == BORDER_CONSTANT)
                        S = &cval[0];
                    else
                    {
                        sx = borderInterpolate(sx, ssize.width, borderType);
                        sy = borderInterpolate(sy, ssize.height, borderType);
                        S  = S0 + sy * sstep + sx * cn;
                    }
                    for (int k = 0; k < cn; k++)
                        D[k] = S[k];
                }
            }
        }
    }
}

template <bool>
struct RemapNoVec
{
    int operator()(const Mat&, void*, const short*, const ushort*, const void*, int, hl::Point&) const { return 0; }
};

template <bool isRelative> using RemapVec_8u = RemapNoVec<isRelative>;

template <class CastOp, class VecOp, typename AT, bool isRelative>
static void remapBilinear(const Mat& _src, Mat& _dst, const Mat& _xy, const Mat& _fxy, const void* _wtab, int borderType, const Scalar& _borderValue, const Point& _offset)
{
    typedef typename CastOp::rtype T;
    typedef typename CastOp::type1 WT;
    Size                           ssize = _src.size(), dsize = _dst.size();
    const int                      cn    = _src.channels();
    const AT*                      wtab  = (const AT*)_wtab;
    const T*                       S0    = _src.ptr<T>();
    size_t                         sstep = _src.step / sizeof(S0[0]);
    T                              cval[HL_CN_MAX];
    CastOp                         castOp;
    VecOp                          vecOp;

    for (int k = 0; k < cn; k++)
        cval[k] = saturate_cast<T>(_borderValue[k & 3]);

    unsigned width1 = std::max(ssize.width - 1, 0), height1 = std::max(ssize.height - 1, 0);
    HL_Assert(!ssize.empty());

    for (int dy = 0; dy < dsize.height; dy++)
    {
        T*            D          = _dst.ptr<T>(dy);
        const short*  XY         = _xy.ptr<short>(dy);
        const ushort* FXY        = _fxy.ptr<ushort>(dy);
        int           X0         = 0;
        bool          prevInlier = false;
        const int     off_y      = (isRelative ? (_offset.y + dy) : 0);
        for (int dx = 0; dx <= dsize.width; dx++)
        {
            bool curInlier = dx < dsize.width ? (unsigned)XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0) < width1 && (unsigned)XY[dx * 2 + 1] + off_y < height1 : !prevInlier;
            if (curInlier == prevInlier)
                continue;

            int X1     = dx;
            dx         = X0;
            X0         = X1;
            prevInlier = curInlier;

            if (!curInlier)
            {
                Point subOffset(_offset.x + dx, _offset.y + dy);
                int   len  = vecOp(_src, D, XY + dx * 2, FXY + dx, wtab, X1 - dx, subOffset);
                D         += len * cn;
                dx        += len;

                if (cn == 1)
                {
                    for (; dx < X1; dx++, D++)
                    {
                        int       sx = XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0), sy = XY[dx * 2 + 1] + off_y;
                        const AT* w = wtab + FXY[dx] * 4;
                        const T*  S = S0 + sy * sstep + sx;
                        *D          = castOp(WT(S[0] * w[0] + S[1] * w[1] + S[sstep] * w[2] + S[sstep + 1] * w[3]));
                    }
                }
                else if (cn == 2)
                    for (; dx < X1; dx++, D += 2)
                    {
                        int       sx = XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0), sy = XY[dx * 2 + 1] + off_y;
                        const AT* w  = wtab + FXY[dx] * 4;
                        const T*  S  = S0 + sy * sstep + sx * 2;
                        WT        t0 = S[0] * w[0] + S[2] * w[1] + S[sstep] * w[2] + S[sstep + 2] * w[3];
                        WT        t1 = S[1] * w[0] + S[3] * w[1] + S[sstep + 1] * w[2] + S[sstep + 3] * w[3];
                        D[0]         = castOp(t0);
                        D[1]         = castOp(t1);
                    }
                else if (cn == 3)
                    for (; dx < X1; dx++, D += 3)
                    {
                        int       sx = XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0), sy = XY[dx * 2 + 1] + off_y;
                        const AT* w  = wtab + FXY[dx] * 4;
                        const T*  S  = S0 + sy * sstep + sx * 3;
                        WT        t0 = S[0] * w[0] + S[3] * w[1] + S[sstep] * w[2] + S[sstep + 3] * w[3];
                        WT        t1 = S[1] * w[0] + S[4] * w[1] + S[sstep + 1] * w[2] + S[sstep + 4] * w[3];
                        WT        t2 = S[2] * w[0] + S[5] * w[1] + S[sstep + 2] * w[2] + S[sstep + 5] * w[3];
                        D[0]         = castOp(t0);
                        D[1]         = castOp(t1);
                        D[2]         = castOp(t2);
                    }
                else if (cn == 4)
                    for (; dx < X1; dx++, D += 4)
                    {
                        int       sx = XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0), sy = XY[dx * 2 + 1] + off_y;
                        const AT* w  = wtab + FXY[dx] * 4;
                        const T*  S  = S0 + sy * sstep + sx * 4;
                        WT        t0 = S[0] * w[0] + S[4] * w[1] + S[sstep] * w[2] + S[sstep + 4] * w[3];
                        WT        t1 = S[1] * w[0] + S[5] * w[1] + S[sstep + 1] * w[2] + S[sstep + 5] * w[3];
                        D[0]         = castOp(t0);
                        D[1]         = castOp(t1);
                        t0           = S[2] * w[0] + S[6] * w[1] + S[sstep + 2] * w[2] + S[sstep + 6] * w[3];
                        t1           = S[3] * w[0] + S[7] * w[1] + S[sstep + 3] * w[2] + S[sstep + 7] * w[3];
                        D[2]         = castOp(t0);
                        D[3]         = castOp(t1);
                    }
                else
                    for (; dx < X1; dx++, D += cn)
                    {
                        int       sx = XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0), sy = XY[dx * 2 + 1] + off_y;
                        const AT* w = wtab + FXY[dx] * 4;
                        const T*  S = S0 + sy * sstep + sx * cn;
                        for (int k = 0; k < cn; k++)
                        {
                            WT t0 = S[k] * w[0] + S[k + cn] * w[1] + S[sstep + k] * w[2] + S[sstep + k + cn] * w[3];
                            D[k]  = castOp(t0);
                        }
                    }
            }
            else
            {
                if (borderType == BORDER_TRANSPARENT)
                {
                    for (; dx < X1; dx++, D += cn)
                    {
                        if (dx >= dsize.width) continue;
                        const int sx = XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0), sy = XY[dx * 2 + 1] + off_y;
                        // If the mapped point is still within bounds, it did not get computed
                        // because it lacked 4 neighbors. Still, it can be computed with an
                        // approximate formula. If it is outside, the point is left untouched.
                        if (sx >= 0 && sx <= ssize.width - 1 && sy >= 0 && sy <= ssize.height - 1)
                        {
                            const AT* w     = wtab + FXY[dx] * 4;
                            WT        w_tot = 0;
                            if (sx >= 0 && sy >= 0) w_tot += w[0];
                            if (sy >= 0 && sx < ssize.width - 1) w_tot += w[1];
                            if (sx >= 0 && sy < ssize.height - 1) w_tot += w[2];
                            if (sx < ssize.width - 1 && sy < ssize.height - 1) w_tot += w[3];
                            if (w_tot == 0.f) continue;
                            const WT w_tot_ini = (WT)w[0] + w[1] + w[2] + w[3];
                            const T* S         = S0 + sy * sstep + sx * cn;
                            for (int k = 0; k < cn; k++)
                            {
                                WT t0 = 0;
                                if (sx >= 0 && sy >= 0) t0 += S[k] * w[0];
                                if (sy >= 0 && sx < ssize.width - 1) t0 += S[k + cn] * w[1];
                                if (sx >= 0 && sy < ssize.height - 1) t0 += S[sstep + k] * w[2];
                                if (sx < ssize.width - 1 && sy < ssize.height - 1) t0 += S[sstep + k + cn] * w[3];
                                t0   = (WT)(t0 * (float)w_tot_ini / w_tot);
                                D[k] = castOp(t0);
                            }
                        }
                    }
                    continue;
                }

                if (cn == 1)
                    for (; dx < X1; dx++, D++)
                    {
                        int sx = XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0), sy = XY[dx * 2 + 1] + off_y;
                        if (borderType == BORDER_CONSTANT && (sx >= ssize.width || sx + 1 < 0 || sy >= ssize.height || sy + 1 < 0))
                        {
                            D[0] = cval[0];
                        }
                        else
                        {
                            int       sx0, sx1, sy0, sy1;
                            T         v0, v1, v2, v3;
                            const AT* w = wtab + FXY[dx] * 4;
                            if (borderType == BORDER_REPLICATE)
                            {
                                sx0 = clip(sx, 0, ssize.width);
                                sx1 = clip(sx + 1, 0, ssize.width);
                                sy0 = clip(sy, 0, ssize.height);
                                sy1 = clip(sy + 1, 0, ssize.height);
                                v0  = S0[sy0 * sstep + sx0];
                                v1  = S0[sy0 * sstep + sx1];
                                v2  = S0[sy1 * sstep + sx0];
                                v3  = S0[sy1 * sstep + sx1];
                            }
                            else
                            {
                                sx0 = borderInterpolate(sx, ssize.width, borderType);
                                sx1 = borderInterpolate(sx + 1, ssize.width, borderType);
                                sy0 = borderInterpolate(sy, ssize.height, borderType);
                                sy1 = borderInterpolate(sy + 1, ssize.height, borderType);
                                v0  = sx0 >= 0 && sy0 >= 0 ? S0[sy0 * sstep + sx0] : cval[0];
                                v1  = sx1 >= 0 && sy0 >= 0 ? S0[sy0 * sstep + sx1] : cval[0];
                                v2  = sx0 >= 0 && sy1 >= 0 ? S0[sy1 * sstep + sx0] : cval[0];
                                v3  = sx1 >= 0 && sy1 >= 0 ? S0[sy1 * sstep + sx1] : cval[0];
                            }
                            D[0] = castOp(WT(v0 * w[0] + v1 * w[1] + v2 * w[2] + v3 * w[3]));
                        }
                    }
                else
                    for (; dx < X1; dx++, D += cn)
                    {
                        int sx = XY[dx * 2] + (isRelative ? (_offset.x + dx) : 0), sy = XY[dx * 2 + 1] + off_y;
                        if (borderType == BORDER_CONSTANT && (sx >= ssize.width || sx + 1 < 0 || sy >= ssize.height || sy + 1 < 0))
                        {
                            for (int k = 0; k < cn; k++)
                                D[k] = cval[k];
                        }
                        else
                        {
                            int       sx0, sx1, sy0, sy1;
                            const T * v0, *v1, *v2, *v3;
                            const AT* w = wtab + FXY[dx] * 4;
                            if (borderType == BORDER_REPLICATE)
                            {
                                sx0 = clip(sx, 0, ssize.width);
                                sx1 = clip(sx + 1, 0, ssize.width);
                                sy0 = clip(sy, 0, ssize.height);
                                sy1 = clip(sy + 1, 0, ssize.height);
                                v0  = S0 + sy0 * sstep + sx0 * cn;
                                v1  = S0 + sy0 * sstep + sx1 * cn;
                                v2  = S0 + sy1 * sstep + sx0 * cn;
                                v3  = S0 + sy1 * sstep + sx1 * cn;
                            }
                            else
                            {
                                sx0 = borderInterpolate(sx, ssize.width, borderType);
                                sx1 = borderInterpolate(sx + 1, ssize.width, borderType);
                                sy0 = borderInterpolate(sy, ssize.height, borderType);
                                sy1 = borderInterpolate(sy + 1, ssize.height, borderType);
                                v0  = sx0 >= 0 && sy0 >= 0 ? S0 + sy0 * sstep + sx0 * cn : &cval[0];
                                v1  = sx1 >= 0 && sy0 >= 0 ? S0 + sy0 * sstep + sx1 * cn : &cval[0];
                                v2  = sx0 >= 0 && sy1 >= 0 ? S0 + sy1 * sstep + sx0 * cn : &cval[0];
                                v3  = sx1 >= 0 && sy1 >= 0 ? S0 + sy1 * sstep + sx1 * cn : &cval[0];
                            }
                            for (int k = 0; k < cn; k++)
                                D[k] = castOp(WT(v0[k] * w[0] + v1[k] * w[1] + v2[k] * w[2] + v3[k] * w[3]));
                        }
                    }
            }
        }
    }
}

template <class CastOp, typename AT, int ONE, bool isRelative>
static void remapBicubic(const Mat& _src, Mat& _dst, const Mat& _xy, const Mat& _fxy, const void* _wtab, int borderType, const Scalar& _borderValue, const Point& _offset)
{
    typedef typename CastOp::rtype T;
    typedef typename CastOp::type1 WT;
    Size                           ssize = _src.size(), dsize = _dst.size();
    const int                      cn    = _src.channels();
    const AT*                      wtab  = (const AT*)_wtab;
    const T*                       S0    = _src.ptr<T>();
    size_t                         sstep = _src.step / sizeof(S0[0]);
    T                              cval[HL_CN_MAX];
    CastOp                         castOp;

    for (int k = 0; k < cn; k++)
        cval[k] = saturate_cast<T>(_borderValue[k & 3]);

    int borderType1 = borderType != BORDER_TRANSPARENT ? borderType : BORDER_REFLECT_101;

    unsigned width1 = std::max(ssize.width - 3, 0), height1 = std::max(ssize.height - 3, 0);

    if (_dst.isContinuous() && _xy.isContinuous() && _fxy.isContinuous() && !isRelative)
    {
        dsize.width  *= dsize.height;
        dsize.height  = 1;
    }

    for (int dy = 0; dy < dsize.height; dy++)
    {
        T*            D     = _dst.ptr<T>(dy);
        const short*  XY    = _xy.ptr<short>(dy);
        const ushort* FXY   = _fxy.ptr<ushort>(dy);
        const int     off_y = isRelative ? (_offset.y + dy) : 0;
        for (int dx = 0; dx < dsize.width; dx++, D += cn)
        {
            const int off_x = isRelative ? (_offset.x + dx) : 0;
            int       sx = XY[dx * 2] - 1 + off_x, sy = XY[dx * 2 + 1] - 1 + off_y;
            const AT* w = wtab + FXY[dx] * 16;
            if ((unsigned)sx < width1 && (unsigned)sy < height1)
            {
                const T* S = S0 + sy * sstep + sx * cn;
                for (int k = 0; k < cn; k++)
                {
                    WT sum  = S[0] * w[0] + S[cn] * w[1] + S[cn * 2] * w[2] + S[cn * 3] * w[3];
                    S      += sstep;
                    sum    += S[0] * w[4] + S[cn] * w[5] + S[cn * 2] * w[6] + S[cn * 3] * w[7];
                    S      += sstep;
                    sum    += S[0] * w[8] + S[cn] * w[9] + S[cn * 2] * w[10] + S[cn * 3] * w[11];
                    S      += sstep;
                    sum    += S[0] * w[12] + S[cn] * w[13] + S[cn * 2] * w[14] + S[cn * 3] * w[15];
                    S      += 1 - sstep * 3;
                    D[k]    = castOp(sum);
                }
            }
            else
            {
                int x[4], y[4];
                if (borderType == BORDER_TRANSPARENT && ((unsigned)(sx + 1) >= (unsigned)ssize.width || (unsigned)(sy + 1) >= (unsigned)ssize.height))
                    continue;

                if (borderType1 == BORDER_CONSTANT && (sx >= ssize.width || sx + 4 <= 0 || sy >= ssize.height || sy + 4 <= 0))
                {
                    for (int k = 0; k < cn; k++)
                        D[k] = cval[k];
                    continue;
                }

                for (int i = 0; i < 4; i++)
                {
                    x[i] = borderInterpolate(sx + i, ssize.width, borderType1) * cn;
                    y[i] = borderInterpolate(sy + i, ssize.height, borderType1);
                }

                for (int k = 0; k < cn; k++, S0++, w -= 16)
                {
                    WT cv = cval[k], sum = cv * ONE;
                    for (int i = 0; i < 4; i++, w += 4)
                    {
                        int      yi = y[i];
                        const T* S  = S0 + yi * sstep;
                        if (yi < 0)
                            continue;
                        if (x[0] >= 0)
                            sum += (S[x[0]] - cv) * w[0];
                        if (x[1] >= 0)
                            sum += (S[x[1]] - cv) * w[1];
                        if (x[2] >= 0)
                            sum += (S[x[2]] - cv) * w[2];
                        if (x[3] >= 0)
                            sum += (S[x[3]] - cv) * w[3];
                    }
                    D[k] = castOp(sum);
                }
                S0 -= cn;
            }
        }
    }
}

template <class CastOp, typename AT, int ONE, bool isRelative>
static void remapLanczos4(const Mat& _src, Mat& _dst, const Mat& _xy, const Mat& _fxy, const void* _wtab, int borderType, const Scalar& _borderValue, const Point& _offset)
{
    typedef typename CastOp::rtype T;
    typedef typename CastOp::type1 WT;
    Size                           ssize = _src.size(), dsize = _dst.size();
    const int                      cn    = _src.channels();
    const AT*                      wtab  = (const AT*)_wtab;
    const T*                       S0    = _src.ptr<T>();
    size_t                         sstep = _src.step / sizeof(S0[0]);
    T                              cval[HL_CN_MAX];
    CastOp                         castOp;

    for (int k = 0; k < cn; k++)
        cval[k] = saturate_cast<T>(_borderValue[k & 3]);

    int borderType1 = borderType != BORDER_TRANSPARENT ? borderType : BORDER_REFLECT_101;

    unsigned width1 = std::max(ssize.width - 7, 0), height1 = std::max(ssize.height - 7, 0);

    if (_dst.isContinuous() && _xy.isContinuous() && _fxy.isContinuous() && !isRelative)
    {
        dsize.width  *= dsize.height;
        dsize.height  = 1;
    }

    for (int dy = 0; dy < dsize.height; dy++)
    {
        T*            D     = _dst.ptr<T>(dy);
        const short*  XY    = _xy.ptr<short>(dy);
        const ushort* FXY   = _fxy.ptr<ushort>(dy);
        const int     off_y = isRelative ? (_offset.y + dy) : 0;
        for (int dx = 0; dx < dsize.width; dx++, D += cn)
        {
            const int off_x = isRelative ? (_offset.x + dx) : 0;
            int       sx = XY[dx * 2] - 3 + off_x, sy = XY[dx * 2 + 1] - 3 + off_y;
            const AT* w = wtab + FXY[dx] * 64;
            const T*  S = S0 + sy * sstep + sx * cn;
            if ((unsigned)sx < width1 && (unsigned)sy < height1)
            {
                for (int k = 0; k < cn; k++)
                {
                    WT sum = 0;
                    for (int r = 0; r < 8; r++, S += sstep, w += 8)
                        sum += S[0] * w[0] + S[cn] * w[1] + S[cn * 2] * w[2] + S[cn * 3] * w[3] + S[cn * 4] * w[4] + S[cn * 5] * w[5] + S[cn * 6] * w[6] + S[cn * 7] * w[7];
                    w    -= 64;
                    S    -= sstep * 8 - 1;
                    D[k]  = castOp(sum);
                }
            }
            else
            {
                int x[8], y[8];
                if (borderType == BORDER_TRANSPARENT && ((unsigned)(sx + 3) >= (unsigned)ssize.width || (unsigned)(sy + 3) >= (unsigned)ssize.height))
                    continue;

                if (borderType1 == BORDER_CONSTANT && (sx >= ssize.width || sx + 8 <= 0 || sy >= ssize.height || sy + 8 <= 0))
                {
                    for (int k = 0; k < cn; k++)
                        D[k] = cval[k];
                    continue;
                }

                for (int i = 0; i < 8; i++)
                {
                    x[i] = borderInterpolate(sx + i, ssize.width, borderType1) * cn;
                    y[i] = borderInterpolate(sy + i, ssize.height, borderType1);
                }

                for (int k = 0; k < cn; k++, S0++, w -= 64)
                {
                    WT cv = cval[k], sum = cv * ONE;
                    for (int i = 0; i < 8; i++, w += 8)
                    {
                        int      yi = y[i];
                        const T* S1 = S0 + yi * sstep;
                        if (yi < 0)
                            continue;
                        if (x[0] >= 0)
                            sum += (S1[x[0]] - cv) * w[0];
                        if (x[1] >= 0)
                            sum += (S1[x[1]] - cv) * w[1];
                        if (x[2] >= 0)
                            sum += (S1[x[2]] - cv) * w[2];
                        if (x[3] >= 0)
                            sum += (S1[x[3]] - cv) * w[3];
                        if (x[4] >= 0)
                            sum += (S1[x[4]] - cv) * w[4];
                        if (x[5] >= 0)
                            sum += (S1[x[5]] - cv) * w[5];
                        if (x[6] >= 0)
                            sum += (S1[x[6]] - cv) * w[6];
                        if (x[7] >= 0)
                            sum += (S1[x[7]] - cv) * w[7];
                    }
                    D[k] = castOp(sum);
                }
                S0 -= cn;
            }
        }
    }
}

typedef void (*RemapNNFunc)(const Mat& _src, Mat& _dst, const Mat& _xy, int borderType, const Scalar& _borderValue, const Point& _offset);

typedef void (*RemapFunc)(const Mat& _src, Mat& _dst, const Mat& _xy, const Mat& _fxy, const void* _wtab, int borderType, const Scalar& _borderValue, const Point& _offset);

class RemapInvoker:
    public ParallelLoopBody
{
public:
    RemapInvoker(const Mat& _src, Mat& _dst, const Mat* _m1, const Mat* _m2, int _borderType, const Scalar& _borderValue, int _planar_input, RemapNNFunc _nnfunc, RemapFunc _ifunc, const void* _ctab):
        ParallelLoopBody(), src(&_src), dst(&_dst), m1(_m1), m2(_m2), borderType(_borderType), borderValue(_borderValue), planar_input(_planar_input), nnfunc(_nnfunc), ifunc(_ifunc), ctab(_ctab)
    {
    }

    virtual void operator()(const Range& range) const override
    {
        int       x, y, x1, y1;
        const int buf_size = 1 << 14;
        int       brows0 = std::min(128, dst->rows), map_depth = m1->depth();
        int       bcols0 = std::min(buf_size / brows0, dst->cols);
        brows0           = std::min(buf_size / bcols0, dst->rows);

        Mat _bufxy(brows0, bcols0, HL_16SC2), _bufa;
        if (!nnfunc)
            _bufa.create(brows0, bcols0, HL_16UC1);

        for (y = range.start; y < range.end; y += brows0)
        {
            for (x = 0; x < dst->cols; x += bcols0)
            {
                int brows = std::min(brows0, range.end - y);
                int bcols = std::min(bcols0, dst->cols - x);
                Mat dpart(*dst, Rect(x, y, bcols, brows));
                Mat bufxy(_bufxy, Rect(0, 0, bcols, brows));

                if (nnfunc)
                {
                    if (m1->type() == HL_16SC2 && m2->empty())    // the data is already in the right format
                        bufxy = (*m1)(Rect(x, y, bcols, brows));
                    else if (map_depth != HL_32F)
                    {
                        for (y1 = 0; y1 < brows; y1++)
                        {
                            short*        XY  = bufxy.ptr<short>(y1);
                            const short*  sXY = m1->ptr<short>(y + y1) + x * 2;
                            const ushort* sA  = m2->ptr<ushort>(y + y1) + x;

                            for (x1 = 0; x1 < bcols; x1++)
                            {
                                int a          = sA[x1] & (INTER_TAB_SIZE2 - 1);
                                XY[x1 * 2]     = sXY[x1 * 2] + NNDeltaTab_i[a][0];
                                XY[x1 * 2 + 1] = sXY[x1 * 2 + 1] + NNDeltaTab_i[a][1];
                            }
                        }
                    }
                    else if (!planar_input)
                        (*m1)(Rect(x, y, bcols, brows)).convertTo(bufxy, bufxy.depth());
                    else
                    {
                        for (y1 = 0; y1 < brows; y1++)
                        {
                            short*       XY = bufxy.ptr<short>(y1);
                            const float* sX = m1->ptr<float>(y + y1) + x;
                            const float* sY = m2->ptr<float>(y + y1) + x;
                            x1              = 0;

                            for (; x1 < bcols; x1++)
                            {
                                XY[x1 * 2]     = saturate_cast<short>(sX[x1]);
                                XY[x1 * 2 + 1] = saturate_cast<short>(sY[x1]);
                            }
                        }
                    }
                    nnfunc(*src, dpart, bufxy, borderType, borderValue, Point(x, y));
                    continue;
                }

                Mat bufa(_bufa, Rect(0, 0, bcols, brows));
                for (y1 = 0; y1 < brows; y1++)
                {
                    short*  XY = bufxy.ptr<short>(y1);
                    ushort* A  = bufa.ptr<ushort>(y1);

                    if (m1->type() == HL_16SC2 && (m2->type() == HL_16UC1 || m2->type() == HL_16SC1))
                    {
                        bufxy            = (*m1)(Rect(x, y, bcols, brows));

                        const ushort* sA = m2->ptr<ushort>(y + y1) + x;
                        x1               = 0;

                        for (; x1 < bcols; x1++)
                            A[x1] = (ushort)(sA[x1] & (INTER_TAB_SIZE2 - 1));
                    }
                    else if (planar_input)
                    {
                        const float* sX = m1->ptr<float>(y + y1) + x;
                        const float* sY = m2->ptr<float>(y + y1) + x;

                        x1              = 0;
                        for (; x1 < bcols; x1++)
                        {
                            int sx         = hlRound(sX[x1] * static_cast<int>(INTER_TAB_SIZE));
                            int sy         = hlRound(sY[x1] * static_cast<int>(INTER_TAB_SIZE));
                            int v          = (sy & (INTER_TAB_SIZE - 1)) * INTER_TAB_SIZE + (sx & (INTER_TAB_SIZE - 1));
                            XY[x1 * 2]     = saturate_cast<short>(sx >> INTER_BITS);
                            XY[x1 * 2 + 1] = saturate_cast<short>(sy >> INTER_BITS);
                            A[x1]          = (ushort)v;
                        }
                    }
                    else
                    {
                        const float* sXY = m1->ptr<float>(y + y1) + x * 2;
                        x1               = 0;

                        for (; x1 < bcols; x1++)
                        {
                            int sx         = hlRound(sXY[x1 * 2] * static_cast<int>(INTER_TAB_SIZE));
                            int sy         = hlRound(sXY[x1 * 2 + 1] * static_cast<int>(INTER_TAB_SIZE));
                            int v          = (sy & (INTER_TAB_SIZE - 1)) * INTER_TAB_SIZE + (sx & (INTER_TAB_SIZE - 1));
                            XY[x1 * 2]     = saturate_cast<short>(sx >> INTER_BITS);
                            XY[x1 * 2 + 1] = saturate_cast<short>(sy >> INTER_BITS);
                            A[x1]          = (ushort)v;
                        }
                    }
                }
                ifunc(*src, dpart, bufxy, bufa, ctab, borderType, borderValue, Point(x, y));
            }
        }
    }

private:
    const Mat*  src;
    Mat*        dst;
    const Mat * m1, *m2;
    int         borderType;
    Scalar      borderValue;
    int         planar_input;
    RemapNNFunc nnfunc;
    RemapFunc   ifunc;
    const void* ctab;
};

}    // namespace hl

void hl::remap(const Mat& _src, Mat& _dst, const Mat& _map1, const Mat& _map2, int interpolation, int borderType, const Scalar& borderValue)
{
    const bool hasRelativeFlag      = ((interpolation & WARP_RELATIVE_MAP) != 0);

    static RemapNNFunc nn_tab[2][8] = {
        {remapNearest<uchar, false>, remapNearest<schar, false>, remapNearest<ushort, false>, remapNearest<short, false>, remapNearest<int, false>, remapNearest<float, false>, remapNearest<double, false>, 0},
        {remapNearest<uchar, true>, remapNearest<schar, true>, remapNearest<ushort, true>, remapNearest<short, true>, remapNearest<int, true>, remapNearest<float, true>, remapNearest<double, true>, 0}};

    static RemapFunc linear_tab[2][8] = {
        {remapBilinear<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, RemapVec_8u<false>, short, false>, 0, remapBilinear<Cast<float, ushort>, RemapNoVec<false>, float, false>, remapBilinear<Cast<float, short>, RemapNoVec<false>, float, false>, 0, remapBilinear<Cast<float, float>, RemapNoVec<false>, float, false>, remapBilinear<Cast<double, double>, RemapNoVec<false>, float, false>, 0},
        {remapBilinear<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, RemapVec_8u<true>, short, true>, 0, remapBilinear<Cast<float, ushort>, RemapNoVec<true>, float, true>, remapBilinear<Cast<float, short>, RemapNoVec<true>, float, true>, 0, remapBilinear<Cast<float, float>, RemapNoVec<true>, float, true>, remapBilinear<Cast<double, double>, RemapNoVec<true>, float, true>, 0}};

    static RemapFunc cubic_tab[2][8] = {
        {remapBicubic<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, short, INTER_REMAP_COEF_SCALE, false>, 0, remapBicubic<Cast<float, ushort>, float, 1, false>, remapBicubic<Cast<float, short>, float, 1, false>, 0, remapBicubic<Cast<float, float>, float, 1, false>, remapBicubic<Cast<double, double>, float, 1, false>, 0},
        {remapBicubic<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, short, INTER_REMAP_COEF_SCALE, true>, 0, remapBicubic<Cast<float, ushort>, float, 1, true>, remapBicubic<Cast<float, short>, float, 1, true>, 0, remapBicubic<Cast<float, float>, float, 1, true>, remapBicubic<Cast<double, double>, float, 1, true>, 0}};

    static RemapFunc lanczos4_tab[2][8] = {
        {remapLanczos4<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, short, INTER_REMAP_COEF_SCALE, false>, 0, remapLanczos4<Cast<float, ushort>, float, 1, false>, remapLanczos4<Cast<float, short>, float, 1, false>, 0, remapLanczos4<Cast<float, float>, float, 1, false>, remapLanczos4<Cast<double, double>, float, 1, false>, 0},
        {remapLanczos4<FixedPtCast<int, uchar, INTER_REMAP_COEF_BITS>, short, INTER_REMAP_COEF_SCALE, true>, 0, remapLanczos4<Cast<float, ushort>, float, 1, true>, remapLanczos4<Cast<float, short>, float, 1, true>, 0, remapLanczos4<Cast<float, float>, float, 1, true>, remapLanczos4<Cast<double, double>, float, 1, true>, 0}};

    HL_Assert(!_map1.empty());
    HL_Assert(_map2.empty() || (_map2.size() == _map1.size()));

    Mat src = _src, map1 = _map1, map2 = _map2;
    _dst.create(map1.size(), src.type());
    Mat dst = _dst;

    HL_Assert(dst.cols < SHRT_MAX && dst.rows < SHRT_MAX && src.cols < SHRT_MAX && src.rows < SHRT_MAX);

    if (dst.data == src.data)
        src = src.clone();

    interpolation &= ~WARP_RELATIVE_MAP;
    if (interpolation == INTER_AREA)
        interpolation = INTER_LINEAR;

    int type = src.type(), depth = HL_MAT_DEPTH(type);

    RemapNNFunc nnfunc            = 0;
    RemapFunc   ifunc             = 0;
    const void* ctab              = 0;
    bool        fixpt             = depth == HL_8U;
    bool        planar_input      = false;

    const int relativeOptionIndex = (hasRelativeFlag ? 1 : 0);
    if (interpolation == INTER_NEAREST)
    {
        nnfunc = nn_tab[relativeOptionIndex][depth];
        HL_Assert(nnfunc != 0);
    }
    else
    {
        if (interpolation == INTER_LINEAR)
            ifunc = linear_tab[relativeOptionIndex][depth];
        else if (interpolation == INTER_CUBIC)
        {
            ifunc = cubic_tab[relativeOptionIndex][depth];
            HL_Assert(_src.channels() <= 4);
        }
        else if (interpolation == INTER_LANCZOS4)
        {
            ifunc = lanczos4_tab[relativeOptionIndex][depth];
            HL_Assert(_src.channels() <= 4);
        }
        else
            HL_Error(hl::Error::StsBadArg, "Unknown interpolation method");
        HL_Assert(ifunc != 0);
        ctab = initInterTab2D(interpolation, fixpt);
    }

    const Mat *m1 = &map1, *m2 = &map2;

    if ((map1.type() == HL_16SC2 && (map2.type() == HL_16UC1 || map2.type() == HL_16SC1 || map2.empty())) || (map2.type() == HL_16SC2 && (map1.type() == HL_16UC1 || map1.type() == HL_16SC1 || map1.empty())))
    {
        if (map1.type() != HL_16SC2)
            std::swap(m1, m2);
    }
    else
    {
        HL_Assert(((map1.type() == HL_32FC2 || map1.type() == HL_16SC2) && map2.empty()) || (map1.type() == HL_32FC1 && map2.type() == HL_32FC1));
        planar_input = map1.channels() == 1;
    }

    RemapInvoker invoker(src, dst, m1, m2, borderType, borderValue, planar_input, nnfunc, ifunc, ctab);
    parallel_for_(Range(0, dst.rows), invoker, dst.total() / (double)(1 << 16));
}

namespace hl
{

class WarpAffineInvoker:
    public ParallelLoopBody
{
public:
    WarpAffineInvoker(const Mat& _src, Mat& _dst, int _interpolation, int _borderType, const Scalar& _borderValue, int* _adelta, int* _bdelta, const double* _M):
        ParallelLoopBody(), src(_src), dst(_dst), interpolation(_interpolation), borderType(_borderType), borderValue(_borderValue), adelta(_adelta), bdelta(_bdelta), M(_M)
    {
    }

    virtual void operator()(const Range& range) const override
    {
        const int            BLOCK_SZ = 64;
        AutoBuffer<short, 0> __XY(BLOCK_SZ * BLOCK_SZ * 2), __A(BLOCK_SZ * BLOCK_SZ);
        short *              XY = __XY.data(), *A = __A.data();
        const int            AB_BITS     = MAX(10, (int)INTER_BITS);
        const int            AB_SCALE    = 1 << AB_BITS;
        int                  round_delta = interpolation == INTER_NEAREST ? AB_SCALE / 2 : AB_SCALE / INTER_TAB_SIZE / 2, x, y, y1;

        int bh0                          = std::min(BLOCK_SZ / 2, dst.rows);
        int bw0                          = std::min(BLOCK_SZ * BLOCK_SZ / bh0, dst.cols);
        bh0                              = std::min(BLOCK_SZ * BLOCK_SZ / bw0, dst.rows);

        for (y = range.start; y < range.end; y += bh0)
        {
            for (x = 0; x < dst.cols; x += bw0)
            {
                int bw = std::min(bw0, dst.cols - x);
                int bh = std::min(bh0, range.end - y);

                Mat _XY(bh, bw, HL_16SC2, XY);
                Mat dpart(dst, Rect(x, y, bw, bh));

                for (y1 = 0; y1 < bh; y1++)
                {
                    short* xy = XY + y1 * bw * 2;
                    int    X0 = saturate_cast<int>((M[1] * (y + y1) + M[2]) * AB_SCALE) + round_delta;
                    int    Y0 = saturate_cast<int>((M[4] * (y + y1) + M[5]) * AB_SCALE) + round_delta;

                    if (interpolation == INTER_NEAREST)
                        hal::warpAffineBlocklineNN(adelta + x, bdelta + x, xy, X0, Y0, bw);
                    else
                        hal::warpAffineBlockline(adelta + x, bdelta + x, xy, A + y1 * bw, X0, Y0, bw);
                }

                if (interpolation == INTER_NEAREST)
                    remap(src, dpart, _XY, Mat(), interpolation, borderType, borderValue);
                else
                {
                    Mat _matA(bh, bw, HL_16U, A);
                    remap(src, dpart, _XY, _matA, interpolation, borderType, borderValue);
                }
            }
        }
    }

private:
    Mat           src;
    Mat           dst;
    int           interpolation, borderType;
    Scalar        borderValue;
    int *         adelta, *bdelta;
    const double* M;
};

namespace hal
{

void warpAffine(int src_type, const uchar* src_data, size_t src_step, int src_width, int src_height, uchar* dst_data, size_t dst_step, int dst_width, int dst_height, const double M[6], int interpolation, int borderType, const double borderValue[4])
{
    Mat src(Size(src_width, src_height), src_type, const_cast<uchar*>(src_data), src_step);
    Mat dst(Size(dst_width, dst_height), src_type, dst_data, dst_step);

    int             x;
    AutoBuffer<int> _abdelta(dst.cols * 2);
    int *           adelta = &_abdelta[0], *bdelta = adelta + dst.cols;
    const int       AB_BITS  = MAX(10, (int)INTER_BITS);
    const int       AB_SCALE = 1 << AB_BITS;

    for (x = 0; x < dst.cols; x++)
    {
        adelta[x] = saturate_cast<int>(M[0] * x * AB_SCALE);
        bdelta[x] = saturate_cast<int>(M[3] * x * AB_SCALE);
    }

    Range             range(0, dst.rows);
    WarpAffineInvoker invoker(src, dst, interpolation, borderType, Scalar(borderValue[0], borderValue[1], borderValue[2], borderValue[3]), adelta, bdelta, M);
    parallel_for_(range, invoker, dst.total() / (double)(1 << 16));
}

void warpAffineBlocklineNN(int* adelta, int* bdelta, short* xy, int X0, int Y0, int bw)
{
    constexpr int AB_BITS = MAX(10, static_cast<int>(INTER_BITS));
    int           x1      = 0;
    for (; x1 < bw; x1++)
    {
        const int X    = (X0 + adelta[x1]) >> AB_BITS;
        const int Y    = (Y0 + bdelta[x1]) >> AB_BITS;
        xy[x1 * 2]     = saturate_cast<short>(X);
        xy[x1 * 2 + 1] = saturate_cast<short>(Y);
    }
}

void warpAffineBlockline(int* adelta, int* bdelta, short* xy, short* alpha, int X0, int Y0, int bw)
{
    const int AB_BITS = MAX(10, (int)INTER_BITS);

    for (int x1 = 0; x1 < bw; x1++)
    {
        int X          = (X0 + adelta[x1]) >> (AB_BITS - INTER_BITS);
        int Y          = (Y0 + bdelta[x1]) >> (AB_BITS - INTER_BITS);
        xy[x1 * 2]     = saturate_cast<short>(X >> INTER_BITS);
        xy[x1 * 2 + 1] = saturate_cast<short>(Y >> INTER_BITS);
        alpha[x1]      = (short)((Y & (INTER_TAB_SIZE - 1)) * INTER_TAB_SIZE + (X & (INTER_TAB_SIZE - 1)));
    }
}

}    // namespace hal
}    // namespace hl

void hl::warpAffine(const Mat& _src, Mat& _dst, const Mat& _M0, Size dsize, int flags, int borderType, const Scalar& borderValue)
{
    int interpolation = flags & INTER_MAX;
    HL_Assert(_src.channels() <= 4 || (interpolation != INTER_LANCZOS4 && interpolation != INTER_CUBIC));

    Mat src = _src, M0 = _M0;
    _dst.create(dsize.empty() ? src.size() : dsize, src.type());
    Mat dst = _dst;
    HL_Assert(src.cols > 0 && src.rows > 0);
    if (dst.data == src.data)
        src = src.clone();

    double M[6] = {0};
    Mat    matM(2, 3, HL_64F, M);
    if (interpolation == INTER_AREA)
        interpolation = INTER_LINEAR;

    HL_Assert((M0.type() == HL_32F || M0.type() == HL_64F) && M0.rows == 2 && M0.cols == 3);
    M0.convertTo(matM, matM.type());

    if (!(flags & WARP_INVERSE_MAP))
    {
        double D   = M[0] * M[4] - M[1] * M[3];
        D          = D != 0 ? 1. / D : 0;
        double A11 = M[4] * D, A22 = M[0] * D;
        M[0]       = A11;
        M[1]      *= -D;
        M[3]      *= -D;
        M[4]       = A22;
        double b1  = -M[0] * M[2] - M[1] * M[5];
        double b2  = -M[3] * M[2] - M[4] * M[5];
        M[2]       = b1;
        M[5]       = b2;
    }

    hal::warpAffine(src.type(), src.data, src.step, src.cols, src.rows, dst.data, dst.step, dst.cols, dst.rows, M, interpolation, borderType, borderValue.val);
}
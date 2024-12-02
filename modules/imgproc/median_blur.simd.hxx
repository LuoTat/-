#include "precomp.hxx"
#include <vector>

namespace hl
{
namespace cpu_baseline
{
static void medianBlur_8u_O1(const Mat& _src, Mat& _dst, int ksize)
{
    typedef ushort HT;

    /**
     * This structure represents a two-tier histogram. The first tier (known as the
     * "coarse" level) is 4 bit wide and the second tier (known as the "fine" level)
     * is 8 bit wide. Pixels inserted in the fine level also get inserted into the
     * coarse bucket designated by the 4 MSBs of the fine bucket value.
     *
     * The structure is aligned on 16 bits, which is a prerequisite for SIMD
     * instructions. Each bucket is 16 bit wide, which means that extra care must be
     * taken to prevent overflow.
     */
    typedef struct
    {
        HT coarse[16];
        HT fine[16][16];
    } Histogram;

/**
 * HOP is short for Histogram OPeration. This macro makes an operation \a op on
 * histogram \a h for pixel value \a x. It takes care of handling both levels.
 */
#define HOP(h, x, op)    \
    h.coarse[x >> 4] op, \
        *((HT*)h.fine + x) op

#define COP(c, j, x, op)                      \
    h_coarse[16 * (n * c + j) + (x >> 4)] op, \
        h_fine[16 * (n * (16 * c + (x >> 4)) + j) + (x & 0xF)] op

    int cn = _dst.channels(), m = _dst.rows, r = (ksize - 1) / 2;
    HL_Assert(cn > 0 && cn <= 4);
    size_t sstep = _src.step, dstep = _dst.step;

    int STRIPE_SIZE = std::min(_dst.cols, 512 / cn);

#define HL_ALIGNMENT 16

    std::vector<HT> _h_coarse(1 * 16 * (STRIPE_SIZE + 2 * r) * cn + HL_ALIGNMENT);
    std::vector<HT> _h_fine(16 * 16 * (STRIPE_SIZE + 2 * r) * cn + HL_ALIGNMENT);
    HT*             h_coarse = alignPtr(&_h_coarse[0], HL_ALIGNMENT);
    HT*             h_fine   = alignPtr(&_h_fine[0], HL_ALIGNMENT);

    for (int x = 0; x < _dst.cols; x += STRIPE_SIZE)
    {
        int          i, j, k, c, n = std::min(_dst.cols - x, STRIPE_SIZE) + r * 2;
        const uchar* src = _src.ptr() + x * cn;
        uchar*       dst = _dst.ptr() + (x - r) * cn;

        memset(h_coarse, 0, 16 * n * cn * sizeof(h_coarse[0]));
        memset(h_fine, 0, 16 * 16 * n * cn * sizeof(h_fine[0]));

        // First row initialization
        for (c = 0; c < cn; c++)
        {
            for (j = 0; j < n; j++)
                COP(c, j, src[cn * j + c], += (HT)(r + 2));

            for (i = 1; i < r; i++)
            {
                const uchar* p = src + sstep * std::min(i, m - 1);
                for (j = 0; j < n; j++)
                    COP(c, j, p[cn * j + c], ++);
            }
        }

        for (i = 0; i < m; i++)
        {
            const uchar* p0 = src + sstep * std::max(0, i - r - 1);
            const uchar* p1 = src + sstep * std::min(m - 1, i + r);

            for (c = 0; c < cn; c++)
            {
                Histogram HL_DECL_ALIGNED(HL_ALIGNMENT) H;
                HT        HL_DECL_ALIGNED(HL_ALIGNMENT) luc[16];

                memset(&H, 0, sizeof(H));
                memset(luc, 0, sizeof(luc));

                // Update column histograms for the entire row.
                for (j = 0; j < n; j++)
                {
                    COP(c, j, p0[j * cn + c], --);
                    COP(c, j, p1[j * cn + c], ++);
                }

                // First column initialization
                for (k = 0; k < 16; ++k)
                {
                    for (int ind = 0; ind < 16; ++ind)
                        H.fine[k][ind] = (HT)(H.fine[k][ind] + (2 * r + 1) * h_fine[16 * n * (16 * c + k) + ind]);
                }

                HT* px = h_coarse + 16 * n * c;
                for (j = 0; j < 2 * r; ++j, px += 16)
                {
                    for (int ind = 0; ind < 16; ++ind)
                        H.coarse[ind] += px[ind];
                }

                for (j = r; j < n - r; j++)
                {
                    int t = 2 * r * r + 2 * r, b, sum = 0;
                    HT* segment;
                    px = h_coarse + 16 * (n * c + std::min(j + r, n - 1));
                    for (int ind = 0; ind < 16; ++ind)
                        H.coarse[ind] += px[ind];

                    // Find median at coarse level
                    for (k = 0; k < 16; ++k)
                    {
                        sum += H.coarse[k];
                        if (sum > t)
                        {
                            sum -= H.coarse[k];
                            break;
                        }
                    }
                    HL_Assert(k < 16);

                    /* Update corresponding histogram segment */
                    if (luc[k] <= j - r)
                    {
                        memset(&H.fine[k], 0, 16 * sizeof(HT));
                        px = h_fine + 16 * (n * (16 * c + k) + j - r);
                        for (luc[k] = HT(j - r); luc[k] < MIN(j + r + 1, n); ++luc[k], px += 16)
                        {
                            for (int ind = 0; ind < 16; ++ind)
                                H.fine[k][ind] += px[ind];
                        }

                        if (luc[k] < j + r + 1)
                        {
                            px = h_fine + 16 * (n * (16 * c + k) + (n - 1));
                            for (int ind = 0; ind < 16; ++ind)
                                H.fine[k][ind] = (HT)(H.fine[k][ind] + (j + r + 1 - n) * px[ind]);
                            luc[k] = (HT)(j + r + 1);
                        }
                    }
                    else
                    {
                        px = h_fine + 16 * n * (16 * c + k);
                        for (; luc[k] < j + r + 1; ++luc[k])
                        {
                            for (int ind = 0; ind < 16; ++ind)
                                H.fine[k][ind] += px[16 * MIN(luc[k], n - 1) + ind] - px[16 * MAX(luc[k] - 2 * r - 1, 0) + ind];
                        }
                    }

                    px = h_coarse + 16 * (n * c + MAX(j - r, 0));
                    for (int ind = 0; ind < 16; ++ind)
                        H.coarse[ind] -= px[ind];

                    /* Find median in segment */
                    segment = H.fine[k];
                    for (b = 0; b < 16; b++)
                    {
                        sum += segment[b];
                        if (sum > t)
                        {
                            dst[dstep * i + cn * j + c] = (uchar)(16 * k + b);
                            break;
                        }
                    }
                    HL_Assert(b < 16);
                }
            }
        }
    }

#undef HOP
#undef COP
}

static void medianBlur_8u_Om(const Mat& _src, Mat& _dst, int m)
{
#define N 16
    int          zone0[4][N];
    int          zone1[4][N * N];
    int          x, y;
    int          n2       = m * m / 2;
    Size         size     = _dst.size();
    const uchar* src      = _src.ptr();
    uchar*       dst      = _dst.ptr();
    int          src_step = (int)_src.step, dst_step = (int)_dst.step;
    int          cn      = _src.channels();
    const uchar* src_max = src + size.height * src_step;
    HL_Assert(cn > 0 && cn <= 4);

#define UPDATE_ACC01(pix, cn, op) \
    {                             \
        int p = (pix);            \
        zone1[cn][p] op;          \
        zone0[cn][p >> 4] op;     \
    }

    //HL_Assert( size.height >= nx && size.width >= nx );
    for (x = 0; x < size.width; x++, src += cn, dst += cn)
    {
        uchar*       dst_cur    = dst;
        const uchar* src_top    = src;
        const uchar* src_bottom = src;
        int          k, c;
        int          src_step1 = src_step, dst_step1 = dst_step;

        if (x % 2 != 0)
        {
            src_bottom = src_top += src_step * (size.height - 1);
            dst_cur              += dst_step * (size.height - 1);
            src_step1             = -src_step1;
            dst_step1             = -dst_step1;
        }

        // init accumulator
        memset(zone0, 0, sizeof(zone0[0]) * cn);
        memset(zone1, 0, sizeof(zone1[0]) * cn);

        for (y = 0; y <= m / 2; y++)
        {
            for (c = 0; c < cn; c++)
            {
                if (y > 0)
                {
                    for (k = 0; k < m * cn; k += cn)
                        UPDATE_ACC01(src_bottom[k + c], c, ++);
                }
                else
                {
                    for (k = 0; k < m * cn; k += cn)
                        UPDATE_ACC01(src_bottom[k + c], c, += m / 2 + 1);
                }
            }

            if ((src_step1 > 0 && y < size.height - 1) || (src_step1 < 0 && size.height - y - 1 > 0))
                src_bottom += src_step1;
        }

        for (y = 0; y < size.height; y++, dst_cur += dst_step1)
        {
            // find median
            for (c = 0; c < cn; c++)
            {
                int s = 0;
                for (k = 0;; k++)
                {
                    int t = s + zone0[c][k];
                    if (t > n2) break;
                    s = t;
                }

                for (k *= N;; k++)
                {
                    s += zone1[c][k];
                    if (s > n2) break;
                }

                dst_cur[c] = (uchar)k;
            }

            if (y + 1 == size.height)
                break;

            if (cn == 1)
            {
                for (k = 0; k < m; k++)
                {
                    int p = src_top[k];
                    int q = src_bottom[k];
                    zone1[0][p]--;
                    zone0[0][p >> 4]--;
                    zone1[0][q]++;
                    zone0[0][q >> 4]++;
                }
            }
            else if (cn == 3)
            {
                for (k = 0; k < m * 3; k += 3)
                {
                    UPDATE_ACC01(src_top[k], 0, --);
                    UPDATE_ACC01(src_top[k + 1], 1, --);
                    UPDATE_ACC01(src_top[k + 2], 2, --);

                    UPDATE_ACC01(src_bottom[k], 0, ++);
                    UPDATE_ACC01(src_bottom[k + 1], 1, ++);
                    UPDATE_ACC01(src_bottom[k + 2], 2, ++);
                }
            }
            else
            {
                HL_Assert(cn == 4);
                for (k = 0; k < m * 4; k += 4)
                {
                    UPDATE_ACC01(src_top[k], 0, --);
                    UPDATE_ACC01(src_top[k + 1], 1, --);
                    UPDATE_ACC01(src_top[k + 2], 2, --);
                    UPDATE_ACC01(src_top[k + 3], 3, --);

                    UPDATE_ACC01(src_bottom[k], 0, ++);
                    UPDATE_ACC01(src_bottom[k + 1], 1, ++);
                    UPDATE_ACC01(src_bottom[k + 2], 2, ++);
                    UPDATE_ACC01(src_bottom[k + 3], 3, ++);
                }
            }

            if ((src_step1 > 0 && src_bottom + src_step1 < src_max) || (src_step1 < 0 && src_bottom + src_step1 >= src))
                src_bottom += src_step1;

            if (y >= m / 2)
                src_top += src_step1;
        }
    }
#undef N
#undef UPDATE_ACC
}

namespace
{
struct MinMax8u
{
    typedef uchar value_type;
    typedef int   arg_type;

    arg_type load(const uchar* ptr) { return *ptr; }

    void store(uchar* ptr, arg_type val) { *ptr = (uchar)val; }

    void operator()(arg_type& a, arg_type& b) const
    {
        int t  = HL_FAST_CAST_8U(a - b);
        b     += t;
        a     -= t;
    }
};

struct MinMax16u
{
    typedef ushort value_type;
    typedef int    arg_type;

    arg_type load(const ushort* ptr) { return *ptr; }

    void store(ushort* ptr, arg_type val) { *ptr = (ushort)val; }

    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a          = std::min(a, b);
        b          = std::max(b, t);
    }
};

struct MinMax16s
{
    typedef short value_type;
    typedef int   arg_type;

    arg_type load(const short* ptr) { return *ptr; }

    void store(short* ptr, arg_type val) { *ptr = (short)val; }

    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a          = std::min(a, b);
        b          = std::max(b, t);
    }
};

struct MinMax32f
{
    typedef float value_type;
    typedef float arg_type;

    arg_type load(const float* ptr) { return *ptr; }

    void store(float* ptr, arg_type val) { *ptr = val; }

    void operator()(arg_type& a, arg_type& b) const
    {
        arg_type t = a;
        a          = std::min(a, b);
        b          = std::max(b, t);
    }
};

typedef MinMax8u  MinMaxVec8u;
typedef MinMax16u MinMaxVec16u;
typedef MinMax16s MinMaxVec16s;
typedef MinMax32f MinMaxVec32f;

template <class Op, class VecOp>
static void medianBlur_SortNet(const Mat& _src, Mat& _dst, int m)
{
    typedef typename Op::value_type  T;
    typedef typename Op::arg_type    WT;
    typedef typename VecOp::arg_type VT;

    const T* src   = _src.ptr<T>();
    T*       dst   = _dst.ptr<T>();
    int      sstep = (int)(_src.step / sizeof(T));
    int      dstep = (int)(_dst.step / sizeof(T));
    Size     size  = _dst.size();
    int      i, j, k, cn = _src.channels();
    Op       op;
    VecOp    vop;

    if (m == 3)
    {
        if (size.width == 1 || size.height == 1)
        {
            int len     = size.width + size.height - 1;
            int sdelta  = size.height == 1 ? cn : sstep;
            int sdelta0 = size.height == 1 ? 0 : sstep - cn;
            int ddelta  = size.height == 1 ? cn : dstep;

            for (i = 0; i < len; i++, src += sdelta0, dst += ddelta)
                for (j = 0; j < cn; j++, src++)
                {
                    WT p0 = src[i > 0 ? -sdelta : 0];
                    WT p1 = src[0];
                    WT p2 = src[i < len - 1 ? sdelta : 0];

                    op(p0, p1);
                    op(p1, p2);
                    op(p0, p1);
                    dst[j] = (T)p1;
                }
            return;
        }

        size.width *= cn;
        for (i = 0; i < size.height; i++, dst += dstep)
        {
            const T* row0  = src + std::max(i - 1, 0) * sstep;
            const T* row1  = src + i * sstep;
            const T* row2  = src + std::min(i + 1, size.height - 1) * sstep;
            int      limit = cn;

            for (j = 0;;)
            {
                for (; j < limit; j++)
                {
                    int j0 = j >= cn ? j - cn : j;
                    int j2 = j < size.width - cn ? j + cn : j;
                    WT  p0 = row0[j0], p1 = row0[j], p2 = row0[j2];
                    WT  p3 = row1[j0], p4 = row1[j], p5 = row1[j2];
                    WT  p6 = row2[j0], p7 = row2[j], p8 = row2[j2];

                    op(p1, p2);
                    op(p4, p5);
                    op(p7, p8);
                    op(p0, p1);
                    op(p3, p4);
                    op(p6, p7);
                    op(p1, p2);
                    op(p4, p5);
                    op(p7, p8);
                    op(p0, p3);
                    op(p5, p8);
                    op(p4, p7);
                    op(p3, p6);
                    op(p1, p4);
                    op(p2, p5);
                    op(p4, p7);
                    op(p4, p2);
                    op(p6, p4);
                    op(p4, p2);
                    dst[j] = (T)p4;
                }

                if (limit == size.width)
                    break;

                for (int nlanes = 1; j <= size.width - nlanes - cn; j += nlanes)
                {
                    VT p0 = vop.load(row0 + j - cn), p1 = vop.load(row0 + j), p2 = vop.load(row0 + j + cn);
                    VT p3 = vop.load(row1 + j - cn), p4 = vop.load(row1 + j), p5 = vop.load(row1 + j + cn);
                    VT p6 = vop.load(row2 + j - cn), p7 = vop.load(row2 + j), p8 = vop.load(row2 + j + cn);

                    vop(p1, p2);
                    vop(p4, p5);
                    vop(p7, p8);
                    vop(p0, p1);
                    vop(p3, p4);
                    vop(p6, p7);
                    vop(p1, p2);
                    vop(p4, p5);
                    vop(p7, p8);
                    vop(p0, p3);
                    vop(p5, p8);
                    vop(p4, p7);
                    vop(p3, p6);
                    vop(p1, p4);
                    vop(p2, p5);
                    vop(p4, p7);
                    vop(p4, p2);
                    vop(p6, p4);
                    vop(p4, p2);
                    vop.store(dst + j, p4);
                }

                limit = size.width;
            }
        }
    }
    else if (m == 5)
    {
        if (size.width == 1 || size.height == 1)
        {
            int len     = size.width + size.height - 1;
            int sdelta  = size.height == 1 ? cn : sstep;
            int sdelta0 = size.height == 1 ? 0 : sstep - cn;
            int ddelta  = size.height == 1 ? cn : dstep;

            for (i = 0; i < len; i++, src += sdelta0, dst += ddelta)
                for (j = 0; j < cn; j++, src++)
                {
                    int i1 = i > 0 ? -sdelta : 0;
                    int i0 = i > 1 ? -sdelta * 2 : i1;
                    int i3 = i < len - 1 ? sdelta : 0;
                    int i4 = i < len - 2 ? sdelta * 2 : i3;
                    WT  p0 = src[i0], p1 = src[i1], p2 = src[0], p3 = src[i3], p4 = src[i4];

                    op(p0, p1);
                    op(p3, p4);
                    op(p2, p3);
                    op(p3, p4);
                    op(p0, p2);
                    op(p2, p4);
                    op(p1, p3);
                    op(p1, p2);
                    dst[j] = (T)p2;
                }
            return;
        }

        size.width *= cn;
        for (i = 0; i < size.height; i++, dst += dstep)
        {
            const T* row[5];
            row[0]    = src + std::max(i - 2, 0) * sstep;
            row[1]    = src + std::max(i - 1, 0) * sstep;
            row[2]    = src + i * sstep;
            row[3]    = src + std::min(i + 1, size.height - 1) * sstep;
            row[4]    = src + std::min(i + 2, size.height - 1) * sstep;
            int limit = cn * 2;

            for (j = 0;;)
            {
                for (; j < limit; j++)
                {
                    WT  p[25];
                    int j1 = j >= cn ? j - cn : j;
                    int j0 = j >= cn * 2 ? j - cn * 2 : j1;
                    int j3 = j < size.width - cn ? j + cn : j;
                    int j4 = j < size.width - cn * 2 ? j + cn * 2 : j3;
                    for (k = 0; k < 5; k++)
                    {
                        const T* rowk = row[k];
                        p[k * 5]      = rowk[j0];
                        p[k * 5 + 1]  = rowk[j1];
                        p[k * 5 + 2]  = rowk[j];
                        p[k * 5 + 3]  = rowk[j3];
                        p[k * 5 + 4]  = rowk[j4];
                    }

                    op(p[1], p[2]);
                    op(p[0], p[1]);
                    op(p[1], p[2]);
                    op(p[4], p[5]);
                    op(p[3], p[4]);
                    op(p[4], p[5]);
                    op(p[0], p[3]);
                    op(p[2], p[5]);
                    op(p[2], p[3]);
                    op(p[1], p[4]);
                    op(p[1], p[2]);
                    op(p[3], p[4]);
                    op(p[7], p[8]);
                    op(p[6], p[7]);
                    op(p[7], p[8]);
                    op(p[10], p[11]);
                    op(p[9], p[10]);
                    op(p[10], p[11]);
                    op(p[6], p[9]);
                    op(p[8], p[11]);
                    op(p[8], p[9]);
                    op(p[7], p[10]);
                    op(p[7], p[8]);
                    op(p[9], p[10]);
                    op(p[0], p[6]);
                    op(p[4], p[10]);
                    op(p[4], p[6]);
                    op(p[2], p[8]);
                    op(p[2], p[4]);
                    op(p[6], p[8]);
                    op(p[1], p[7]);
                    op(p[5], p[11]);
                    op(p[5], p[7]);
                    op(p[3], p[9]);
                    op(p[3], p[5]);
                    op(p[7], p[9]);
                    op(p[1], p[2]);
                    op(p[3], p[4]);
                    op(p[5], p[6]);
                    op(p[7], p[8]);
                    op(p[9], p[10]);
                    op(p[13], p[14]);
                    op(p[12], p[13]);
                    op(p[13], p[14]);
                    op(p[16], p[17]);
                    op(p[15], p[16]);
                    op(p[16], p[17]);
                    op(p[12], p[15]);
                    op(p[14], p[17]);
                    op(p[14], p[15]);
                    op(p[13], p[16]);
                    op(p[13], p[14]);
                    op(p[15], p[16]);
                    op(p[19], p[20]);
                    op(p[18], p[19]);
                    op(p[19], p[20]);
                    op(p[21], p[22]);
                    op(p[23], p[24]);
                    op(p[21], p[23]);
                    op(p[22], p[24]);
                    op(p[22], p[23]);
                    op(p[18], p[21]);
                    op(p[20], p[23]);
                    op(p[20], p[21]);
                    op(p[19], p[22]);
                    op(p[22], p[24]);
                    op(p[19], p[20]);
                    op(p[21], p[22]);
                    op(p[23], p[24]);
                    op(p[12], p[18]);
                    op(p[16], p[22]);
                    op(p[16], p[18]);
                    op(p[14], p[20]);
                    op(p[20], p[24]);
                    op(p[14], p[16]);
                    op(p[18], p[20]);
                    op(p[22], p[24]);
                    op(p[13], p[19]);
                    op(p[17], p[23]);
                    op(p[17], p[19]);
                    op(p[15], p[21]);
                    op(p[15], p[17]);
                    op(p[19], p[21]);
                    op(p[13], p[14]);
                    op(p[15], p[16]);
                    op(p[17], p[18]);
                    op(p[19], p[20]);
                    op(p[21], p[22]);
                    op(p[23], p[24]);
                    op(p[0], p[12]);
                    op(p[8], p[20]);
                    op(p[8], p[12]);
                    op(p[4], p[16]);
                    op(p[16], p[24]);
                    op(p[12], p[16]);
                    op(p[2], p[14]);
                    op(p[10], p[22]);
                    op(p[10], p[14]);
                    op(p[6], p[18]);
                    op(p[6], p[10]);
                    op(p[10], p[12]);
                    op(p[1], p[13]);
                    op(p[9], p[21]);
                    op(p[9], p[13]);
                    op(p[5], p[17]);
                    op(p[13], p[17]);
                    op(p[3], p[15]);
                    op(p[11], p[23]);
                    op(p[11], p[15]);
                    op(p[7], p[19]);
                    op(p[7], p[11]);
                    op(p[11], p[13]);
                    op(p[11], p[12]);
                    dst[j] = (T)p[12];
                }

                if (limit == size.width)
                    break;

                for (int nlanes = 1; j <= size.width - nlanes - cn * 2; j += nlanes)
                {
                    VT p0 = vop.load(row[0] + j - cn * 2), p5 = vop.load(row[1] + j - cn * 2), p10 = vop.load(row[2] + j - cn * 2), p15 = vop.load(row[3] + j - cn * 2), p20 = vop.load(row[4] + j - cn * 2);
                    VT p1 = vop.load(row[0] + j - cn * 1), p6 = vop.load(row[1] + j - cn * 1), p11 = vop.load(row[2] + j - cn * 1), p16 = vop.load(row[3] + j - cn * 1), p21 = vop.load(row[4] + j - cn * 1);
                    VT p2 = vop.load(row[0] + j - cn * 0), p7 = vop.load(row[1] + j - cn * 0), p12 = vop.load(row[2] + j - cn * 0), p17 = vop.load(row[3] + j - cn * 0), p22 = vop.load(row[4] + j - cn * 0);
                    VT p3 = vop.load(row[0] + j + cn * 1), p8 = vop.load(row[1] + j + cn * 1), p13 = vop.load(row[2] + j + cn * 1), p18 = vop.load(row[3] + j + cn * 1), p23 = vop.load(row[4] + j + cn * 1);
                    VT p4 = vop.load(row[0] + j + cn * 2), p9 = vop.load(row[1] + j + cn * 2), p14 = vop.load(row[2] + j + cn * 2), p19 = vop.load(row[3] + j + cn * 2), p24 = vop.load(row[4] + j + cn * 2);

                    vop(p1, p2);
                    vop(p0, p1);
                    vop(p1, p2);
                    vop(p4, p5);
                    vop(p3, p4);
                    vop(p4, p5);
                    vop(p0, p3);
                    vop(p2, p5);
                    vop(p2, p3);
                    vop(p1, p4);
                    vop(p1, p2);
                    vop(p3, p4);
                    vop(p7, p8);
                    vop(p6, p7);
                    vop(p7, p8);
                    vop(p10, p11);
                    vop(p9, p10);
                    vop(p10, p11);
                    vop(p6, p9);
                    vop(p8, p11);
                    vop(p8, p9);
                    vop(p7, p10);
                    vop(p7, p8);
                    vop(p9, p10);
                    vop(p0, p6);
                    vop(p4, p10);
                    vop(p4, p6);
                    vop(p2, p8);
                    vop(p2, p4);
                    vop(p6, p8);
                    vop(p1, p7);
                    vop(p5, p11);
                    vop(p5, p7);
                    vop(p3, p9);
                    vop(p3, p5);
                    vop(p7, p9);
                    vop(p1, p2);
                    vop(p3, p4);
                    vop(p5, p6);
                    vop(p7, p8);
                    vop(p9, p10);
                    vop(p13, p14);
                    vop(p12, p13);
                    vop(p13, p14);
                    vop(p16, p17);
                    vop(p15, p16);
                    vop(p16, p17);
                    vop(p12, p15);
                    vop(p14, p17);
                    vop(p14, p15);
                    vop(p13, p16);
                    vop(p13, p14);
                    vop(p15, p16);
                    vop(p19, p20);
                    vop(p18, p19);
                    vop(p19, p20);
                    vop(p21, p22);
                    vop(p23, p24);
                    vop(p21, p23);
                    vop(p22, p24);
                    vop(p22, p23);
                    vop(p18, p21);
                    vop(p20, p23);
                    vop(p20, p21);
                    vop(p19, p22);
                    vop(p22, p24);
                    vop(p19, p20);
                    vop(p21, p22);
                    vop(p23, p24);
                    vop(p12, p18);
                    vop(p16, p22);
                    vop(p16, p18);
                    vop(p14, p20);
                    vop(p20, p24);
                    vop(p14, p16);
                    vop(p18, p20);
                    vop(p22, p24);
                    vop(p13, p19);
                    vop(p17, p23);
                    vop(p17, p19);
                    vop(p15, p21);
                    vop(p15, p17);
                    vop(p19, p21);
                    vop(p13, p14);
                    vop(p15, p16);
                    vop(p17, p18);
                    vop(p19, p20);
                    vop(p21, p22);
                    vop(p23, p24);
                    vop(p0, p12);
                    vop(p8, p20);
                    vop(p8, p12);
                    vop(p4, p16);
                    vop(p16, p24);
                    vop(p12, p16);
                    vop(p2, p14);
                    vop(p10, p22);
                    vop(p10, p14);
                    vop(p6, p18);
                    vop(p6, p10);
                    vop(p10, p12);
                    vop(p1, p13);
                    vop(p9, p21);
                    vop(p9, p13);
                    vop(p5, p17);
                    vop(p13, p17);
                    vop(p3, p15);
                    vop(p11, p23);
                    vop(p11, p15);
                    vop(p7, p19);
                    vop(p7, p11);
                    vop(p11, p13);
                    vop(p11, p12);
                    vop.store(dst + j, p12);
                }

                limit = size.width;
            }
        }
    }
}
}    // namespace

void medianBlur(const Mat& src0, Mat& dst, int ksize)
{
    bool useSortNet = ksize == 3 || (ksize == 5);

    Mat src;
    if (useSortNet)
    {
        if (dst.data != src0.data)
            src = src0;
        else
            src0.copyTo(src);

        if (src.depth() == HL_8U)
            medianBlur_SortNet<MinMax8u, MinMaxVec8u>(src, dst, ksize);
        else if (src.depth() == HL_16U)
            medianBlur_SortNet<MinMax16u, MinMaxVec16u>(src, dst, ksize);
        else if (src.depth() == HL_16S)
            medianBlur_SortNet<MinMax16s, MinMaxVec16s>(src, dst, ksize);
        else if (src.depth() == HL_32F)
            medianBlur_SortNet<MinMax32f, MinMaxVec32f>(src, dst, ksize);
        else
            HL_Error(HL_StsUnsupportedFormat, "");

        return;
    }
    else
    {
        // TODO AVX guard (external call)
        hl::copyMakeBorder(src0, src, 0, 0, ksize / 2, ksize / 2, BORDER_REPLICATE | BORDER_ISOLATED);

        int cn = src0.channels();
        HL_Assert(src.depth() == HL_8U && (cn == 1 || cn == 3 || cn == 4));

        double img_size_mp = (double)(src0.total()) / (1 << 20);
        if (ksize <= 3 + (img_size_mp < 1 ? 12 : img_size_mp < 4 ? 6
                                                                 : 2)
                             * 3)
            medianBlur_8u_Om(src, dst, ksize);
        else
            medianBlur_8u_O1(src, dst, ksize);
    }
}
}    // namespace cpu_baseline
}    // namespace hl
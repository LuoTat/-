#include "precomp.hxx"

namespace hl
{

static void flipHoriz(const uchar* src, size_t sstep, uchar* dst, size_t dstep, Size size, size_t esz)
{
    int             i, j, limit = (int)(((size.width + 1) / 2) * esz);
    AutoBuffer<int> _tab(size.width * esz);
    int*            tab = _tab.data();

    for (i = 0; i < size.width; i++)
        for (size_t k = 0; k < esz; k++)
            tab[i * esz + k] = (int)((size.width - i - 1) * esz + k);

    for (; size.height--; src += sstep, dst += dstep)
    {
        for (i = 0; i < limit; i++)
        {
            j        = tab[i];
            uchar t0 = src[i], t1 = src[j];
            dst[i] = t1;
            dst[j] = t0;
        }
    }
}

static void flipVert(const uchar* src0, size_t sstep, uchar* dst0, size_t dstep, Size size, size_t esz)
{
    const uchar* src1  = src0 + (size.height - 1) * sstep;
    uchar*       dst1  = dst0 + (size.height - 1) * dstep;
    size.width        *= (int)esz;

    for (int y = 0; y < (size.height + 1) / 2; y++, src0 += sstep, src1 -= sstep, dst0 += dstep, dst1 -= dstep)
    {
        int i = 0;

        if (isAligned<sizeof(int)>(src0, src1, dst0, dst1))
        {
            for (; i <= size.width - 16; i += 16)
            {
                int t0                = ((int*)(src0 + i))[0];
                int t1                = ((int*)(src1 + i))[0];

                ((int*)(dst0 + i))[0] = t1;
                ((int*)(dst1 + i))[0] = t0;

                t0                    = ((int*)(src0 + i))[1];
                t1                    = ((int*)(src1 + i))[1];

                ((int*)(dst0 + i))[1] = t1;
                ((int*)(dst1 + i))[1] = t0;

                t0                    = ((int*)(src0 + i))[2];
                t1                    = ((int*)(src1 + i))[2];

                ((int*)(dst0 + i))[2] = t1;
                ((int*)(dst1 + i))[2] = t0;

                t0                    = ((int*)(src0 + i))[3];
                t1                    = ((int*)(src1 + i))[3];

                ((int*)(dst0 + i))[3] = t1;
                ((int*)(dst1 + i))[3] = t0;
            }

            for (; i <= size.width - 4; i += 4)
            {
                int t0                = ((int*)(src0 + i))[0];
                int t1                = ((int*)(src1 + i))[0];

                ((int*)(dst0 + i))[0] = t1;
                ((int*)(dst1 + i))[0] = t0;
            }
        }

        for (; i < size.width; i++)
        {
            uchar t0 = src0[i];
            uchar t1 = src1[i];

            dst0[i]  = t1;
            dst1[i]  = t0;
        }
    }
}

void flip(const Mat& _src, Mat& _dst, int flip_mode)
{
    HL_Assert(_src.dims <= 2);
    Size size = _src.size();

    if (flip_mode < 0)
    {
        if (size.width == 1)
            flip_mode = 0;
        if (size.height == 1)
            flip_mode = 1;
    }

    if ((size.width == 1 && flip_mode > 0) || (size.height == 1 && flip_mode == 0))
    {
        return _src.copyTo(_dst);
    }

    Mat src  = _src;
    int type = src.type();
    _dst.create(size, type);
    Mat dst    = _dst;

    size_t esz = HL_ELEM_SIZE(type);

    if (flip_mode <= 0)
        flipVert(src.ptr(), src.step, dst.ptr(), dst.step, src.size(), esz);
    else
        flipHoriz(src.ptr(), src.step, dst.ptr(), dst.step, src.size(), esz);

    if (flip_mode < 0)
        flipHoriz(dst.ptr(), dst.step, dst.ptr(), dst.step, dst.size(), esz);
}

}    // namespace hl
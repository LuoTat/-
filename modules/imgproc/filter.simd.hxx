#include "precomp.hxx"
// #include "filter.hxx

#include <cstddef>

namespace hl
{
namespace cpu_baseline
{
// forward declarations
int                   FilterEngine__start(FilterEngine& this_, const Size& _wholeSize, const Size& sz, const Point& ofs);
int                   FilterEngine__proceed(FilterEngine& this_, const uchar* src, int srcstep, int count, uchar* dst, int dststep);
void                  FilterEngine__apply(FilterEngine& this_, const Mat& src, Mat& dst, const Size& wsz, const Point& ofs);
Ptr<BaseRowFilter>    getLinearRowFilter(int srcType, int bufType, const Mat& kernel, int anchor, int symmetryType);
Ptr<BaseColumnFilter> getLinearColumnFilter(int bufType, int dstType, const Mat& kernel, int anchor, int symmetryType, double delta, int bits);
Ptr<BaseFilter>       getLinearFilter(int srcType, int dstType, const Mat& filter_kernel, Point anchor, double delta, int bits);

typedef int HL_DECL_ALIGNED(1) unaligned_int;
#define VEC_ALIGN HL_MALLOC_ALIGN

int FilterEngine__start(FilterEngine& this_, const Size& _wholeSize, const Size& sz, const Point& ofs)
{
    int i, j;

    this_.wholeSize = _wholeSize;
    this_.roi       = Rect(ofs, sz);
    HL_Assert(this_.roi.x >= 0 && this_.roi.y >= 0 && this_.roi.width >= 0 && this_.roi.height >= 0 && this_.roi.x + this_.roi.width <= this_.wholeSize.width && this_.roi.y + this_.roi.height <= this_.wholeSize.height);

    int          esz         = (int)getElemSize(this_.srcType);
    int          bufElemSize = (int)getElemSize(this_.bufType);
    const uchar* constVal    = !this_.constBorderValue.empty() ? &this_.constBorderValue[0] : 0;

    int _maxBufRows          = std::max(this_.ksize.height + 3,
                               std::max(this_.anchor.y,
                                        this_.ksize.height - this_.anchor.y - 1)
                                       * 2
                                   + 1);

    if (this_.maxWidth < this_.roi.width || _maxBufRows != (int)this_.rows.size())
    {
        this_.rows.resize(_maxBufRows);
        this_.maxWidth = std::max(this_.maxWidth, this_.roi.width);
        int cn         = HL_MAT_CN(this_.srcType);
        this_.srcRow.resize(esz * (this_.maxWidth + this_.ksize.width - 1));
        if (this_.columnBorderType == BORDER_CONSTANT)
        {
            HL_Assert(constVal != NULL);
            this_.constBorderRow.resize(getElemSize(this_.bufType) * (this_.maxWidth + this_.ksize.width - 1 + VEC_ALIGN));
            uchar* dst  = alignPtr(&this_.constBorderRow[0], VEC_ALIGN);
            int    n    = (int)this_.constBorderValue.size();
            int    N    = (this_.maxWidth + this_.ksize.width - 1) * esz;
            uchar* tdst = this_.isSeparable() ? &this_.srcRow[0] : dst;

            for (i = 0; i < N; i += n)
            {
                n = std::min(n, N - i);
                for (j = 0; j < n; j++)
                    tdst[i + j] = constVal[j];
            }

            if (this_.isSeparable())
                (*this_.rowFilter)(&this_.srcRow[0], dst, this_.maxWidth, cn);
        }

        int maxBufStep = bufElemSize * (int)alignSize(this_.maxWidth + (!this_.isSeparable() ? this_.ksize.width - 1 : 0), VEC_ALIGN);
        this_.ringBuf.resize(maxBufStep * this_.rows.size() + VEC_ALIGN);
    }

    // adjust bufstep so that the used part of the ring buffer stays compact in memory
    this_.bufStep = bufElemSize * (int)alignSize(this_.roi.width + (!this_.isSeparable() ? this_.ksize.width - 1 : 0), VEC_ALIGN);

    this_.dx1     = std::max(this_.anchor.x - this_.roi.x, 0);
    this_.dx2     = std::max(this_.ksize.width - this_.anchor.x - 1 + this_.roi.x + this_.roi.width - this_.wholeSize.width, 0);

    // recompute border tables
    if (this_.dx1 > 0 || this_.dx2 > 0)
    {
        if (this_.rowBorderType == BORDER_CONSTANT)
        {
            HL_Assert(constVal != NULL);
            int nr = this_.isSeparable() ? 1 : (int)this_.rows.size();
            for (i = 0; i < nr; i++)
            {
                uchar* dst = this_.isSeparable() ? &this_.srcRow[0] : alignPtr(&this_.ringBuf[0], VEC_ALIGN) + this_.bufStep * i;
                memcpy(dst, constVal, this_.dx1 * esz);
                memcpy(dst + (this_.roi.width + this_.ksize.width - 1 - this_.dx2) * esz, constVal, this_.dx2 * esz);
            }
        }
        else
        {
            int xofs1     = std::min(this_.roi.x, this_.anchor.x) - this_.roi.x;

            int  btab_esz = this_.borderElemSize, wholeWidth = this_.wholeSize.width;
            int* btab = (int*)&this_.borderTab[0];

            for (i = 0; i < this_.dx1; i++)
            {
                int p0 = (borderInterpolate(i - this_.dx1, wholeWidth, this_.rowBorderType) + xofs1) * btab_esz;
                for (j = 0; j < btab_esz; j++)
                    btab[i * btab_esz + j] = p0 + j;
            }

            for (i = 0; i < this_.dx2; i++)
            {
                int p0 = (borderInterpolate(wholeWidth + i, wholeWidth, this_.rowBorderType) + xofs1) * btab_esz;
                for (j = 0; j < btab_esz; j++)
                    btab[(i + this_.dx1) * btab_esz + j] = p0 + j;
            }
        }
    }

    this_.rowCount = this_.dstY = 0;
    this_.startY = this_.startY0 = std::max(this_.roi.y - this_.anchor.y, 0);
    this_.endY                   = std::min(this_.roi.y + this_.roi.height + this_.ksize.height - this_.anchor.y - 1, this_.wholeSize.height);

    if (this_.columnFilter)
        this_.columnFilter->reset();
    if (this_.filter2D)
        this_.filter2D->reset();

    return this_.startY;
}

int FilterEngine__proceed(FilterEngine& this_, const uchar* src, int srcstep, int count, uchar* dst, int dststep)
{
    HL_DbgAssert(this_.wholeSize.width > 0 && this_.wholeSize.height > 0);

    const int* btab = &this_.borderTab[0];
    int        esz = (int)getElemSize(this_.srcType), btab_esz = this_.borderElemSize;
    uchar**    brows   = &this_.rows[0];
    int        bufRows = (int)this_.rows.size();
    int        cn      = HL_MAT_CN(this_.bufType);
    int        width = this_.roi.width, kwidth = this_.ksize.width;
    int        kheight = this_.ksize.height, ay = this_.anchor.y;
    int        _dx1 = this_.dx1, _dx2 = this_.dx2;
    int        width1     = this_.roi.width + kwidth - 1;
    int        xofs1      = std::min(this_.roi.x, this_.anchor.x);
    bool       isSep      = this_.isSeparable();
    bool       makeBorder = (_dx1 > 0 || _dx2 > 0) && this_.rowBorderType != BORDER_CONSTANT;
    int        dy = 0, i = 0;

    src   -= xofs1 * esz;
    count  = std::min(count, this_.remainingInputRows());

    HL_Assert(src && dst && count > 0);

    for (;; dst += dststep * i, dy += i)
    {
        int dcount  = bufRows - ay - this_.startY - this_.rowCount + this_.roi.y;
        dcount      = dcount > 0 ? dcount : bufRows - kheight + 1;
        dcount      = std::min(dcount, count);
        count      -= dcount;
        for (; dcount-- > 0; src += srcstep)
        {
            int    bi   = (this_.startY - this_.startY0 + this_.rowCount) % bufRows;
            uchar* brow = alignPtr(&this_.ringBuf[0], VEC_ALIGN) + bi * this_.bufStep;
            uchar* row  = isSep ? &this_.srcRow[0] : brow;

            if (++this_.rowCount > bufRows)
            {
                --this_.rowCount;
                ++this_.startY;
            }

            memcpy(row + _dx1 * esz, src, (width1 - _dx2 - _dx1) * esz);

            if (makeBorder)
            {
                if (btab_esz * (int)sizeof(int) == esz)
                {
                    const int* isrc = (const int*)src;
                    int*       irow = (int*)row;

                    for (i = 0; i < _dx1 * btab_esz; i++)
                        irow[i] = isrc[btab[i]];
                    for (i = 0; i < _dx2 * btab_esz; i++)
                        irow[i + (width1 - _dx2) * btab_esz] = isrc[btab[i + _dx1 * btab_esz]];
                }
                else
                {
                    for (i = 0; i < _dx1 * esz; i++)
                        row[i] = src[btab[i]];
                    for (i = 0; i < _dx2 * esz; i++)
                        row[i + (width1 - _dx2) * esz] = src[btab[i + _dx1 * esz]];
                }
            }

            if (isSep)
                (*this_.rowFilter)(row, brow, width, HL_MAT_CN(this_.srcType));
        }

        int max_i = std::min(bufRows, this_.roi.height - (this_.dstY + dy) + (kheight - 1));
        for (i = 0; i < max_i; i++)
        {
            int srcY = borderInterpolate(this_.dstY + dy + i + this_.roi.y - ay,
                                         this_.wholeSize.height,
                                         this_.columnBorderType);
            if (srcY < 0)    // can happen only with constant border type
                brows[i] = alignPtr(&this_.constBorderRow[0], VEC_ALIGN);
            else
            {
                HL_Assert(srcY >= this_.startY);
                if (srcY >= this_.startY + this_.rowCount)
                    break;
                int bi   = (srcY - this_.startY0) % bufRows;
                brows[i] = alignPtr(&this_.ringBuf[0], VEC_ALIGN) + bi * this_.bufStep;
            }
        }
        if (i < kheight)
            break;
        i -= kheight - 1;
        if (isSep)
            (*this_.columnFilter)((const uchar**)brows, dst, dststep, i, this_.roi.width* cn);
        else
            (*this_.filter2D)((const uchar**)brows, dst, dststep, i, this_.roi.width, cn);
    }

    this_.dstY += dy;
    HL_Assert(this_.dstY <= this_.roi.height);
    return dy;
}

void FilterEngine__apply(FilterEngine& this_, const Mat& src, Mat& dst, const Size& wsz, const Point& ofs)
{
    HL_DbgAssert(src.type() == this_.srcType && dst.type() == this_.dstType);

    FilterEngine__start(this_, wsz, src.size(), ofs);
    int y = this_.startY - ofs.y;
    FilterEngine__proceed(this_,
                          src.ptr() + y * (ptrdiff_t)src.step,
                          (int)src.step,
                          this_.endY - this_.startY,
                          dst.ptr(),
                          (int)dst.step);
}



}    // namespace cpu_baseline
}    // namespace hl
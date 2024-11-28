#pragma once

#include "grfmt_base.hxx"

namespace hl
{

enum BmpCompression
{
    BMP_RGB       = 0,
    BMP_RLE8      = 1,
    BMP_RLE4      = 2,
    BMP_BITFIELDS = 3
};

// Windows Bitmap reader
class BmpDecoder final: public BaseImageDecoder
{
public:
    BmpDecoder();
    ~BmpDecoder() override;

    bool readData(Mat& img) override;
    bool readHeader() override;
    void close();

    ImageDecoder newDecoder() const override;

protected:
    void initMask();
    void maskBGRA(uchar* des, const uchar* src, int num, bool alpha_required);
    void maskBGRAtoGray(uchar* des, const uchar* src, int num);

    enum Origin
    {
        ORIGIN_TL = 0,
        ORIGIN_BL = 1
    };

    RLByteStream   m_strm;
    PaletteEntry   m_palette[256];
    Origin         m_origin;
    int            m_bpp;
    int            m_offset;
    BmpCompression m_rle_code;
    uint           m_rgba_mask[4];
    int            m_rgba_bit_offset[4];
    float          m_rgba_scale_factor[4];
};

// ... writer
class BmpEncoder final: public BaseImageEncoder
{
public:
    BmpEncoder();
    ~BmpEncoder() override;

    bool write(const Mat& img, const std::vector<int>& params) override;

    ImageEncoder newEncoder() const override;
};

}    // namespace hl

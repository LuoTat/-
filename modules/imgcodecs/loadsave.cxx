#include "precomp.hxx"
#include "grfmts.hxx"
#include "utils.hxx"

#include <iostream>
#include <fstream>
#include <cerrno>
#include "openHL/core/utils/logger.hxx"
#include "openHL/imgcodecs.hxx"

namespace hl
{

const static size_t HL_IO_MAX_IMAGE_PARAMS = 50;
const static size_t HL_IO_MAX_IMAGE_WIDTH  = 1 << 20;
const static size_t HL_IO_MAX_IMAGE_HEIGHT = 1 << 20;
const static size_t HL_IO_MAX_IMAGE_PIXELS = 1 << 30;

static Size validateInputImageSize(const Size& size)
{
    HL_Assert(size.width > 0);
    HL_Assert(static_cast<size_t>(size.width) <= HL_IO_MAX_IMAGE_WIDTH);
    HL_Assert(size.height > 0);
    HL_Assert(static_cast<size_t>(size.height) <= HL_IO_MAX_IMAGE_HEIGHT);
    uint64 pixels = (uint64)size.width * (uint64)size.height;
    HL_Assert(pixels <= HL_IO_MAX_IMAGE_PIXELS);
    return size;
}

struct ImageCodecInitializer
{
    ImageCodecInitializer()
    {
        decoders.push_back(makePtr<BmpDecoder>());
        encoders.push_back(makePtr<BmpEncoder>());
    }

    std::vector<ImageDecoder> decoders;
    std::vector<ImageEncoder> encoders;
};

static ImageCodecInitializer& getCodecs()
{
    static ImageCodecInitializer g_codecs;
    return g_codecs;
}

static ImageDecoder findDecoder(const String& filename)
{
    size_t i, maxlen = 0;

    ImageCodecInitializer& codecs = getCodecs();
    for (i = 0; i < codecs.decoders.size(); i++)
    {
        size_t len = codecs.decoders[i]->signatureLength();
        maxlen     = std::max(maxlen, len);
    }

    FILE* f = fopen(filename.c_str(), "rb");

    if (!f)
    {
        HL_LOG_WARNING("WARNING", std::format("findDecoder('{0}'): can't open file for reading", filename));
        return ImageDecoder();
    }

    String signature(maxlen, ' ');
    maxlen = fread((void*)signature.c_str(), 1, maxlen, f);
    fclose(f);
    signature = signature.substr(0, maxlen);

    for (i = 0; i < codecs.decoders.size(); i++)
    {
        if (codecs.decoders[i]->checkSignature(signature))
            return codecs.decoders[i]->newDecoder();
    }

    return ImageDecoder();
}

static ImageEncoder findEncoder(const String& _ext)
{
    if (_ext.size() <= 1)
        return ImageEncoder();

    const char* ext = strrchr(_ext.c_str(), '.');
    if (!ext)
        return ImageEncoder();
    int len = 0;
    for (ext++; len < 128 && isalnum(ext[len]); len++);

    ImageCodecInitializer& codecs = getCodecs();
    for (size_t i = 0; i < codecs.encoders.size(); i++)
    {
        String      description = codecs.encoders[i]->getDescription();
        const char* descr       = strchr(description.c_str(), '(');

        while (descr)
        {
            descr = strchr(descr + 1, '.');
            if (!descr)
                break;
            int j = 0;
            for (descr++; j < len && isalnum(descr[j]); j++)
            {
                int c1 = tolower(ext[j]);
                int c2 = tolower(descr[j]);
                if (c1 != c2)
                    break;
            }
            if (j == len && !isalnum(descr[j]))
                return codecs.encoders[i]->newEncoder();
            descr += j;
        }
    }

    return ImageEncoder();
}

static bool imread_(const String& filename, int flags, Mat& mat)
{
    ImageDecoder decoder;
    decoder = findDecoder(filename);

    if (!decoder)
    {
        return 0;
    }

    int scale_denom = 1;
    if (flags > IMREAD_LOAD_GDAL)
    {
        if (flags & IMREAD_REDUCED_GRAYSCALE_2)
            scale_denom = 2;
        else if (flags & IMREAD_REDUCED_GRAYSCALE_4)
            scale_denom = 4;
        else if (flags & IMREAD_REDUCED_GRAYSCALE_8)
            scale_denom = 8;
    }

    decoder->setScale(scale_denom);

    decoder->setSource(filename);

    try
    {
        if (!decoder->readHeader())
            return 0;
    }
    catch (const hl::Exception& e)
    {
        HL_LOG_ERROR("ERROR", std::format("imread_('{0}'): can't read header: {1}", filename, e.what()));
        return 0;
    }
    catch (...)
    {
        HL_LOG_ERROR("ERROR", std::format("imread_('{0}'): can't read header: unknown exception", filename));
        return 0;
    }

    Size size = validateInputImageSize(Size(decoder->width(), decoder->height()));

    int type  = decoder->type();
    if ((flags & IMREAD_LOAD_GDAL) != IMREAD_LOAD_GDAL && flags != IMREAD_UNCHANGED)
    {
        if ((flags & IMREAD_ANYDEPTH) == 0)
            type = HL_MAKETYPE(HL_8U, HL_MAT_CN(type));

        if ((flags & IMREAD_COLOR) != 0 || ((flags & IMREAD_ANYCOLOR) != 0 && HL_MAT_CN(type) > 1))
            type = HL_MAKETYPE(HL_MAT_DEPTH(type), 3);
        else
            type = HL_MAKETYPE(HL_MAT_DEPTH(type), 1);
    }

    mat.create(size.height, size.width, type);

    bool success = false;
    try
    {
        if (decoder->readData(mat))
            success = true;
    }
    catch (const hl::Exception& e)
    {
        HL_LOG_ERROR("ERROR", std::format("imread_('{0}'): can't read data: {1}", filename, e.what()));
    }
    catch (...)
    {
        HL_LOG_ERROR("ERROR", std::format("imread_('{0}'): can't read data: unknown exception", filename));
    }
    if (!success)
    {
        mat.release();
        return false;
    }

    if (decoder->setScale(scale_denom) > 1)
    {
        resize(mat, mat, Size(size.width / scale_denom, size.height / scale_denom), 0, 0, INTER_LINEAR_EXACT);
    }

    return true;
}

Mat imread(const String& filename, int flags)
{
    Mat img;
    imread_(filename, flags, img);
    return img;
}

static bool imwrite_(const String& filename, const Mat& _img, const std::vector<int>& params_, bool flipv)
{
    ImageEncoder encoder = findEncoder(filename);
    if (!encoder)
        HL_Error(Error::StsError, "could not find a writer for the specified extension");

    Mat image = _img;
    HL_Assert(!image.empty());

    HL_Assert(image.channels() == 1 || image.channels() == 3 || image.channels() == 4);

    Mat temp;
    if (!encoder->isFormatSupported(image.depth()))
    {
        HL_Assert(encoder->isFormatSupported(HL_8U));
        image.convertTo(temp, HL_8U);
        image = temp;
    }

    if (flipv)
    {
        // flip(image, temp, 0);
        image = temp;
    }
    encoder->setDestination(filename);

    HL_Check((params_.size() & 1) == 0, "Encoding 'params' must be key-value pairs");
    HL_CheckLE(params_.size(), (size_t)(HL_IO_MAX_IMAGE_PARAMS * 2), "");
    bool code = false;
    try
    {
        code = encoder->write(image, params_);

        if (!code)
        {
            FILE* f = fopen(filename.c_str(), "wb");
            if (!f)
            {
                if (errno == EACCES)
                {
                    HL_LOG_WARNING("WARNING", std::format("imwrite_('{0}'): can't open file for writing: permission denied", filename));
                }
            }
            else
            {
                fclose(f);
                remove(filename.c_str());
            }
        }
    }
    catch (const hl::Exception& e)
    {
        HL_LOG_ERROR("ERROR", std::format("imwrite_('{0}'): can't write data: {1}", filename, e.what()));
    }
    catch (...)
    {
        HL_LOG_ERROR("ERROR", std::format("imwrite_('{0}'): can't write data: unknown exception", filename));
    }

    return code;
}

bool imwrite(const String& filename, const Mat& _img, const std::vector<int>& params)
{
    HL_Assert(!_img.empty());
    return imwrite_(filename, _img, params, false);
}

}    // namespace hl
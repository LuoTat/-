#include "precomp.hxx"
#include "grfmt_base.hxx"
#include "bitstrm.hxx"

namespace hl
{

BaseImageDecoder::BaseImageDecoder()
{
    m_width = m_height = 0;
    m_type             = -1;
    m_buf_supported    = false;
    m_scale_denom      = 1;
}

bool BaseImageDecoder::setSource(const String& filename)
{
    m_filename = filename;
    m_buf.release();
    return true;
}

bool BaseImageDecoder::setSource(const Mat& buf)
{
    if (!m_buf_supported)
        return false;
    m_filename = String();
    m_buf      = buf;
    return true;
}

size_t BaseImageDecoder::signatureLength() const
{
    return m_signature.size();
}

bool BaseImageDecoder::checkSignature(const String& signature) const
{
    size_t len = signatureLength();
    return signature.size() >= len && memcmp(signature.c_str(), m_signature.c_str(), len) == 0;
}

int BaseImageDecoder::setScale(const int& scale_denom)
{
    int temp      = m_scale_denom;
    m_scale_denom = scale_denom;
    return temp;
}

ImageDecoder BaseImageDecoder::newDecoder() const
{
    return ImageDecoder();
}

BaseImageEncoder::BaseImageEncoder()
{
    m_buf           = 0;
    m_buf_supported = false;
}

bool BaseImageEncoder::isFormatSupported(int depth) const
{
    return depth == HL_8U;
}

String BaseImageEncoder::getDescription() const
{
    return m_description;
}

bool BaseImageEncoder::setDestination(const String& filename)
{
    m_filename = filename;
    m_buf      = 0;
    return true;
}

bool BaseImageEncoder::setDestination(std::vector<uchar>& buf)
{
    if (!m_buf_supported)
        return false;
    m_buf = &buf;
    m_buf->clear();
    m_filename = String();
    return true;
}

bool BaseImageEncoder::writemulti(const std::vector<Mat>&, const std::vector<int>&)
{
    return false;
}

ImageEncoder BaseImageEncoder::newEncoder() const
{
    return ImageEncoder();
}

void BaseImageEncoder::throwOnEror() const
{
    if (!m_last_error.empty())
    {
        String msg = "Raw image encoder error: " + m_last_error;
        HL_Error(Error::BadImageSize, msg.c_str());
    }
}

}    // namespace hl

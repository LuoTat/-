#include "BmpDecoder.h"
#include "BmpEncoder.h"
#include "imgcodecs.h"

void imread_(const char* filename, Mat* img)
{
    BmpDecoder decoder = initBmpDecoder();
    setSource(&decoder, filename);
    readHeader(&decoder);
    *img = createMat(decoder.height, decoder.width, decoder.bpp >> 3, sizeof(unsigned char));
    readData(&decoder, img);
    close(&decoder);
}

Mat imread(const char* filename)
{
    Mat img;
    imread_(filename, &img);
    return img;
}

void imwrite(const char* filename, const Mat* img)
{
    BmpEncoder encoder;
    setDestination(&encoder, filename);
    write(&encoder, img);
}
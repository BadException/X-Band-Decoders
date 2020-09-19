#include "msu_reader.h"

MSUReader::MSUReader()
{
    imageBuffer = new unsigned short[100000 * 12008];
    frames = 0;
}

MSUReader::~MSUReader()
{
    delete[] imageBuffer;
}

void MSUReader::pushFrame(uint8_t *data)
{
    // Offset to start reading from
    int pos = 5 * 38;

    // Convert to 10 bits values
    for (int i = 0; i < 12044; i += 4)
    {
        msuLineBuffer[i] = (data[pos + 0] << 2) | (data[pos + 1] >> 6);
        msuLineBuffer[i + 1] = ((data[pos + 1] % 64) << 4) | (data[pos + 2] >> 4);
        msuLineBuffer[i + 2] = ((data[pos + 2] % 16) << 6) | (data[pos + 3] >> 2);
        msuLineBuffer[i + 3] = ((data[pos + 3] % 4) << 8) | data[pos + 4];
        pos += 5;
    }

    // Deinterleave and load into our image buffer
    for (int i = 0; i < 6004; i++)
    {
        uint16_t pixel = msuLineBuffer[i * 2];
        uint16_t pixel2 = msuLineBuffer[i * 2 + 1];
        imageBuffer[frames * 12008 + i] = pixel * 60;
        imageBuffer[frames * 12008 + 6000 + i] = pixel2 * 60;
    }

    frames++;
}

cimg_library::CImg<unsigned short> MSUReader::getImage()
{
    return cimg_library::CImg<unsigned short>(&imageBuffer[0], 12008, frames);
}

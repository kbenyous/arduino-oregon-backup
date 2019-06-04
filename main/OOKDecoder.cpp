#include "OOKDecoder.h"
#include "Arduino.h"

OOKDecoder::OOKDecoder()
{
    resetDecoder();
}

bool OOKDecoder::nextPulse(word width)
{
    if (state != DONE)

        switch (decode(width))
        {
        case -1:
            resetDecoder();
            break;
        case 1:
            done();
            break;
        }
    return isDone();
}

bool OOKDecoder::isDone() const { return state == DONE; }

const byte *OOKDecoder::getData(byte &count) const
{
    count = pos;
    return data;
}

void OOKDecoder::resetDecoder()
{
    total_bits = bits = pos = flip = 0;
    state = UNKNOWN;
}

// add one bit to the packet data buffer

void OOKDecoder::gotBit(char value)
{
    total_bits++;
    byte *ptr = data + pos;
    *ptr = (*ptr >> 1) | (value << 7);

    if (++bits >= 8)
    {
        bits = 0;
        if (++pos >= sizeof data)
        {
            resetDecoder();
            return;
        }
    }
    state = OK;
}

// store a bit using Manchester encoding
void OOKDecoder::manchester(char value)
{
    flip ^= value; // manchester code, long pulse flips the bit
    gotBit(flip);
}

// move bits to the front so that all the bits are aligned to the end
void OOKDecoder::alignTail(byte max = 0)
{
    // align bits
    if (bits != 0)
    {
        data[pos] >>= 8 - bits;
        for (byte i = 0; i < pos; ++i)
            data[i] = (data[i] >> bits) | (data[i + 1] << (8 - bits));
        bits = 0;
    }
    // optionally shift bytes down if there are too many of 'em
    if (max > 0 && pos > max)
    {
        byte n = pos - max;
        pos = max;
        for (byte i = 0; i < pos; ++i)
            data[i] = data[i + n];
    }
}

void OOKDecoder::reverseBits()
{
    for (byte i = 0; i < pos; ++i)
    {
        byte b = data[i];
        for (byte j = 0; j < 8; ++j)
        {
            data[i] = (data[i] << 1) | (b & 1);
            b >>= 1;
        }
    }
}

void OOKDecoder::reverseNibbles()
{
    for (byte i = 0; i < pos; ++i)
        data[i] = (data[i] << 4) | (data[i] >> 4);
}

void OOKDecoder::done()
{
    while (bits)
        gotBit(0); // padding
    state = DONE;
}

#ifndef Oregon_h
#define Oregon_h
#include "Arduino.h"

class OOKDecoder
{
protected:
    byte total_bits, bits, flip, state, pos, data[25];

    virtual char decode(word width) = 0;

public:
    enum
    {
        UNKNOWN,
        T0,
        T1,
        T2,
        T3,
        OK,
        DONE
    };

    OOKDecoder();

    bool nextPulse(word width);

    bool isDone() const;

    const byte *getData(byte &count) const;

    void resetDecoder();

    virtual void gotBit(char value);

    void manchester(char value);

    void alignTail(byte max = 0);

    void reverseBits();

    void reverseNibbles();

    void done();
};

#endif
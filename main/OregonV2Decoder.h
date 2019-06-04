#include "OOKDecoder.h"


class OregonV2Decoder : public OOKDecoder {
  public:

    OregonV2Decoder();

    // add one bit to the packet data buffer
    virtual void gotBit (char value) ;

    virtual char decode (word width) ;
};
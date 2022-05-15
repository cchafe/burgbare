#ifndef AUDIOINTERFACE_H
#define AUDIOINTERFACE_H

#include <QtGlobal>  //For QT4 types
typedef float sample_t;
typedef qint8 int8_t;

class AudioInterface
{
public:
    AudioInterface();
    enum audioBitResolutionT {
        BIT8  = 1,  ///< 8 bits
        BIT16 = 2,  ///< 16 bits (default)
        BIT24 = 3,  ///< 24 bits
        BIT32 = 4   ///< 32 bits
    };

    static void fromSampleToBitConversion(
        const sample_t* const input, int8_t* output,
        const AudioInterface::audioBitResolutionT targetBitResolution);
    static void fromBitToSampleConversion(
        const int8_t* const input, sample_t* output,
        const AudioInterface::audioBitResolutionT sourceBitResolution);
};

#endif // AUDIOINTERFACE_H

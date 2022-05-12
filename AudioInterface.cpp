#include "AudioInterface.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <cstring>
AudioInterface::AudioInterface()
{

}
void AudioInterface::fromSampleToBitConversion(
    const sample_t* const input, int8_t* output,
    const AudioInterface::audioBitResolutionT targetBitResolution)
{
    int8_t tmp_8;
    uint8_t tmp_u8;  // unsigned to quantize the remainder in 24bits
    int16_t tmp_16;
    double tmp_sample;
    sample_t tmp_sample16;
    sample_t tmp_sample8;
    switch (targetBitResolution) {
    case BIT8:
        // 8bit integer between -128 to 127
        tmp_sample =
            std::max(-127.0, std::min(127.0, std::round((*input) * 127.0)));  // 2^7 = 128
        tmp_8 = static_cast<int8_t>(tmp_sample);
        std::memcpy(output, &tmp_8, 1);  // 8bits = 1 bytes
        break;
    case BIT16:
        // 16bit integer between -32768 to 32767
        // original scaling: tmp_sample = floor( (*input) * 32768.0 ); // 2^15 = 32768.0
        tmp_sample = std::max(
            -32767.0, std::min(32767.0, std::round((*input) * 32767.0)));  // 2^15 = 32768
        tmp_16 = static_cast<int16_t>(tmp_sample);
        std::memcpy(
            output, &tmp_16,
            2);  // 2 bytes output in Little Endian order (LSB -> smallest address)
        break;
    case BIT24:
        // To convert to 24 bits, we first quantize the number to 16bit
        tmp_sample   = (*input) * 32768.0;  // 2^15 = 32768.0
        tmp_sample16 = floor(tmp_sample);
        tmp_16       = static_cast<int16_t>(tmp_sample16);

        // Then we compute the remainder error, and quantize that part into an 8bit number
        // Note that this remainder is always positive, so we use an unsigned integer
        tmp_sample8 = floor(
            (tmp_sample - tmp_sample16)  // this is a positive number, between 0.0-1.0
            * 256.0);
        tmp_u8 = static_cast<uint8_t>(tmp_sample8);

        // Finally, we copy the 16bit number in the first 2 bytes,
        // and the 8bit number in the third bite
        std::memcpy(output, &tmp_16, 2);      // 16bits = 2 bytes
        std::memcpy(output + 2, &tmp_u8, 1);  // 8bits = 1 bytes
        break;
    case BIT32:
        tmp_sample = *input;
        // not necessary yet:
        // tmp_sample = std::max(-1.0, std::min(1.0, tmp_sample));
        std::memcpy(output, &tmp_sample, 4);  // 32bit = 4 bytes
        break;
    }
}

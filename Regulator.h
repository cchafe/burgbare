#ifndef REGULATOR_H
#define REGULATOR_H

#include "burgalgorithm.h"

typedef float sample_t;

class ChanData
{
   public:
    ChanData(int i, int FPP, int hist);
    int ch;
    int trainSamps;
    std::vector<sample_t> mTruth;
    std::vector<sample_t> mTrain;
    std::vector<sample_t> mTail;
    std::vector<sample_t> mPrediction;  // ORDER
    std::vector<long double> mCoeffs;
    std::vector<sample_t> mXfadedPred;
    std::vector<sample_t> mLastPred;
    std::vector<std::vector<sample_t>> mLastPackets;
    std::vector<sample_t> mCrossFadeDown;
    std::vector<sample_t> mCrossFadeUp;
    std::vector<sample_t> mCrossfade;
};

class Regulator
{
public:
    Regulator(int rcvChannels, int bit_res, int FPP, int hist);
    virtual ~Regulator();
    BurgAlgorithm ba;
private:
    int mNumChannels;
    int mAudioBitRes;
    int mFPP;
    int mHist;
    double mMsecTolerance;
    std::vector<ChanData*> mChanData;
};

#endif // REGULATOR_H

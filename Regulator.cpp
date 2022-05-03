#include "Regulator.h"

Regulator::Regulator(int rcvChannels, int bit_res, int FPP, int hist) :
    mNumChannels(rcvChannels)
    , mAudioBitRes(bit_res)
  , mFPP(FPP)
  , mHist(hist)
{
    for (int i = 0; i < mNumChannels; i++) {
        ChanData* tmp = new ChanData(i, mFPP, mHist);
        mChanData.push_back(tmp);
//        for (int s = 0; s < mFPP; s++)
//            sampleToBits(0.0, i, s);  // zero all channels in mXfrBuffer
    }

};

Regulator::~Regulator()
{
}

ChanData::ChanData(int i, int FPP, int hist) : ch(i)
{
    trainSamps = (hist * FPP);
    mTruth.resize(FPP, 0.0);
    mXfadedPred.resize(FPP, 0.0);
    mLastPred.resize(FPP, 0.0);
    for (int i = 0; i < hist; i++) {
        std::vector<sample_t> tmp(FPP, 0.0);
        mLastPackets.push_back(tmp);
    }
    mTrain.resize(trainSamps, 0.0);
    mPrediction.resize(trainSamps - 1, 0.0);  // ORDER
    mCoeffs.resize(trainSamps - 2, 0.0);
    mCrossFadeDown.resize(FPP, 0.0);
    mCrossFadeUp.resize(FPP, 0.0);
    mCrossfade.resize(FPP, 0.0);
}

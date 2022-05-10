#ifndef WAVFILEIO_H
#define WAVFILEIO_H
#include <vector>
#include <QString>
#include <stk/FileWvIn.h>
#include <stk/FileWvOut.h>
using namespace std;


class WavFileIO
{
public:
    WavFileIO();
    stk::FileWvIn wavIn; // read incrementally so that normalization is ok
    void openIFile(QString ifn, int sk);
    void readFramesFromFor(int start, int ln, double g);
    void readFrames(QString ifn, int sk, int ln, double g);
    void writeFrames(QString ofn, vector<vector<float> > oframes);
    vector<float> iframes;
    stk::StkFrames stkframe;
    QString iwavFilename;
    QString owavFilename;
    int skip;
    int length;
    double gain;
};

#endif // WAVFILEIO_H

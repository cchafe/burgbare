#include "wavfileio.h"
#include <QString>
#include <QDebug>

WavFileIO::WavFileIO()
{
}

void WavFileIO::openIFile(QString ifn, int sk)
{
    iwavFilename = ifn;
    std::string tmp = iwavFilename.toStdString();
    wavIn.openFile( tmp, false,false,true );
    //    qDebug() << iwavFilename << " is " << (double)wavIn.getSize()/(double)wavIn.getFileRate()
    //            << "seconds long" << "\t" << (double)wavIn.getSize() << "samples";
    //     qDebug() << iwavFilename << " is " << wavIn.channelsOut() << "channels";
    skip = sk;
}

void WavFileIO::readFramesFromFor(int start, int ln, double g)
{
    wavIn.reset();
    wavIn.addTime(start+skip);
    length = ln;
    gain = g;
    iframes.resize(length);
    for ( size_t i = 0; i < iframes.size(); i++ )
        iframes.at(i) = gain * wavIn.tick();
    //    qDebug() << "iframes samples" << iframes.size();
}

void WavFileIO::writeFrames(QString ofn, vector<vector<float> > ofv)
//void WavFileIO::writeFrames(QString ofn, vector<double> ofv)
{
    qDebug() << "output.size()" << ofv.size() << "x" << ofv.at(0).size();
    owavFilename = ofn;
    std::string tmp = owavFilename.toStdString();
    stk::FileWvOut wavOut;
//#define nO 2
    stkframe.resize(1,ofv.size());
    wavOut.openFile( tmp,
                     ofv.size(), stk::FileWrite::FILE_WAV, stk::Stk::STK_SINT16 );
    for ( size_t i = 0; i < ofv.at(0).size(); i++ ) {
        for ( size_t j = 0; j < ofv.size(); j++ ) {
            stkframe(0,j) = ofv[j][i];
        }
        wavOut.tick(stkframe);
    }
    qDebug() << "wrote " << ofv.at(0).size() << " frames to " << owavFilename;
}

// https://c.mql5.com/3/133/Tutorial_on_Burg_smethod_algorithm_recursion.pdf

#include <QCoreApplication>
#include "burgalgorithm.h"
#include <iostream>
#include <QLoggingCategory>
#include <stk/Stk.h>
#include "wavfileio.h"
using namespace stk;

#define FS 48000.0
QStringList runType = { "orig", "loss", "zero", "fade", "pred", "pure", "resi" };
enum Param { dummy, p_ifile, p_skip, p_fpp, p_run, p_hist, p_plen,
             p_rate, p_off, p_cons, p_stoc, p_ofile, p_lfile };
int LOST[] = { 0, 2, 15, 16, 24, 26, 69, 70, 71, 72, 95, 97, 99, 344, 465, 466, 467, 644, 645, 702, 703, 704, 722, 723, 724, 872, 873, 980, 1110, 1111, 1112, 1321, 1322, 1407, 1408, 1409, 1411, 1415, 1416, 1417, 1418, 1465, 1477, 1479, 1531, 1532, 1533, 1534, 1710, 1719, 1720, 1721, 1722, 1724, 2060, 2061, 2062, 2063, 2114, 2115, 2116, 2235, 2236, 2237, 2238, 2239, 2240, 2241, 2249, 2250, 2251, 2273, 2390, 2391, 2392, 2393, 2399, 2408, 2600, 2601, 2602, 2635, 2688, 2689, 2849, 2850, 2869, 2870, 2871, 2872, 2919, 2920, 3103, 3104, 3269, 3270, 3308, 3309, 3343, 3344, 3345, 3346, 3404, 3405, 3406, 3438, 3439, 3472, 3488, 3489, 3558, 3559, 3560, 3722, 3723, 3830, 3831, 4034, 4044, 4045, 4115, 4116, 4117, 4118, 4190, 4191, 4192, 4194, 4322, 4323, 4352, 4353, 4359, 4956, 4957, 5206, 5207, 5759, 5845, 5848, 5849, 5866, 6006, 6007, 6039, 6040, 6103, 6104, 6148, 6149, 6175, 6176, 6177, 6179 };
int LOSTLEN = 154;
///////////////////////////////////////////////////////

// packets
#define FPP 128
#define PLEN 800// 8000

// run and AR settings
#define RUN 3 // output predicted
#define HIST 2
#define EPOCHS 2
// will have 2 epochs: the past for training, and future predicted
#define PACKETSLOST 1

// impairments
#define RATE 100
#define OFF 0
#define CONS 1
#define STOC false

// parameters
QString idir = "/tmp/";
QString shortIName = "input";
QString odir = "/tmp/";
QString shortOName = "test";
QString dotWav (".wav");
int fpp = FPP;
int hist = HIST;
int run = RUN;
int plen = PLEN;
int rate = RATE;
int off = OFF;
int cons = CONS;
int stoc = STOC;
int skip = 0;

// dependencies
#define IFILENAME (idir + shortIName + dotWav)
#define OFILENAME (odir + shortOName + (runType[run]) + dotWav)
#define LOSSFILENAME (ldir + shortLName + ".dat")
#define PLOTFILENAME (odir + (runType[run]) + ".dat")
#define TRAINSAMPS (hist*fpp)
#define ORDER (TRAINSAMPS-1)
#define SKIP(x) (x-TRAINSAMPS)
#define TOTALSAMPS (fpp*plen)

///////////////////////////////////////////////////////

void qtMessageHandler([[maybe_unused]] QtMsgType type,
[[maybe_unused]] const QMessageLogContext& context,
const QString& msg)
{
    std::cerr << msg.toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=true"));
    Stk::setSampleRate(FS);
    qInstallMessageHandler(qtMessageHandler);
    qDebug() << "preloaded" << LOSTLEN << "packet numbers (from recording):" << LOST[0] << ":" << LOST[LOSTLEN-1];
    qDebug() << (runType[run]);

    for ( int i = 1; i < argc; i++ ) {
        QString tmpStr = QString::fromStdString(argv[i]);
        switch(i)
        {
        case p_ifile  : shortIName = tmpStr;  break;
        case p_skip  : skip = tmpStr.toInt();  break;
        case p_fpp  : fpp = tmpStr.toInt();  break;
        case p_run  : run = tmpStr.toInt();  break;
        case p_hist  : hist = tmpStr.toInt();  break;
        case p_plen  : plen = tmpStr.toInt();  break;
        case p_rate  : rate = tmpStr.toInt();  break;
        case p_off  : off = tmpStr.toInt();  break;
        case p_cons  : cons = tmpStr.toInt();  break;
        case p_stoc  : stoc = tmpStr.toInt();  break;
        case p_ofile  : shortOName = tmpStr;  break;
            //        case p_lfile  : shortLName = tmpStr;  break;
        }
    }

#define DPY(x,y) << x << y
    qDebug() << shortIName
                DPY("skip",skip)
                DPY("fpp",fpp)
                DPY("run",run)
                DPY("hist",hist)
                DPY("plen",plen)
                DPY("rate",rate)
                DPY("off",off)
                DPY("cons",cons)
                DPY("stoc",stoc)
                //             << shortOName << shortLName
                ;

    if (hist >= rate)
        qDebug() << "!!!!!!!!!!! ------------- hist >= rate" << hist << rate;
    WavFileIO iwv;
//    iwv.openIFile(IFILENAME, (skip<TRAINSAMPS)?TRAINSAMPS:skip);

    WavFileIO owv;
    //    FileIO log;
    //    log.openStream("/tmp/error.dat");
    //    log.openStream("/tmp/coeffs.dat");

    //    FileIO lossFile;
    //    lossFile.openReadStream("recordedLost.dat");
    //    qDebug() << lossFile.readInt();
    BurgAlgorithm ba;
    vector<vector<float>> output(1,vector<float>( TOTALSAMPS,0.0));
    qDebug() << "output.size()" << output.size() << "x" << output.at(0).size();
    //    vector<double> output( TOTALSAMPS, 0.0 );
    vector<float> train( TRAINSAMPS, 0.0 );
    vector<float> prediction( TRAINSAMPS-1 ); // ORDER
    vector<long double> coeffs( TRAINSAMPS-2, 0.0 );
    vector<float> truth( fpp, 0.0 );
    vector<float> xfadedPred( fpp, 0.0 );
    vector<float> nextPred( fpp, 0.0 );
    vector<float> lastGoodPacket( fpp, 0.0 );
    vector<vector<float>> lastPackets;
    for ( int i = 0; i < hist; i++ ) {
        vector<float> tmp( fpp, 0.0 );
        lastPackets.push_back(tmp);
    }
    vector<float> fadeUp( fpp, 0.0 );
    vector<float> fadeDown( fpp, 0.0 );
    for ( int i = 0; i < fpp; i++ ) {
        fadeUp[i] = (double)i/(double)fpp;
        fadeDown[i] = 1.0 - fadeUp[i];
    }
    double phasor = 0.0;
    //    Plot plot(PLOTFILENAME);
    //    Loss loss(LOSSFILENAME);
    //    int lostPacket = loss.read();
    bool glitch = false;
    bool lastWasGlitch = glitch;
#define THISPACKET (pCnt*fpp)

#define PACKETSAMP ( int s = 0; s < fpp; s++ )
    int glitchCnt = 0;
    int accCons = 0;
    int maxCons = 0;
    int lostPtr = 0;
    int nextLost = LOST[lostPtr++];
    for ( int pCnt = 0; pCnt < plen; pCnt++) {
        //        qDebug() << "pCnt" << pCnt;
        //        double mSimulatedLossRate;
        //        double mSimulatedJitterRate;
        //        double mSimulatedJitterMaxDelay;

//        iwv.readFramesFromFor(THISPACKET, fpp, 1.0);
//        for PACKETSAMP truth[s] = iwv.iframes.at(s);
//                for PACKETSAMP {
//                    truth[s] = 0.3*sin(phasor);
//                    phasor += 0.1;
//                }
                        for PACKETSAMP {
                            truth[s] = (s==0) ? 0.5 : 0.3*sin(phasor);
                            phasor += 0.01;
                        }
        //        qDebug() << pCnt;
        glitch = !((pCnt-off)%rate);
        if (stoc>0) {
            glitch = false;
            int rem = (pCnt % 6000);
            if (!rem) {
                lostPtr = 0;
                qDebug() << "cycle lost data";
                nextLost = LOST[lostPtr++];
            }
            int tmp = (pCnt / 6000);
            tmp = pCnt - (tmp*6000);
            if (tmp == nextLost) {
                glitch = true;
                nextLost = LOST[lostPtr++];
                qDebug() << nextLost;
            }
        }

        if(pCnt) {
            if (glitch) {
                glitchCnt++;
                accCons++;
            } else {
                maxCons = (accCons>maxCons) ? accCons : maxCons;
                accCons = 0;
            }
            if(run > 2) {
                for ( int i = 0; i < hist; i++ ) {
                    for PACKETSAMP train[s+((hist-(i+1))*fpp)] =  lastPackets[i][s];
                }
                //                qDebug() << train.size();

                // GET LINEAR PREDICTION COEFFICIENTS
                ba.train( coeffs, train, pCnt );

                // LINEAR PREDICT DATA
                vector<float> tail( train );

                ba.predict( coeffs, tail ); // resizes to TRAINSAMPS-2 + TRAINSAMPS

                for ( int i = 0; i < ORDER; i++ )
                    prediction[i] = tail[i+TRAINSAMPS];

                for PACKETSAMP xfadedPred[s] = truth[s] * fadeUp[s] +
                        nextPred[s] * fadeDown[s];
            }
#define OUT(ch,x) (output[ch][THISPACKET+x])
            for PACKETSAMP {
                switch(run)
                {
                case 0  : OUT(0,s) = truth[s];
                    break;
                case 1  : OUT(0,s) = (glitch) ? lastGoodPacket[s] : truth[s];
                    break;
                case 2  : OUT(0,s) = (glitch) ? 0.0 : truth[s];
                    break;
                case 3  :
                    OUT(0,s) =
                            (glitch) ? prediction[s]
                                       : ( (lastWasGlitch) ?
                                               xfadedPred[ s ]
                                               : truth[s] );
                    break;
                case 4  :
                    OUT(0,s) = (glitch) ? prediction[s] : truth[s];
                    break;
                case 5  : OUT(0,s) = prediction[s];
                    break;
                case 6  : {
                    double tmp = prediction[s] * fadeUp[s] +
                            nextPred[s] * fadeDown[s];
                    OUT(0,s) = (s==0) ? tmp : tmp;
                    // predicted fade up, last predicted fade down
                    glitch = false;
                }
                    break;
                }
                //                plot.write(OUT(0,s));
            }
            lastWasGlitch = glitch;
            for PACKETSAMP
                    nextPred[s] = prediction[ s + fpp];
        }

        // if pCnt==0 initialization follows
        for ( int i = hist-1; i>0; i-- ) {
            for PACKETSAMP lastPackets[i][s] = lastPackets[i-1][s];
        }

        // needs to be primed and will only be able to glitch if pCnt>0
        for PACKETSAMP lastPackets[0][s] = ((!glitch)||(pCnt<hist)) ? truth[s] : prediction[s];

        if (!glitch) for PACKETSAMP lastGoodPacket[s] = truth[s];

    }
    owv.writeFrames(OFILENAME, output);
    //    return a.exec();
    qDebug() << 100.0*(double)glitchCnt / (double)plen << "% loss";
    qDebug() << maxCons << "max consecutive";
    qDebug() << ((double)(plen * fpp)) / FS << "seconds";
}

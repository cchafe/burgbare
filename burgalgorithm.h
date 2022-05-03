#ifndef BURGALGORITHM_H
#define BURGALGORITHM_H

#include <math.h>
#include <vector>
using namespace std;

class BurgAlgorithm
{
public:
    void train(vector<long double> &coeffs, const vector<float> &x, int pCnt);
    void predict( vector<long double> &coeffs, vector<float> &predicted );
};


#endif // BURGALGORITHM_H

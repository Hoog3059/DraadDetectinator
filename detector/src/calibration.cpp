#include "Particle.h"
#include <math.h>
#include "calibration.h"

/*
const int confFitDegrees[] = {3, 5, 6, 6, 6};

const float confFitCoefs3[] = {0.09997537f, -0.12499677f, 0.00778866f};
const float confFitCoefs23[] = {15.67702377f, -63.52182903f, 150.00204801f, -200.67246907f, 136.976235f, -36.96144305f};
const float confFitCoefs234[] = {33.58319853f, -140.49643702f, 364.9884805f, -497.33992442f, 293.95162668f, -15.88403486f, -32.9886872f};
const float confFitCoefs1234[] = {23.24390436f, 147.14172372f, -977.76520963f, 2599.26415718f, -3506.04714566f, 2342.53167029f, -614.91543872f};
const float confFitCoefs12345[] = {49.35604118f, 32.20948708f, -768.63256059f, 2780.54069876f, -4504.58068835f, 3424.18451706f, -991.37748431f};

const float *confFitCoefs[] = {confFitCoefs3, confFitCoefs23, confFitCoefs234, confFitCoefs1234, confFitCoefs12345};

const float confDomainMax[] = {1.2f, 1.2f, 1.2f, 1.2f, 1.1f};
const float confDomainMin[] = {0.3f, 0.3f, 0.3f, 0.3f, 0.3f};
*/

const int confFitDegrees[] = {5, 5, 10, 5, 4};

const float confFitCoefs3[] = {17.04581097, -63.96595693, 162.59782668, -236.94073226, 172.67443738, -48.59816129};
const float confFitCoefs23[] = {27.07158011, -60.2291715, 153.70828641, -266.83179527, 232.16236735,
                                -76.46715652};
const float confFitCoefs234[] = {1.59112513e+04, -2.40368298e+05, 1.59750198e+06, -6.14565500e+06,
                                 1.51710358e+07, -2.51335002e+07, 2.83203836e+07, -2.14455395e+07,
                                 1.04508195e+07, -2.96128701e+06, 3.70716515e+05};
const float confFitCoefs1234[] = {897.2211744, -5332.28472534, 13028.07943322, -15780.41280462,
                                  9450.77066586, -2240.63451999};
const float confFitCoefs12345[] = {-3849.72945749, 16788.40269701, -26916.67106135, 18981.87620309,
                                   -4976.51328562};

const float *confFitCoefs[] = {confFitCoefs3, confFitCoefs23, confFitCoefs234, confFitCoefs1234, confFitCoefs12345};

const float confDomainMax[] = {1.25f, 1.2f, 1.1f, 1.15f, 1.15f};
const float confDomainMin[] = {0.3f, 0.4f, 0.5f, 0.6f, 0.9f};

int getIndexByConfiguration(uint8_t activatedSwitches)
{
    switch (activatedSwitches)
    {
    case 0b00100:
        return 0;
    case 0b00110:
        return 1;
    case 0b01110:
        return 2;
    case 0b01111:
        return 3;
    case 0b11111:
        return 4;
    default:
        // Going here should have been prevented as it is undefined behaviour.
        return 5;
    }
}

bool checkForValidCalibrationSwitchConfiguration(uint8_t activatedSwitches)
{
    switch (activatedSwitches)
    {
    case 0b00100:
    case 0b00110:
    case 0b01110:
    case 0b01111:
    case 0b11111:
        return true;
    default:
        return false;
    }
}

float getDepthByFit(uint8_t activatedSwitches, float Vptp)
{
    float depth = 0;

    switch (activatedSwitches)
    {
    /*
    case 0b00100:
    {
        float a = confFitCoefs3[0];
        float b = confFitCoefs3[1];
        float c = confFitCoefs3[2];
        depth = a / (Vptp + b) + c;
        break;
    }*/
    case 0b00100:

    case 0b00110:
    case 0b01110:
    case 0b01111:
    case 0b11111:
    {
        int index = getIndexByConfiguration(activatedSwitches);
        depth = polynomial(Vptp, confFitCoefs[index], confFitDegrees[index]) + 1;
        break;
    }
    default:
        // Going here should have been prevented as it is undefined behaviour.
        depth = 0.0;
        break;
    }

    return depth;
}

float polynomial(float Vptp, const float *coefs, int deg)
{
    float output = 0;

    for (int i = 0; i <= deg; i++)
    {
        output += coefs[i] * pow(Vptp, (float)i);
    }

    return output;
}
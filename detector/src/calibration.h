#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

bool checkForValidCalibrationSwitchConfiguration(uint8_t activatedSwitches);
float getDepthByFit(uint8_t activatedSwitches, float Vptp);
float polynomial(float Vptp, const float *coefs, int deg);
int getIndexByConfiguration(uint8_t activatedSwitches);

extern const int confFitDegrees[];

extern const float confFitCoefs3[];
extern const float confFitCoefs23[];
extern const float confFitCoefs234[];
extern const float confFitCoefs1234[];
extern const float confFitCoefs12345[];

extern const float *confFitCoefs[];

extern const float confDomainMax[];
extern const float confDomainMin[];

#endif
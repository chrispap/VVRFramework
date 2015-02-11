#ifndef UTILS_H
#define UTILS_H

#include "vvrscenedll.h"

#include <string>

using namespace std;

#define echo(x) cout<<#x<<" = "<<x<<endl

namespace vvr {

double VVRScene_API getSeconds();
double VVRScene_API elapsed(double startTime);
string VVRScene_API getExePath();
string VVRScene_API getBasePath();
double VVRScene_API  normalizeAngle(double angle);

}

#endif

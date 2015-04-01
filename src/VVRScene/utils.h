#ifndef UTILS_H
#define UTILS_H

#include "vvrscenedll.h"

#include <string>

using namespace std;

#define echo(x) cout<<#x<<" = "<<x<<endl
#define SQUARE(x) ((x)*(x))

namespace vvr {

inline float VVRScene_API getSeconds();
double VVRScene_API  normalizeAngle(double angle);
string VVRScene_API getExePath();
string VVRScene_API getBasePath();

}

#endif

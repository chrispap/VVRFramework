#ifndef UTILS_H
#define UTILS_H

#include "vvrframeworkdll.h"

#include <string>

using namespace std;

#define echo(x) cout<<#x<<" = "<<x<<endl

namespace vvr {

double VVRFramework_API getSeconds();
double VVRFramework_API elapsed(double startTime);
string VVRFramework_API getExePath();
string VVRFramework_API getBasePath();
double VVRFramework_API  normalizeAngle(double angle);

}

#endif

#ifndef UTILS_H
#define UTILS_H

#include "vvrframeworkdll.h"

#include <string>

using namespace std;

#define echo(x) cout<<#x<<" = "<<x<<endl

double vvrframework_API getSeconds();
double vvrframework_API elapsed(double startTime);
string vvrframework_API getExePath();
string vvrframework_API getBasePath();

#endif

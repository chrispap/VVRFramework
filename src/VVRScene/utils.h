#ifndef UTILS_H
#define UTILS_H

#include "vvrscenedll.h"

#include <string>

using namespace std;

#define echo(x) cout<<#x<<" = "<<x<<endl

double vvrscene_API getSeconds();
double vvrscene_API elapsed(double startTime);
string vvrscene_API getExePath();
string vvrscene_API getBasePath();

#endif

#ifndef UTILS_H
#define UTILS_H

#include "vvrscenedll.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

#define echo(x) cout<<#x<<" = "<<x<<endl
#define SQUARE(x) ((x)*(x))

namespace vvr {

float VVRScene_API getSeconds();
double VVRScene_API  normalizeAngle(double angle);
string VVRScene_API getExePath();
string VVRScene_API getBasePath();
vector<string> &split(const string &s, char delim, vector<string> &elems);
vector<string> split(const string &s, char delim);

}

#endif

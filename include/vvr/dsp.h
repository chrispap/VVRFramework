#ifndef DSP_H
#define DSP_H

#include "vvrframework_DLL.h"
#include <iostream>
#include <vector>

namespace vvr {
namespace dsp {

typedef std::vector<double> Signal;

Signal VVRFramework_API smooth(const Signal &in, int window_size);

Signal VVRFramework_API diff(const Signal &in, int stride = 1);

Signal VVRFramework_API threshold(const Signal &in, double threshold);

Signal VVRFramework_API consecutiveThreshold(const Signal &in, int max_cons_vals);

unsigned VVRFramework_API detectZero(const Signal &samples, unsigned offset, double tolerance = 0, bool reverse_dir = false);

unsigned VVRFramework_API detectNonZero(const Signal &signal, unsigned offset, double tolerance = 0, bool reverse_dir = false);

double VVRFramework_API interpLinear(double x0, double x1, double y0, double y1, double x);

double VVRFramework_API interpSmooth_0_1(int i, int imax);

}
}

#endif

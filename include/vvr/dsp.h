#ifndef DSP_H
#define DSP_H

#include "vvrframework_DLL.h"
#include <iostream>
#include <vector>

namespace vvr { namespace dsp
{
    typedef std::vector<double> Signal;

    Signal
    VVRFramework_API smooth(const Signal &in, int window_size);

    Signal
    VVRFramework_API diff(const Signal &in, int stride = 1);

    Signal
    VVRFramework_API threshold(const Signal &in, double threshold);

    Signal
    VVRFramework_API consecutive_threshold(const Signal &in, int max_cons_vals);

    unsigned
    VVRFramework_API detect_zero(const Signal &samples, unsigned offset, double tolerance = 0, bool reverse_dir = false);

    unsigned
    VVRFramework_API detect_nonzero(const Signal &signal, unsigned offset, double tolerance = 0, bool reverse_dir = false);

    double
    VVRFramework_API interp_linear(double x0, double x1, double y0, double y1, double x);

    double
    VVRFramework_API interp_smooth_01(int i, int imax);
}}

#endif

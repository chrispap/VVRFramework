#ifndef DSP_H
#define DSP_H

#include "vvrframework_DLL.h"
#include <iostream>
#include <vector>

namespace vvr { namespace dsp
{
    typedef std::vector<double> Signal;

    Signal
    VVRFramework_API diff(const Signal &in, size_t stride = 1);

    Signal
    VVRFramework_API smooth(const Signal &in, size_t window_size);

    Signal
    VVRFramework_API threshold(const Signal &in, double threshold);

    Signal
    VVRFramework_API consecutive_threshold(const Signal &in, size_t max_cons_vals);

    size_t
    VVRFramework_API detect_zero(const Signal &samples, size_t offset, double tol = 0, bool reverse_dir = false);

    size_t
    VVRFramework_API detect_nonzero(const Signal &signal, size_t offset, double tol = 0, bool reverse_dir = false);

    double
    VVRFramework_API interp_linear(double x0, double x1, double y0, double y1, double x);

    double
    VVRFramework_API interp_smooth_01(size_t i, size_t imax);
}}

#endif

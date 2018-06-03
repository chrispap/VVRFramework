#include <vvr/dsp.h>
#include <algorithm>
#include <cmath>

vvr::dsp::Signal vvr::dsp::smooth(const Signal &in, size_t window_size)
{
    Signal out(in.size());

    if (window_size % 2 == 0) window_size++; // we need odd size

    const size_t hw = window_size / 2;
    const size_t ie = in.size() - hw;
    if (in.size() < hw) return out; // too big window

    for (size_t i = hw; i < ie; i++) {
        long double sum = 0;
        for (size_t j = i - hw; j <= i + hw; j++) sum += in[j];
        out[i] = sum / window_size;
    }

    return out;
}

vvr::dsp::Signal vvr::dsp::diff(const Signal &in, size_t stride)
{
    Signal out(in.size());

    if (!stride) stride = 1;
    if (stride > in.size()) return out;

    for (size_t i = 0; i < stride; i++) {
        out[i] = ::fabs(in[i] - in[0]) / (i + 1);
    }

    for (size_t i = stride; i < in.size(); i++) {
        out[i] = ::fabs(in[i] - in[i - stride]);
    }

    if (stride > 1) {
        for (size_t i = stride; i < in.size(); i++) {
            out[i] /= stride;
        }
    }

    return out;
}

vvr::dsp::Signal vvr::dsp::threshold(const Signal &in, double threshold)
{
    Signal out(in);

    for (int i = 0; i < in.size(); i++)
    {
        if (in[i] <= threshold)
            out[i] = 0;
        else
            out[i] = 1;
    }

    return out;
}

vvr::dsp::Signal vvr::dsp::consecutive_threshold(const Signal &in, size_t max_cons_vals)
{
    Signal out(in);

    int zero_counter = 0;

    for (int i = 0; i < in.size(); i++)
    {
        if (!in[i]) zero_counter++;
        else zero_counter = 0;
        if (zero_counter < max_cons_vals)
            out[i] = 1;
        else
            out[i] = 0;
    }

    return out;
}

double vvr::dsp::interp_smooth_01(size_t i, size_t imax)
{
    return sin(1.57079632679489661923 * i / imax);
}

double vvr::dsp::interp_linear(double x0, double x1, double y0, double y1, double x)
{
    const double dx = x1 - x0;
    const double dy = y1 - y0;
    const double a = dy / dx;
    return y0 + a * (x - x0);
}

size_t vvr::dsp::detect_nonzero(const Signal &signal, size_t offset, double tol, bool reverse_dir)
{
    if (signal.empty()) return 0;
    if (offset > signal.size()) return signal.size() - 1;

    if (!reverse_dir)
    {
        for (size_t i = offset; i < signal.size(); i++) {
            if (::fabs(signal[i]) > tol) {
                return i;
            }
        }
    }
    else
    {
        for (size_t i = offset; i >= 0; i--) {
            if (::fabs(signal[i]) > tol) {
                return i;
            }
        }
    }

    return offset;
}

size_t vvr::dsp::detect_zero(const Signal &signal, size_t offset, double tol, bool reverse_dir)
{
    if (!reverse_dir)
    {
        for (size_t i = offset; i < signal.size(); i++) {
            if (::fabs(signal[i]) <= tol) {
                return i;
            }
        }
    }
    else
    {
        for (size_t i = 0; i < offset; i--) {
            const size_t j = offset - i;
            if (::fabs(signal[j]) <= tol) {
                return j;
            }
            if (!i) break; // catch unsigned overflow
        }
    }
    return offset;
}

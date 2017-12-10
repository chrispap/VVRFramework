#ifndef BSPLINE_H
#define BSPLINE_H

#include <vector>
#include <type_traits>

namespace vvr {

template <typename G>
static inline G spline_divide(const G& num, const G& den)
{
    if (den != 0.0) return num / den;
    if (num == 1.0) return 1.0;
    else return 0.0;
}

template<typename G>
G& ref(G& obj) 
{
    return obj; 
}

template<typename G>
G& ref(G* obj) 
{
    return *obj; 
}

template <typename T>
class BSpline
{
    typedef typename std::remove_pointer<T>::type point_t;
    typedef std::vector<std::vector<double>> double_vector_2d;

    std::vector<point_t>    _pts;
    std::vector<T>          _cps;
    std::vector<double>     _knots;
    size_t                  _num_pts;
    bool                    _dirty;

public:
    BSpline() 
        : _num_pts(2)
        , _dirty(true) 
    { }

    point_t eval(const double t)
    {
        const auto &X = _knots;
        const auto &B = _cps;
        const int kn = X.size();
        const int kp = B.size();
        const int mb = kn - kp;
        int i = mb - 1;
        int k = 0;
        double_vector_2d N(kn, std::vector<double>(mb, 0.0));

        /* Find knot span */
        while (t > X[i + 1] && i < kn - mb) i++;
        N[i][k] = 1;

        for (k = 1; k < mb; k++) {
            for (i = 0; i < kn - k - 1; i++) {
                const double A_num = (t - X[i]) * N[i][k - 1];
                const double A_den = X[i + k] - X[i];
                const double C_num = (X[i + k + 1] - t) * N[i + 1][k - 1];
                const double C_den = X[i + k + 1] - X[i + 1];
                const double A = spline_divide(A_num, A_den);
                const double C = spline_divide(C_num, C_den);
                N[i][k] = A + C;
            }
        }

        point_t p = ref(B[0]) * N[0][mb - 1];
        for (i = 1; i < kp; i++) {
            p += ref(B[i]) * N[i][mb - 1];
        }

        return p;
    }

    void update(bool force = false)
    {
        if (!_dirty && !force) return;
        auto range = get_param_range();
        auto dt = (range.second - range.first) / (_num_pts - 1);
        _pts.clear();
        for (int i = 0; i < _num_pts; i++) {
            _pts.push_back(eval(range.first + dt * i));
        }
        _dirty = false;
    }

    void set_num_pts(int num)
    {
        bool force = num != _num_pts;
        if (num < 2) num = 2;
        _num_pts = num;
        update(force);
    }

    void set_cps(std::vector<T> &&cps)
    {
        _cps = cps;
        _dirty = true;
    }

    void set_cps(const std::vector<T> &cps)
    {
        _cps = cps;
        _dirty = true;
    }

    void set_knots(std::vector<double> &&knots)
    {
        _knots = knots;
        _dirty = true;
    }

    void set_knots(const std::vector<double> &knots)
    {
        _knots = knots;
        _dirty = true;
    }

    const auto get_param_range()
    {
        const int mb = _knots.size() - _cps.size();
        std::pair<double, double> range;
        range.first = _knots[mb - 1];
        range.second = *(_knots.end() - 1);
        return range;
    }

    const auto& get_pts() const
    {
        return _pts;
    }

    const auto& get_cps() const
    {
        return _cps;
    }

};

}

#endif

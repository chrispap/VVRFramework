#ifndef BSPLINE_H
#define BSPLINE_H

#include <vector>
#include <type_traits>

namespace vvr {

template<typename G>
G& ref(G& obj) { return obj; }

template<typename G>
G& ref(G* obj) { return *obj; }

template <typename G>
static inline G spline_division(const G& num, const G& den)
{
    if (den!=0.0) return num / den;
    if (num==1.0) return 1.0;
    else return 0.0;
}

template <typename T>
struct BSpline
{
    typedef typename std::remove_pointer<T>::type point_t;
    typedef std::vector<std::vector<double>> double_vector_2d;

    std::vector<T> cps;
    std::vector<double> knots;

    BSpline() {}

    point_t Eval(const double t)
    {
        const auto &X = knots;
        const auto &B = cps;
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
                const double A = spline_division(A_num, A_den);
                const double C = spline_division(C_num, C_den);
                N[i][k] = A + C;
            }
        }

        point_t p = ref(B[0]) * N[0][mb - 1];
        for (i = 1; i < kp; i++) {
            p += ref(B[i]) * N[i][mb - 1];
        }

        return p;
    }

    const auto ParamRange()
    {
        const int mb = knots.size() - cps.size();
        return std::make_pair(knots[mb - 1], (*(knots.end() - 1)));
    }
};

}

#endif

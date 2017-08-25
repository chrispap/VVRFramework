#ifndef BSPLINE_H
#define BSPLINE_H

#include <vector>
#include <type_traits>

namespace vvr {

template <typename T>
class BSpline
{
    template<typename G>
    G& ref(G& obj) { return obj; }

    template<typename G>
    G& ref(G* obj) { return *obj; }

    typedef typename std::remove_pointer<T>::type point_t;
    typedef std::vector<std::vector<double>> double_vector_2d;

    size_t mNumPts;
    std::vector<T> mCps;
    std::vector<double> mKnots;
    std::vector<point_t> mCurvePts;

public:
    BSpline() : mNumPts(0) {}
    void setCtrPts(std::vector<T> &&cps) { mCps = cps; }
    void setCtrPts(const std::vector<T> &cps) { mCps = cps; }
    void setKnots(std::vector<double> &&knots) { mKnots = knots; }
    void setKnots(const std::vector<double> &knots) { mKnots = knots; }
    size_t getNumPts() { return mNumPts; }
    std::vector<T>& getCtrlPts() { return mCps; }
    const std::vector<point_t>& getCurvePts() { return mCurvePts; }

    std::pair<double, double> getParamRange()
    {
        const int mb = mKnots.size() - mCps.size();
        std::pair<double, double> range;
        range.first = mKnots[mb - 1];
        range.second = *(mKnots.end() - 1);
        return range;
    }

    point_t getCurvePoint(const double t)
    {
        const auto &X = mKnots;
        const auto &B = mCps;
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

    void updateCurvePts(size_t num_pts, bool force = false)
    {
        if (num_pts < 2) num_pts = 2;
        if (mNumPts == num_pts && !force) return;
        mNumPts = num_pts;
        mCurvePts.clear();
        auto range = getParamRange();
        auto dt = (range.second - range.first) / (num_pts - 1);
        for (int i = 0; i < num_pts; i++) {
            mCurvePts.push_back(getCurvePoint(range.first + dt * i));
        }
    }

private:
    template <typename G>
    static inline G spline_divide(const G& num, const G& den)
    {
        if (den != 0.0) return num / den;
        if (num == 1.0) return 1.0;
        else return 0.0;
    }
};

}

#endif

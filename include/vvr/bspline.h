#ifndef BSPLINE_H
#define BSPLINE_H

#include <vector>

template<typename Point>
class BSpline
{
    std::vector<double> mKnots;
    std::vector<Point> mCps;
    std::vector<Point> mCurvePts;
    size_t m_num_pts;

public:
    BSpline() : m_num_pts(0) {}
    void setCtrPts(std::vector<Point> &&cps);
    void setKnots(std::vector<double> &&knots);
    size_t getNumPts() { return m_num_pts; }
    std::pair<double, double> getParamRange();
    Point getCurvePoint(const double t);
    void updateCurve(size_t num_pts, bool force = false);
    std::vector<Point>& getCtrlPts() { return mCps; }
    const std::vector<Point>& getCurvePts() { return mCurvePts; }

private:
    template<typename T>
    static inline T spline_divide(const T& num, const T& den)
    {
        if (den != 0.0) return num / den;
        if (num == 1.0) return 1.0;
        else return 0.0;
    }
};

template<typename Point>
void BSpline<Point>::setCtrPts(std::vector<Point> &&cps)
{
    mCps = cps;
}

template<typename Point>
void BSpline<Point>::setKnots(std::vector<double> &&knots)
{
    mKnots = knots;
}

template<typename Point>
std::pair<double, double> BSpline<Point>::getParamRange()
{
    const int mb = mKnots.size() - mCps.size();
    std::pair<double, double> range;
    range.first = mKnots[mb - 1];
    range.second = *(mKnots.end() - 1);
    return range;
}

template<typename Point>
Point BSpline<Point>::getCurvePoint(const double t)
{
    const auto &X = mKnots;
    const auto &B = mCps;
    const int kn = X.size();
    const int kp = B.size();
    const int mb = kn - kp;
    int i;
    int k;
    std::vector<std::vector<double>> N(kn, std::vector<double>(mb, 0.0));

    /* Find knot span */
    i = mb - 1;
    k = 0;
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

    Point p = B[0] * N[0][mb - 1];
    for (i = 1; i < kp; i++) {
        p += B[i] * N[i][mb - 1];
    }

    return p;
}

template<typename Point>
void BSpline<Point>::updateCurve(size_t num_pts, bool force)
{
    if (num_pts < 2) num_pts = 2;
    if (m_num_pts == num_pts && !force) return;
    m_num_pts = num_pts;
    mCurvePts.clear();
    auto range = getParamRange();
    auto dt = (range.second - range.first) / (num_pts - 1);
    for (int i = 0; i < num_pts; i++) {
        mCurvePts.push_back(getCurvePoint(range.first + dt * i));
    }
}

#endif

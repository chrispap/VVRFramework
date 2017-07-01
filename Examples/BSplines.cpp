#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/canvas.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

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

class BSplineScene : public vvr::Scene
{
public:
    BSplineScene();

    const char* getName() const override {
        return "BSpline Scene";
    }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    BSpline<C2DPoint> m_bspline;
    C2DPoint *m_curr_p;
    bool m_disp_curve_pts;
};

#define BOOL_TOGGLE(x) x = !x

#define col_bg          vvr::Colour::white
#define col_line        vvr::Colour::grey
#define col_pts_ctrl    vvr::Colour::red
#define col_pts_sel     vvr::Colour::darkRed

BSplineScene::BSplineScene()
{
    vvr::Shape::DEF_POINT_SIZE = 25;
    vvr::Shape::DEF_LINE_WIDTH = 3;
    m_fullscreen = true;
    m_bg_col = col_bg;
    reset();
}

void BSplineScene::reset()
{
    vvr::Scene::reset();
    m_curr_p = NULL;

    std::vector<C2DPoint> pts = {
        { 0, 100, },
        { 100, -100, },
        { 200, 100, },
        { 300, -100, },
    };

    for (auto &p : pts) {
        p.x -= 150;
        p *= 4;
    }

    m_bspline.setCtrPts(std::move(pts));
    m_bspline.setKnots({ 0, 0, 0, 0, 1, 1, 1, 1 });
    m_bspline.updateCurve(3);
}

void BSplineScene::draw()
{
    enterPixelMode();
    auto curvpts = m_bspline.getCurvePts();
    auto p = curvpts.front();
    if (m_disp_curve_pts) vvr::Point2D(p.x, p.y, col_line).draw();
    for (auto it = curvpts.begin(); it < curvpts.end() - 1; ++it) {
        auto p1 = *(it + 0);
        auto p2 = *(it + 1);
        vvr::LineSeg2D(p1.x, p1.y, p2.x, p2.y, col_line).draw();
        if (m_disp_curve_pts) vvr::Point2D(p2.x, p2.y, col_line).draw();
    }
    for (auto &p : m_bspline.getCtrlPts()) {
        vvr::Colour col = &p == m_curr_p ? col_pts_sel : col_pts_ctrl;
        vvr::Point2D(p.x, p.y, col).draw();
    }
    returnFromPixelMode();
}

void BSplineScene::keyEvent(unsigned char key, bool up, int modif)
{
    key = ::tolower(key);

    switch (key) {
    case 'p': BOOL_TOGGLE(m_disp_curve_pts); break;
    case 'r': reset(); break;
    }
}

void BSplineScene::mousePressed(int x, int y, int modif)
{
    C2DPoint mousept(x, y);

    for (auto &p : m_bspline.getCtrlPts()) {
        if (mousept.Distance(p) < vvr::Shape::DEF_POINT_SIZE) {
            m_curr_p = &p;
        }
    }
}

void BSplineScene::mouseMoved(int x, int y, int modif)
{
    C2DPoint mousept(x, y);

    if (m_curr_p) {
        *m_curr_p = mousept;
    }

    m_bspline.updateCurve(m_bspline.getNumPts(), true);
}

void BSplineScene::mouseReleased(int x, int y, int modif)
{
    m_curr_p = NULL;
}

void BSplineScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::ArrowDir::UP) {
        m_bspline.updateCurve(m_bspline.getNumPts() + 1);
    }
    else if (dir == vvr::ArrowDir::DOWN) {
        m_bspline.updateCurve(m_bspline.getNumPts() - 1);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new BSplineScene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

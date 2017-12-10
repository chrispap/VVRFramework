#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/utils.h>
#include <GeoLib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

class ConvexHullScene : public vvr::Scene
{
public:

    ConvexHullScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    void ComputeConvexHull();

private:
    vvr::Canvas m_canvas;
    std::vector<vvr::Point2D> m_pts;
    std::vector<vvr::Point2D> m_hull;
    vvr::Point2D* m_mousepos;
};

/*---[Convex Hull/Diameter/Width]-------------------------------------------------------*/

bool operator==(const vvr::Point2D &lhs, const vvr::Point2D &rhs)
{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

template <typename point>
bool ccw(const point &a, const point &b, const point &c) 
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x) > 0;
}

template <typename point>
auto area(const point &a, const point &b, const point &c)
{
    return abs((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
}

template <typename point, typename real = decltype(point::x)>
auto dist(const point &a, const point &b)
{
    return (real)hypot(a.x - b.x, a.y - b.y);
}

template <typename point>
std::vector<point> convex_hull(std::vector<point> p)
{
    int n = p.size();
    if (n <= 1) return p;
    int k = 0;
    std::sort(p.begin(), p.end(), [](auto &lhs, auto &rhs) {
        return (lhs.x < rhs.x) || ((lhs.x == rhs.x) && (lhs.y < rhs.y));
    });
    std::vector<point> q(n * 2);
    for (int i = 0; i < n; q[k++] = p[i++]) {
        for (; k >= 2 && !ccw(q[k - 2], q[k - 1], p[i]); --k);
    }
    for (int i = n - 2, t = k; i >= 0; q[k++] = p[i--]) {
        for (; k > t && !ccw(q[k - 2], q[k - 1], p[i]); --k);
    }
    q.resize(k - 1 - (q[0] == q[1]));
    return q;
}

template <typename point, typename real=decltype(point::x)>
real convex_diameter(const std::vector<point> &h, size_t &i1, size_t &i2)
{
    size_t n = h.size();
    if (n == 1) return 0;
    if (n == 2) return dist(h[0], h[1]);
    size_t k = 1;
    while (area(h[n - 1], h[0], h[(k + 1) % n]) > area(h[n - 1], h[0], h[k])) k++;
    real res = 0, d;
    for (size_t i = 0, j = k; i <= k && j < n; i++) {
        if ((d = dist(h[i], h[j])) > res) {
            res = d; 
            i1 = i;
            i2 = j;
        }
        while (j < n && area(h[i], h[(i + 1) % n], h[(j + 1) % n]) > area(h[i], h[(i + 1) % n], h[j])) {
            if ((d = dist(h[i], h[(j + 1) % n])) > res) {
                res = d; 
                i1 = i;
                i2 = j+1;
            }
            ++j;
        }
    }
    return res;
}

template <typename point, typename real = decltype(point::x)>
real convex_width(const std::vector<point> &h, size_t &i1, size_t &i2)
{
    const size_t n = h.size();
    if (n < 3) return 0;

    auto cmp = [](const point &lhs, const point &rhs) -> bool {
        return (lhs.x < rhs.x) || ((lhs.x == rhs.x) && (lhs.y > rhs.y));
    };

    C2DLine calipera, caliperb;
    size_t ia = std::min_element(h.begin(), h.end(), cmp) - h.begin();
    size_t ib = std::max_element(h.begin(), h.end(), cmp) - h.begin();
    calipera.vector = C2DVector(0, -1);
    caliperb.vector = C2DVector(0, +1);

    real rot = 0, minwidth = std::numeric_limits<real>::max();
    while (rot < math::pi)
    {
        C2DPoint pa(h[ia].x, h[ia].y);
        C2DPoint pb(h[ib].x, h[ib].y);
        C2DPoint paa(h[(ia+1)%n].x, h[(ia+1)%n].y);
        C2DPoint pbb(h[(ib+1)%n].x, h[(ib+1)%n].y);
        C2DVector edgea = paa - pa; edgea.MakeUnit();
        C2DVector edgeb = pbb - pb; edgeb.MakeUnit();
        real cosa = calipera.vector.Dot(edgea);
        real cosb = caliperb.vector.Dot(edgeb);
        calipera.point = pa;
        caliperb.point = pb;

        real width;
        size_t min_i1, min_i2;
        if (cosa > cosb)
        {
            calipera.vector = edgea;
            caliperb.vector = calipera.vector * -1;
            width = calipera.DistanceAsRay(caliperb.point);
            min_i1 = ia;
            min_i2 = ib;
            ia = (ia + 1) % n;
            rot += acos(cosa);
        }
        else
        {
            caliperb.vector = edgeb;
            calipera.vector = caliperb.vector * -1;
            width = caliperb.DistanceAsRay(calipera.point);
            min_i1 = ib;
            min_i2 = ia;
            ib = (ib + 1) % n;
            rot += acos(cosb);
        }

        if (width < minwidth)
        {
            minwidth = width;
            i1 = min_i1;
            i2 = min_i2;
        }
    }

    return minwidth;
}

/*--------------------------------------------------------------------------------------*/

ConvexHullScene::ConvexHullScene()
{
    m_bg_col = vvr::Colour(0x44, 0x44, 0x44);
    m_hide_log = false;
    m_hide_sliders = true;
    reset();
    m_canvas.clear();
    m_pts.clear();
    m_pts.push_back({100,140});
    m_pts.push_back({100,-100});
    m_pts.push_back({100,50});
    m_pts.push_back({-200,100});
    m_pts.push_back({-200,-100});
    m_pts.push_back({-250,40});
    m_pts.push_back({-250,80});
    ComputeConvexHull();
}

void ConvexHullScene::reset()
{
    Scene::reset();
    m_mousepos = new vvr::Point2D();
    m_canvas.clear();
    m_canvas.add(m_mousepos);

    /* Create random pts */
    m_pts.clear();
    math::LCG lcg;
    for (int i = 0; i < 500; ++i) {
        float x = -300 + 600 * (lcg.Float() - 0.5f);
        float y = -200 + 400 * 0.8 * (lcg.Float() + 0.1);
        m_pts.push_back(vvr::Point2D(x,y));
    }
    
    /* Compute & Measure time */
    float sec = vvr::getSeconds();
    ComputeConvexHull();
    sec = vvr::getSeconds() - sec;
    std::cout << "Computed in " << sec << "\"" << std::endl;
}

void ConvexHullScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    m_mousepos->colour = vvr::magenta;
    m_mousepos->x = x; m_mousepos->y = y;
}

void ConvexHullScene::mouseReleased(int x, int y, int modif)
{
    m_mousepos->colour = vvr::white;
    m_mousepos->x = x; m_mousepos->y = y;
}

void ConvexHullScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    mousePressed(x, y, modif);

    auto &h = m_hull;
    const size_t n = h.size();
    
    m_mousepos->colour = vvr::green;
    for (size_t i=0; i<n; ++i) {
        if (!ccw(*m_mousepos, h[i], h[(i+1)%n])) { 
            m_mousepos->colour = vvr::red;
            break;
        }
    }
}

void ConvexHullScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::LEFT) {
        m_canvas.prev();
    }
    else if (dir == vvr::RIGHT) {
        m_canvas.next();
    }
}

void ConvexHullScene::draw()
{
    enterPixelMode();
    {
        vvr::Shape::PointSize = 12;
        m_canvas.draw();
        for (const auto &p : m_pts) p.draw();
    }
    exitPixelMode();
}

void ConvexHullScene::ComputeConvexHull()
{
    /* Compute hull */
    m_hull = convex_hull(m_pts);
    auto &h = m_hull;
    const size_t n = h.size();

    /* Display: Hull edges */
    for (size_t i=0; i<n; i++) {
        m_canvas.newFrame(true);
        m_canvas.add(new vvr::LineSeg2D(h[i].x, h[i].y, h[(i+1)%n].x, h[(i+1)%n].y, vvr::orange));
    }

    /* Display: Hull Diameter */
    {
        size_t i1, i2;
        const auto diam = convex_diameter(h, i1, i2);
        m_canvas.add(new vvr::LineSeg2D(h[i1].x, h[i1].y, h[i2].x, h[i2].y, vvr::blue));
        vvr_echo(diam);
    }

    /* Display: Hull Width */
    {
        size_t i1, i2;
        const auto width = convex_width(h, i1, i2);
        C2DPoint p1(h[i1].x, h[i1].y);
        C2DPoint p11(h[(i1+1)%n].x, h[(i1+1)%n].y);
        C2DPoint p2(h[i2].x, h[i2].y);
        C2DVector slimdir = p11 - p1;
        C2DLine calipera(C2DPoint(h[i1].x, h[i1].y), slimdir);
        C2DLine caliperb(C2DPoint(h[i2].x, h[i2].y), slimdir);
        m_canvas.add(calipera, vvr::magenta, true);
        m_canvas.add(caliperb, vvr::cyan, true);
        m_canvas.add(p1, vvr::magenta);
        m_canvas.add(p11, vvr::magenta);
        m_canvas.add(p2, vvr::cyan);
        vvr_echo(width);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new ConvexHullScene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

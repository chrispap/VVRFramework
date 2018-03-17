#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/geom.h>
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
};

ConvexHullScene::ConvexHullScene()
{
    m_bg_col = vvr::Colour(0x44, 0x44, 0x44);
    m_show_log = true;
    m_show_sliders = true;
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
    m_canvas.clear();

    /* Create random pts */
    m_pts.clear();
    math::LCG lcg;
    for (int i = 0; i < 50; ++i) {
        m_pts.push_back(vvr::Point2D(
            600 * (lcg.Float() - 0.5f),
            400 * (lcg.Float() - 0.5f)));
    }

    /* Compute & Measure time */
    float sec = vvr::get_seconds();
    ComputeConvexHull();
    sec = vvr::get_seconds() - sec;
    std::cout << "Computed in " << sec << "\"" << std::endl;
}

void ConvexHullScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    m_pts.push_back(vvr::Point2D(x,y));
    ComputeConvexHull();
}

void ConvexHullScene::mouseReleased(int x, int y, int modif)
{
}

void ConvexHullScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    mousePressed(x, y, modif);
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
        vvr::Shape::PointSize = 10;
        //for (const auto &p : m_pts) p.draw();
        m_canvas.draw();
    }
    exitPixelMode();
}

void ConvexHullScene::ComputeConvexHull()
{
    /* Compute hull */
    auto h = convex_hull(m_pts);
    const size_t n = h.size();

    m_canvas.clear();

    /* Display: Hull edges */
    for (size_t i=0; i<n; i++) {
        m_canvas.newFrame(true);
        m_canvas.add(new vvr::LineSeg2D(h[i].x, h[i].y, h[(i+1)%n].x, h[(i+1)%n].y, vvr::orange));
    }

    m_canvas.newFrame(true);

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
        return vvr::main_with_scene(argc, argv, new ConvexHullScene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

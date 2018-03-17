#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/macros.h>
#include <C2DTriangle.h>
#include <cfloat>

class TriangleScene : public vvr::Scene
{
public:
    TriangleScene();
    const char* getName() const override {
        return "A Triangle";
    }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

private:
    std::vector<vvr::Point2D*> m_pts;
    vvr::Canvas m_canvas, m_canvas_pts;
    vvr::Point2D *m_point_grabbed;
    void createShapesFromPoints();
};

TriangleScene::TriangleScene()
{
    reset();
}

void TriangleScene::reset()
{
    Scene::reset();

    m_bg_col = vvr::white;
    vvr::Shape::PointSize = 10;
    vvr::Shape::LineWidth = 2;

    m_pts.clear();
    m_pts.push_back(new vvr::Point2D(0, 150));
    m_pts.push_back(new vvr::Point2D(-150, -100));
    m_pts.push_back(new vvr::Point2D(150, 50));
    m_canvas.clear();
    m_canvas_pts.clear();
    for (vvr::Shape* sh : m_pts) {
        m_canvas_pts.add(sh);
    }
    createShapesFromPoints();
    m_point_grabbed = NULL;
}

void TriangleScene::createShapesFromPoints()
{
    m_canvas.clear();

    //! Calculate the geometrical quantities of interest.
    C2DPoint p1(m_pts[0]->x, m_pts[0]->y);
    C2DPoint p2(m_pts[1]->x, m_pts[1]->y);
    C2DPoint p3(m_pts[2]->x, m_pts[2]->y);
    C2DTriangle t(p1, p2, p3);
    C2DPoint cc = t.GetCircumCentre();
    C2DPoint ce = (t.GetPoint1() + t.GetPoint2() + t.GetPoint3()) / 3;
    C2DPoint mid1 = (t.GetPoint2() + t.GetPoint3()) / 2;
    C2DPoint mid2 = (t.GetPoint3() + t.GetPoint1()) / 2;
    C2DPoint mid3 = (t.GetPoint1() + t.GetPoint2()) / 2;

    //! Add them to the canvas.
    m_canvas.add(new vvr::Triangle2D(m_pts[0]->x, m_pts[0]->y, m_pts[1]->x, m_pts[1]->y, m_pts[2]->x, m_pts[2]->y));
    m_canvas.add(new vvr::Point2D(cc.x, cc.y, vvr::red));
    m_canvas.add(new vvr::Point2D(ce.x, ce.y, vvr::yellowGreen));
    m_canvas.add(new vvr::Point2D(mid1.x, mid1.y, vvr::red));
    m_canvas.add(new vvr::Point2D(mid2.x, mid2.y, vvr::red));
    m_canvas.add(new vvr::Point2D(mid3.x, mid3.y, vvr::red));
    m_canvas.add(new vvr::LineSeg2D(cc.x, cc.y, mid1.x, mid1.y, vvr::red));
    m_canvas.add(new vvr::LineSeg2D(cc.x, cc.y, mid2.x, mid2.y, vvr::red));
    m_canvas.add(new vvr::LineSeg2D(cc.x, cc.y, mid3.x, mid3.y, vvr::red));
    m_canvas.add(new vvr::LineSeg2D(ce.x, ce.y, t.GetPoint1().x, t.GetPoint1().y, vvr::yellowGreen));
    m_canvas.add(new vvr::LineSeg2D(ce.x, ce.y, t.GetPoint2().x, t.GetPoint2().y, vvr::yellowGreen));
    m_canvas.add(new vvr::LineSeg2D(ce.x, ce.y, t.GetPoint3().x, t.GetPoint3().y, vvr::yellowGreen));
}

void TriangleScene::draw()
{
    enterPixelMode();
    m_canvas.draw();
    m_canvas_pts.draw();
    exitPixelMode();
}

void TriangleScene::mousePressed(int x, int y, int modif)
{
    float min_dist = FLT_MAX;

    for (auto p : m_pts) {
        const float dist = vvr_square(x - p->x) + vvr_square(y - p->y);
        if (dist < min_dist) {
            min_dist = dist;
            m_point_grabbed = p;
        }
    }

    if (min_dist > vvr_square(20)) {
        m_point_grabbed = NULL;
    }

    if (m_point_grabbed) {
        m_point_grabbed->colour = vvr::magenta;
    }
}

void TriangleScene::mouseReleased(int x, int y, int modif)
{
    if (m_point_grabbed) {
        m_point_grabbed->colour = vvr::black;
    }
}

void TriangleScene::mouseMoved(int x, int y, int modif)
{
    if (!m_point_grabbed) return;
    m_point_grabbed->x = x;
    m_point_grabbed->y = y;
    createShapesFromPoints();
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(TriangleScene)
#endif

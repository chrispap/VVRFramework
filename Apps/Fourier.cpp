#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/animation.h>
#include <vvr/scene.h>
#include <GeoLib.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>

struct Fcomp
{
    double l;
    double w;
};

struct FourierScene : public vvr::Scene
{
    FourierScene();
private:
    const char* getName() const override { return "Fourier Series Animation"; }
    void draw() override;
    void reset() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    vvr::Animation m_anim;
    vvr::Canvas m_lines;
    vvr::Canvas m_trace;
    std::vector<Fcomp> fc;
};

FourierScene::FourierScene()
{
    vvr::Shape::PointSize = 2;
    vvr::Shape::LineWidth = 1.5;
    m_bg_col = vvr::grey;
    fc.push_back({  10,   25 });
    fc.push_back({ 150,   50 });
    fc.push_back({  50,  150 });
    fc.push_back({  15,  600 });
    for (auto i=0; i<fc.size(); i++) {
        m_lines.add(new vvr::LineSeg2D);
    }
    reset();
}

void FourierScene::reset()
{
    std::cout << "Reset" << std::endl;
    vvr::Scene::reset();
    m_trace.clear();
    m_anim.reset();
}

void FourierScene::draw()
{
    enterPixelMode();
    m_trace.draw();
    m_lines.draw();
    exitPixelMode();
}

bool FourierScene::idle()
{
    m_anim.update(true);
    const float t = m_anim.t();
    C2DVector e(0, 0);
    size_t i = 0;

    for (vvr::Drawable *d : m_lines.getDrawables())
    {
        auto &ln = *static_cast<vvr::LineSeg2D*>(d);
        C2DVector v(fc[i].l, 0);
        v.TurnLeft(t * math::DegToRad(fc[i].w));
        ln.x1 = e.i; ln.y1 = e.j; e += v;
        ln.x2 = e.i; ln.y2 = e.j;
        ++i;
    }

    m_trace.add(C2DPoint(e), vvr::orange);
    return true;
}

void FourierScene::mousePressed(int x, int y, int modif)
{
}

void FourierScene::mouseMoved(int x, int y, int modif)
{
}

void FourierScene::mouseWheel(int dir, int modif)
{
    static float speed = 1.0f;
    speed *= (1.0f + (0.1f * dir));
    m_anim.setSpeed(speed);
}

void FourierScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(FourierScene)
#endif

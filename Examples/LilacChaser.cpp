#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class LilacChaserScene : public vvr::Scene
{
public:
    LilacChaserScene();

    const char* getName() const override {
        return "Lilac Chaser Illusion";
    }

protected:
    void draw() override;
    void reset() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    vvr::Canvas m_canvas;
    std::vector<vvr::Circle2D*> m_circles;

private:
    const float R = 120;
    const float r = 20;
    const unsigned N = 12;
    const vvr::Colour col_lilac = vvr::Colour("CDA9CD");
};

using namespace std;

LilacChaserScene::LilacChaserScene()
{
    m_bg_col = vvr::Colour("BFBFBF");
    m_fullscreen = false;

    for (int i = 0; i < N; ++i)
    {
        float deg = math::DegToRad(360.0) / N * i;
        float x = R * sin(deg);
        float y = R * cos(deg);
        vvr::Circle2D *c = new vvr::Circle2D(x, y, r, col_lilac);
        c->setSolidRender(true);
        m_canvas.add(c);
        m_circles.push_back(c);
    }

    m_canvas.add(new vvr::LineSeg2D(-10, 0, 10, 0));
    m_canvas.add(new vvr::LineSeg2D(0, -10, 0, 10));
}

void LilacChaserScene::reset()
{
    vvr::Scene::reset();
    m_canvas.resize(1);
    m_canvas.newFrame();
}

void LilacChaserScene::draw()
{
    enterPixelMode();
    m_canvas.draw();
    returnFromPixelMode();
}

bool LilacChaserScene::idle()
{
    double sec = vvr::getSeconds();
    int i = (int) (sec / 0.125) % N;
    vvr::Circle2D *c = m_circles.at(i);
    for (vvr::Circle2D *c : m_circles) {
        c->setColour(col_lilac);
    }
    c->setColour(m_bg_col);
    return true;
}

void LilacChaserScene::mousePressed(int x, int y, int modif)
{

}

void LilacChaserScene::mouseMoved(int x, int y, int modif)
{

}

void LilacChaserScene::mouseWheel(int dir, int modif)
{
    const double coef = dir > 0 ? 1.2 : 1.0 / 1.2;

    if (shiftDown(modif))
    {
        for (vvr::Circle2D *c : m_circles) {
            math::vec v(c->x, c->y, 0);
            v *= coef; c->x = v.x; c->y = v.y;
        }
    }
    else
    {
        for (vvr::Circle2D *c : m_circles) {
            c->r *= coef;
        }
    }
}

void LilacChaserScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir==vvr::LEFT) {

    }
    else if (dir==vvr::RIGHT) {

    }
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new LilacChaserScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}

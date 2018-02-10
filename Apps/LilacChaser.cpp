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
    void mouseWheel(int dir, int modif) override;

private:
    vvr::Canvas m_canvas;
    std::vector<vvr::Circle2D> m_circles;

private:
    const float R = 120;
    const float r = 20;
    const unsigned N = 12;
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
        vvr::Circle2D c(x, y, r, vvr::lilac);
        c.filled = true;
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
    for (vvr::Circle2D &c : m_circles) c.draw();
    exitPixelMode();
}

bool LilacChaserScene::idle()
{
    double sec = vvr::getSeconds();
    for (vvr::Circle2D &c : m_circles) c.colour = vvr::lilac;
    m_circles.at((size_t)(sec / 0.125) % N).colour = m_bg_col;
    return true;
}

void LilacChaserScene::mouseWheel(int dir, int modif)
{
    double coef = dir > 0 ? 1.2 : 1.0 / 1.2;

    if (shiftDown(modif))
    {
        for (vvr::Circle2D &c : m_circles) {
            c.SetCentre(c.GetCentre() * coef);
        }
    }
    else
    {
        for (vvr::Circle2D &c : m_circles) {
            c.SetRadius(c.GetRadius() * coef);
        }
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

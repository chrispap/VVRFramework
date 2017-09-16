#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/picking.h>
#include <iostream>
#include <fstream>
#include <string>

class Simple2DScene : public vvr::Scene
{
public:
    Simple2DScene();

private:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override  { m_picker->pick(x,y,modif); }
    void mouseMoved(int x, int y, int modif) override    { m_picker->move(x,y,modif); }
    void mouseReleased(int x, int y, int modif) override { m_picker->drop(x,y,modif); }
    const char* getName() const override { return "Simple 2D Drawing"; }

private:
    typedef vvr::CascadePicker2D<
    vvr::MousePicker2D<vvr::Point3D>,
    vvr::MousePicker2D<vvr::Circle2D>
    > picker_t;

    picker_t::Ptr m_picker;
    vvr::Canvas m_canvas;
};

Simple2DScene::Simple2DScene()
{
    m_bg_col = vvr::grey;
    m_canvas.add(new vvr::Circle2D( -40, -20, 40, vvr::red));
    m_canvas.add(new vvr::Circle2D( -20,  20, 40, vvr::green));
    m_canvas.add(new vvr::Circle2D(   0, -20, 40, vvr::blue));
    m_canvas.add(new vvr::Circle2D(  20,  20, 40, vvr::black));
    m_canvas.add(new vvr::Circle2D(  40, -20, 40, vvr::yellow));
    m_canvas.add(new vvr::Point3D(40, -20, 0 , vvr::white));
    m_picker = picker_t::Make(m_canvas);
}

void Simple2DScene::draw()
{
    enterPixelMode();
    m_canvas.draw();
    exitPixelMode();
}

void Simple2DScene::reset()
{
    vvr::Scene::reset();
    m_canvas.clear();
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new Simple2DScene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
    }
    return 1;
}

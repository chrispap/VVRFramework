#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/picking.h>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>

class Simple2DScene : public vvr::Scene
{
public:
    Simple2DScene();

private:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    const char* getName() const override { return "Simple 2D Drawing"; }

private:
    typedef vvr::CascadePicker2D<
    vvr::MousePicker2D<vvr::Point3D>,
    vvr::MousePicker2D<vvr::Circle2D>
    > PickerT;
   
    vvr::Canvas m_canvas;
    PickerT::Ptr m_picker;
};

Simple2DScene::Simple2DScene()
{
    using namespace std::placeholders;
    m_bg_col = vvr::grey;
    m_canvas.add(new vvr::Circle2D(-40, -20, 40, vvr::red));
    m_canvas.add(new vvr::Circle2D(-20, 20, 40, vvr::green));
    m_canvas.add(new vvr::Circle2D(0, -20, 40, vvr::blue));
    m_canvas.add(new vvr::Circle2D(20, 20, 40, vvr::black));
    m_canvas.add(new vvr::Circle2D(40, -20, 40, vvr::yellow));
    m_canvas.add(new vvr::Point3D(40, -20, 0, vvr::white));
    m_picker = PickerT::Make(m_canvas);
}

void Simple2DScene::mousePressed(int x, int y, int modif)  
{
    m_picker->pick(vvr::Mousepos{ x, y }, modif);
}

void Simple2DScene::mouseMoved(int x, int y, int modif)  
{
    m_picker->drag(vvr::Mousepos{ x, y }, modif);
}

void Simple2DScene::mouseReleased(int x, int y, int modif)  
{
    m_picker->drop(); 
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

vvr_invoke_main_with_scene(Simple2DScene)

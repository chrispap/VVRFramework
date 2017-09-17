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
   
    vvr::Canvas _canvas;
    PickerT::Ptr _picker;
};

Simple2DScene::Simple2DScene()
{
    using namespace std::placeholders;
    m_bg_col = vvr::grey;
    _canvas.add(new vvr::Circle2D(-40, -20, 40, vvr::red));
    _canvas.add(new vvr::Circle2D(-20, 20, 40, vvr::green));
    _canvas.add(new vvr::Circle2D(0, -20, 40, vvr::blue));
    _canvas.add(new vvr::Circle2D(20, 20, 40, vvr::black));
    _canvas.add(new vvr::Circle2D(40, -20, 40, vvr::yellow));
    _canvas.add(new vvr::Point3D(40, -20, 0, vvr::white));
    _picker = PickerT::Make(_canvas);
}

void Simple2DScene::mousePressed(int x, int y, int modif)  
{
    _picker->pick(x, y, modif); 
}

void Simple2DScene::mouseMoved(int x, int y, int modif)  
{
    _picker->drag(x, y, modif); 
}

void Simple2DScene::mouseReleased(int x, int y, int modif)  
{
    _picker->drop(x, y, modif); 
}

void Simple2DScene::draw()
{
    enterPixelMode();
    _canvas.draw();
    exitPixelMode();
}

void Simple2DScene::reset()
{
    vvr::Scene::reset();
    _canvas.clear();
}

vvr_invoke_main_with_scene(Simple2DScene)

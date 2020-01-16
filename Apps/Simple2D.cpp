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

/*---[Simple2DScene]--------------------------------------------------------------------*/
class Simple2DScene : public vvr::Scene
{
public:
    Simple2DScene();

private:
    void draw() override;
    void reset() override;
    void mouseHovered(int x, int y, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    const char* getName() const override { return "Simple 2D Drawing"; }

private:
    typedef vvr::PriorityPicker2D<
    vvr::MousePicker2D<vvr::Point3D>,
    vvr::MousePicker2D<vvr::Circle2D>
    > PickerT;

    vvr::Canvas m_canvas;
    PickerT::Ptr m_picker;
};

/*--------------------------------------------------------------------------------------*/
Simple2DScene::Simple2DScene()
{
    using namespace std::placeholders;
    m_bg_col = vvr::grey;

    auto bp0 = new vvr::Point3D( {0,    0,      0});
    auto bp1 = new vvr::Point3D( {640,  0,      0});
    auto bp2 = new vvr::Point3D( {640,  -480,    0});
    auto bp3 = new vvr::Point3D( {0,    -480,    0});

    m_canvas.add(new vvr::CompositeLine({bp0, bp1}, vvr::black));
    m_canvas.add(new vvr::CompositeLine({bp1, bp2}, vvr::black));
    m_canvas.add(new vvr::CompositeLine({bp2, bp3}, vvr::black));
    m_canvas.add(new vvr::CompositeLine({bp3, bp0}, vvr::black));

    m_canvas.add(bp0);
    m_canvas.add(bp1);
    m_canvas.add(bp2);
    m_canvas.add(bp3);

    auto fp0 = (vvr::Point3D*) m_canvas.add(new vvr::Point3D({449.0, -250.0, 0}, vvr::red));
    auto fp1 = (vvr::Point3D*) m_canvas.add(new vvr::Point3D({140.0, -210.0, 0}, vvr::green));
    auto fp2 = (vvr::Point3D*) m_canvas.add(new vvr::Point3D({102.0, -291.0, 0}, vvr::blue));
    auto fp3 = (vvr::Point3D*) m_canvas.add(new vvr::Point3D({423.0, -331.0, 0}, vvr::orange));
    //auto fp4 = (vvr::Point3D*) m_canvas.add(new vvr::Point3D({, 0}, vvr::magenta));

    m_canvas.add(new vvr::CompositeLine({fp0, fp1}, vvr::Gray));
    m_canvas.add(new vvr::CompositeLine({fp1, fp2}, vvr::Gray));
    m_canvas.add(new vvr::CompositeLine({fp2, fp3}, vvr::Gray));
    m_canvas.add(new vvr::CompositeLine({fp3, fp0}, vvr::Gray));
    //m_canvas.add(new vvr::CompositeLine({fp4, fp0}, vvr::Gray));

    m_picker = PickerT::Make(m_canvas);
}

void Simple2DScene::mouseHovered(int x, int y, int modif)
{
    m_picker->do_pick(vvr::Mousepos{ x, y }, modif);
}

void Simple2DScene::mousePressed(int x, int y, int modif)
{
    m_picker->do_pick(vvr::Mousepos{ x, y }, modif, true);
}

void Simple2DScene::mouseMoved(int x, int y, int modif)
{
    m_picker->do_drag(vvr::Mousepos{ x, y }, modif);
}

void Simple2DScene::mouseReleased(int x, int y, int modif)
{
    m_picker->do_drop();
}

void Simple2DScene::draw()
{
    enterPixelMode();
    m_canvas.draw();
    if (m_picker->get_picked()) {
        m_picker->get_picked()->draw();
        cursorGrab();
    } else cursorHand();
    exitPixelMode();
}

void Simple2DScene::reset()
{
    vvr::Scene::reset();
    m_canvas.clear();
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(Simple2DScene)
#endif

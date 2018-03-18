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

    /* Create circles in random positions. */
    for (int i=0; i<11; i++) {
        int x = rand() % 800 - 400;
        int y = rand() % 600 - 300;
        m_canvas.add(new vvr::Circle2D(x, y, 20, vvr::white));
    }
    for (int i=0; i<11; i++) {
        int x = rand() % 800 - 400;
        int y = rand() % 600 - 300;
        m_canvas.add(new vvr::Point3D(x, y, 0, vvr::white));
    }

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

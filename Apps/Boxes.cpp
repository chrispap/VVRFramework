#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/palette.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <set>
#include <vector>
#include <MathGeoLib.h>

/*--------------------------------------------------------------------------------------*/
class BoxesScene : public vvr::Scene
{
public:
    BoxesScene();
    const char* getName() const override { return "Boxes Scene"; }
    void draw() override;
    void resize() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void setBoxFromCurrentView();
    vvr::Canvas     mCanvas;
    vvr::Aabb3D*    mAabb1;
    vvr::Aabb3D*    mAabb2;
    vvr::Obb3D*     mBox;
};

BoxesScene::BoxesScene()
{
    m_bg_col = vvr::Colour("768E77");
    m_perspective_proj = false;
    m_fullscreen = false;
    mBox = new vvr::Obb3D();
    mAabb1 = new vvr::Aabb3D(0, 0, 0, 10, 10, 10, vvr::red);
    mAabb2 = new vvr::Aabb3D(-10, -10, -10, 5, 5, 5, vvr::green);
    mBox->filled = true;
    mBox->colour = vvr::BurlyWood;
    mBox->col_edge = vvr::BurlyWood;
    mAabb1->setTransparency(0.5);
    mAabb2->setTransparency(0.5);
}

void BoxesScene::resize()
{
    if (m_first_resize)
    {
        mCanvas.add(mAabb1)->hide();
        mCanvas.add(mAabb2)->hide();
        mCanvas.add(mBox)->show();
        setBoxFromCurrentView();
    }
}

void BoxesScene::draw()
{
    getGlobalAxes().drawif();
    mCanvas.draw();
}

void BoxesScene::setBoxFromCurrentView()
{
    vec lo(0, 0, 0);
    vec hi(2, 4, 8);
    math::AABB aabb(lo, hi);
    math::float4x4 transform;
    transform = math::float4x4::RotateFromTo(vec(0, 0, 1), getFrustum().Front());
    mBox->set(aabb, transform);
}

void BoxesScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case 'a':
        getGlobalAxes().toggle();
        break;

    case 'b':
        mAabb1->toggle();
        mAabb2->toggle();
        break;

    case 'o':
        mBox->toggle();

    case 'm':
        setBoxFromCurrentView();
        break;
    }
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(BoxesScene)
#endif

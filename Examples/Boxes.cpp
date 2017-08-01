#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <set>
#include <vector>
#include <MathGeoLib.h>

/*--- Scene Implementation ------------------------------------------------------------*/

using math::vec;

class BoxesScene : public vvr::Scene
{
public:
    BoxesScene();
    const char* getName() const { return "Boxes Scene"; }
    void draw() override;
    void resize() override;
    void reset() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void setBoxFromCurrentView();
    vvr::Canvas mCanvas;
    vvr::Axes *mAxes;
    vvr::Aabb3D *mAabb1;
    vvr::Aabb3D *mAabb2;
    vvr::Obb3D* mBox;
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
    mAabb1->setTransparency(0.5);
    mAabb2->setTransparency(0.5);
}

void BoxesScene::reset()
{
    Scene::reset();
    setBoxFromCurrentView();
}

void BoxesScene::resize()
{
    static bool first_pass = true;
    if (first_pass) {
        mAxes = getGlobalAxes();
        mCanvas.add(mAxes)->show();
        mCanvas.add(mAabb1)->hide();
        mCanvas.add(mAabb2)->hide();
        mCanvas.add(mBox)->show();
        reset();
    }
    first_pass = false;
}

void BoxesScene::draw()
{
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

void BoxesScene::mousePressed(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);
    return Scene::mousePressed(x, y, modif);
}

void BoxesScene::mouseMoved(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);
    return Scene::mouseMoved(x, y, modif);
}

void BoxesScene::mouseReleased(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);
    return Scene::mouseReleased(x, y, modif);
}

void BoxesScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case 'a': 
        mAxes->toggleVisibility();
        break;
    
    case 'b': 
        mAabb1->toggleVisibility();
        mAabb2->toggleVisibility();
        break;
    
    case 'o':
        mBox->toggleVisibility();
    
    case 'm': 
        setBoxFromCurrentView(); 
        break;
    }
}

/*-------------------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new BoxesScene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

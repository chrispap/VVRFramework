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

class BoxesScene : public vvr::Scene
{
public:
    BoxesScene();
    const char* getName() const { return "Boxes Scene"; }

protected:
    void draw() override;
    void resize() override;
    void reset() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;

private:
    void load3DModels();

private:
    int m_disp_flag;
};

#define FLAG_SHOW_AXES 1
#define FLAG_SHOW_AABB 2
#define FLAG_SHOW_WIRE 4
#define FLAG_SHOW_SOLID 8
#define FLAG_SHOW_NORMALS 16

#define objName "cube.obj"
#define objName "bunny_low.obj"
#define objName "unicorn_low.obj"
#define objName "dragon_low_low.obj"
#define objName "large/unicorn.obj"
#define objName "ironman.obj"

using namespace vvr;
using namespace std;
using namespace math;

BoxesScene::BoxesScene()
{
    m_bg_col = Colour("768E77");
    m_perspective_proj = false;
    m_fullscreen = true;
    
    m_disp_flag = 0;
    m_disp_flag |= FLAG_SHOW_SOLID;
    m_disp_flag |= FLAG_SHOW_WIRE;
}

void BoxesScene::reset()
{
    Scene::reset();
    auto pos = getFrustum().Pos();
    pos.y += 10;
    pos.z -= 40;
    setCameraPos(pos);
}

void BoxesScene::resize()
{
    static bool first_pass = true;
    if (first_pass) reset();
    first_pass = false;
}

void BoxesScene::draw()
{
    if (m_disp_flag & FLAG_SHOW_AXES) drawAxes();

    vvr::Box3D b1(0, 0, 0, 10, 10,10, Colour::red);
    vvr::Box3D b2(-10, -10, -10, 5,5,5, Colour::green);
    b1.setTransparency(.5);
    b2.setTransparency(.5);
    b1.draw();
    b2.draw();
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
    case 'a': m_disp_flag ^= FLAG_SHOW_AXES; break;
    case 'w': m_disp_flag ^= FLAG_SHOW_WIRE; break;
    case 's': m_disp_flag ^= FLAG_SHOW_SOLID; break;
    case 'n': m_disp_flag ^= FLAG_SHOW_NORMALS; break;
    case 'b': m_disp_flag ^= FLAG_SHOW_AABB; break;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new BoxesScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}

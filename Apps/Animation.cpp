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
#include <vector>
#include <string>
#include <MathGeoLib.h>

class AnimationScene : public vvr::Scene
{
public:
    AnimationScene();
    const char* getName() const override { return "Animation Scene"; }

protected:
    void draw() override;
    void resize() override;
    void reset() override;
    bool idle() override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    float m_sphere_rad;
    bool m_pause;
};

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

using namespace vvr;
using namespace std;
using namespace math;

AnimationScene::AnimationScene()
{
    m_bg_col = Colour("768E77");
    m_perspective_proj = true;
    m_fullscreen = true;
    m_pause = false;
}

void AnimationScene::reset()
{
    Scene::reset();
}

void AnimationScene::resize()
{
    // Making FIRST PASS static and initialing it to true we make
    // sure that the if block will be executed only once.
    static bool first_pass = true;

    if (first_pass)
    {
        m_sphere_rad = getSceneWidth() / 10;

        first_pass = false;
    }
}

void AnimationScene::draw()
{
    vvr::Sphere3D sphere(0, 0, 0, m_sphere_rad, Colour(134, 100, 25));
    sphere.filled = true;
    sphere.draw();
}

bool AnimationScene::idle()
{
    if (m_pause) return false;

    const float sec = vvr::getSeconds();

    if (m_sphere_rad > getSceneWidth() / 4)
    {
        m_sphere_rad = getSceneWidth() / 10;
    }
    else
    {
        m_sphere_rad += sec * 0.1 + 1;
    }

    return !m_pause;
}

void AnimationScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case ' ': m_pause = !m_pause; break;
    }

}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::main_with_scene(argc, argv, new AnimationScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}

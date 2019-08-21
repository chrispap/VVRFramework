#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/animation.h>
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
    vvr::Sphere3D sphere;
    vvr::Animation anim;
};

AnimationScene::AnimationScene()
{
    m_perspective_proj = true;
    reset();
}

void AnimationScene::reset()
{
    Scene::reset();
    sphere = vvr::Sphere3D(0, 0, 0, 0);
    sphere.colour = vvr::BlueViolet;
    sphere.filled = true;
    anim.setTime(0);
}

void AnimationScene::resize()
{
    if (m_first_resize) {
        sphere.r = getSceneWidth() / 10;
    }
}

void AnimationScene::draw()
{
    sphere.draw();
    getGlobalAxes().draw();
}

bool AnimationScene::idle()
{
    anim.update();
    if (anim.paused()) return false;
    if (sphere.r > getSceneWidth() / 4) anim.setTime(0);
    sphere.r = anim.t() * 15;
    return true;
}

void AnimationScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);
    switch (key)
    {
    case ' ': anim.toggle(); break;
    case 's': sphere.filled^=1; break;
    }
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(AnimationScene)
#endif

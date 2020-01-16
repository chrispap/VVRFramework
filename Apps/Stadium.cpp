#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/picking.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <set>
#include <vector>
#include <MathGeoLib.h>

static const float nfl_stadium_width = 48.75;
static const float nfl_stadium_length = 109.75;

struct FieldDragger
{
    vvr_decl_shared_ptr(FieldDragger)

    using Drawable = vvr::Drawable;
    using Ray = math::Ray;

    bool on_pick(Drawable* drw, Ray ray);
    void on_drag(Drawable* drw, Ray ray0, Ray ray1);
    void on_drop(Drawable* drw);

    vvr::LineSeg3D lineOfSight;
};

class StadiumScene : public vvr::Scene
{
public:
    StadiumScene();
    const char* getName() const override {
        return "NFL Stadium";
    }

private:
    void resize() override;
    void reset() override;
    void draw() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void sliderChanged(int slider_id, float val) override;

private:
    void createField();
    void setCameraPos(const vec &pos);

private:
    typedef vvr::MousePicker3D<FieldDragger> PickerT;
    FieldDragger::Ptr m_dragger;
    PickerT::Ptr m_picker;
    vvr::Canvas m_field;
    vvr::Canvas m_bounds;
    vvr::Canvas m_yardlines;
    vvr::Quad3D::Ptr m_grass;
};

StadiumScene::StadiumScene()
{
    m_bg_col = vvr::Colour("768E77");
    m_perspective_proj = true;
    m_show_sliders = true;
    m_show_log = true;

    createField();
    m_dragger = FieldDragger::Make();
    m_picker = PickerT::Make(m_field, m_dragger.get());
}

void StadiumScene::resize()
{
    if (!m_first_resize) return;
    reset();
}

void StadiumScene::reset()
{
    Scene::reset();
    vec camPos;
    camPos.x = 0;
    camPos.y = 1.80f;
    camPos.z = nfl_stadium_width;
    setCameraPos(camPos);
}

void StadiumScene::draw()
{
    auto fru = getFrustum();
    fru.SetViewPlaneDistances(1, 300);
    setFrustum(fru);
    getGlobalAxes().draw();
    m_field.draw();
    m_dragger->lineOfSight.drawif();
}

void StadiumScene::mousePressed(int x, int y, int modif)
{
    m_picker->do_pick(unproject(x, y), modif);
    if (m_picker->picked()) return;

    Scene::mousePressed(x, y, modif);
}

void StadiumScene::mouseMoved(int x, int y, int modif)
{
    if (m_picker->picked()) {
        m_picker->do_pick(unproject(x, y), modif);
        return;
    }

    Scene::mouseMoved(x, y, modif);
    auto camPos = getFrustum().Pos();
    vvr_echo(camPos);
}

void StadiumScene::mouseReleased(int x, int y, int modif)
{
    m_picker->do_drop(unproject(x, y), modif);
    Scene::mouseReleased(x, y, modif);
}

void StadiumScene::mouseWheel(int dir, int modif)
{
    auto fru = getFrustum();
    const vec newPos = fru.Pos() + (fru.Front() * (5.0f * math::Sign((float)dir)));
    fru.SetPos(newPos);
    setFrustum(fru);
}

void StadiumScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
}

void StadiumScene::sliderChanged(int slider_id, float val)
{
    if (slider_id >=0 && slider_id <= 2) {
        auto fru = getFrustum();
        vec camPos = fru.Pos();
        camPos[slider_id] = 300.0f * (val-0.5f);
        setCameraPos(camPos);
    }
}

void StadiumScene::createField()
{
    const float w = nfl_stadium_width;
    const float l = nfl_stadium_length;
    const float e = 0.001f;

    float W = l + 1;
    float B = w + 1;
    float D = -0.01f; // 5cm
    float T = 2;

    m_grass = vvr::Quad3D::Make(vec::zero, vec::unitY, 10.0f, vvr::DarkGreen);
    m_grass->hsz.x = l / 2;
    m_grass->hsz.y = w / 2;

    const vec c1 = { l/2, e,  w/2}, c2 = { l/2, e, -w/2};
    const vec c3 = {-l/2, e, -w/2}, c4 = {-l/2, e,  w/2};
    m_bounds.add(new vvr::LineSeg3D({c1, c2}, vvr::White));
    m_bounds.add(new vvr::LineSeg3D({c2, c3}, vvr::White));
    m_bounds.add(new vvr::LineSeg3D({c3, c4}, vvr::White));
    m_bounds.add(new vvr::LineSeg3D({c4, c1}, vvr::White));

    for (int i=-4; i<=4; i++) {
        const float tenYards = 9.14400f;
        const vec p1 = {tenYards * i, e, w/2};
        const vec p2 = {tenYards * i, e, -w/2};
        m_yardlines.add(new vvr::LineSeg3D({p1, p2}, vvr::White));
    }

    m_field.setDelOnClear(false);
    m_field.add(m_grass.get());
    m_field.add(&m_bounds);
    m_field.add(&m_yardlines);
}

void StadiumScene::setCameraPos(const vec &camPos)
{
    /* Don't go under the ground or too close to center */
    if (camPos.y > 0 && camPos.Length() > 10) {
        Scene::setCameraPos(camPos);
    }
    vvr_echo(camPos);
}

void markPointOnField(vvr::Quad3D const& quad, vvr::LineSeg3D &line, math::Ray const& ray)
{
    float d;
    quad.Intersects(ray, &d);
    line.a = ray.pos + ray.dir * d;
    line.b = line.a + vec{0,2,0};
    vvr_echo(line.a);
}

bool FieldDragger::on_pick(vvr::Drawable* drw, Ray ray)
{
    markPointOnField(static_cast<vvr::Quad3D&>(*drw), lineOfSight, ray);
    return true;
}

void FieldDragger::on_drag(vvr::Drawable* drw, Ray ray0, Ray ray)
{
    return;
}

void FieldDragger::on_drop(vvr::Drawable* drw)
{
    return;
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(StadiumScene)
#endif

#include "Math/MathConstants.h"
#include "vvr/animation.h"
#include "vvr/palette.h"
#include <vvr/counter.h>
#include <vvr/drawing.h>
#include <vvr/mesh.h>
#include <vvr/picking.h>
#include <vvr/scene.h>
#include <vvr/settings.h>
#include <vvr/utils.h>

/*---[RollingDisks]---------------------------------------------------------------------*/
class RollingDisksScene : public vvr::Scene {
public:
  RollingDisksScene();

private:
  void draw() override;
  void reset() override;
  void keyEvent(unsigned char, bool, int) override;
  bool idle() override;
  const char *getName() const override;

private:
  vvr::Canvas m_canvas;
  vvr::Circle2D m_circle0;
  vvr::Circle2D m_circle1;
  vvr::Animation m_anim;
};

/*--------------------------------------------------------------------------------------*/
const char *RollingDisksScene::getName() const { return "Rolling Disks"; }

RollingDisksScene::RollingDisksScene() {
  m_bg_col = vvr::grey;
  m_fullscreen = false;
  m_anim.toggle();

  m_circle0.colour = vvr::red;
  m_circle1.colour = vvr::green;

  vvr::Shape::SetPointSize(2);
  vvr::Shape::SetLineWidth(2);
}

bool RollingDisksScene::idle() {
  // ScopedCounter counter{"idle"};

  using math::DegToRad;

  const float r0 = 200;
  const float r1 = r0 * 0.45;
  const float rdot = 2;
  const float rps_lin = DegToRad(30 * 8);
  const float rps_cen = rps_lin * (r1 / r0);
  const float rps_self = rps_lin * ((r1 / r0) + 1);
  const unsigned ndots = 1;
  const unsigned tailSize = 200;

  m_anim.update();
  const float t{m_anim.t()};

  m_circle0.setGeom({{0, 0}, r0});
  m_circle1.setGeom({{0, (r0 + r1)}, r1});
  m_circle1.RotateToRight(t * (rps_cen), {0, 0});

  m_canvas.newFrame(true);

  for (int i = 0; i < ndots; i++) {
    auto v = C2DVector{0, (r1 + rdot)};
    auto dot_color = i ? vvr::black : vvr::blue;
    v.TurnRight((math::pi * 2) / ndots * i);
    C2DPoint cen = m_circle1.GetCentre() + v;
    vvr::Circle2D *dot = new vvr::Circle2D({cen, rdot}, dot_color);
    dot->RotateToRight(t * rps_self, m_circle1.GetCentre());
    m_canvas.add(dot);
  }

  return !m_anim.paused();
  return true;
}

void RollingDisksScene::keyEvent(unsigned char key, bool up, int modif) {
  Scene::keyEvent(key, up, modif);
  key = tolower(key);
  switch (key) {
  case ' ':
    m_anim.toggle();
    break;
  case 'r':
    reset();
    break;
  case 'c':
    m_circle0.toggle();
    break;
  case 'v':
    m_circle1.toggle();
    break;
  }
}

void RollingDisksScene::draw() {
  // ScopedCounter counter{"drawing"};
  enterPixelMode();
  m_circle0.drawif();
  m_circle1.drawif();
  m_canvas.draw();
  exitPixelMode();
}

void RollingDisksScene::reset() {
  vvr::Scene::reset();
  m_canvas.clear();
  m_anim.reset();
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(RollingDisksScene)
#endif

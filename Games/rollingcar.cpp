
#include "Math/MathConstants.h"
#include "Math/MathFunc.h"
#include "vvr/dragging.h"
#include <C2DLine.h>
#include <C2DPoint.h>
#include <C2DVector.h>
#include <MathGeoLibFwd.h>
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <vector>
#include <vvr/animation.h>
#include <vvr/counter.h>
#include <vvr/drawing.h>
#include <vvr/macros.h>
#include <vvr/mesh.h>
#include <vvr/palette.h>
#include <vvr/picking.h>
#include <vvr/scene.h>
#include <vvr/settings.h>
#include <vvr/utils.h>

#define BENCHMARKS_ON 0

const vvr::Colour colours[] = {
  vvr::red,
  vvr::green,
  vvr::blue,
  vvr::yellow,
  vvr::cyan,
  vvr::magenta,
  vvr::orange,
  vvr::DeepPink,
  vvr::ForestGreen,
  vvr::white,
  vvr::grey,
  vvr::black,
};

static int randomColourIndex = 0;

template <typename T>
T
step(T value, T step)
{
  return value - value % step;
}

vvr::Colour
randomColour()
{
  return colours[randomColourIndex++ % (sizeof(colours) / sizeof(colours[0]))];
}

/*---[Wheel]-----------------------------------------------------------------*/

struct Wheel : vvr::Drawable
{
  vvr_decl_shared_ptr(Wheel);

  Wheel(float radius, double wheel_speed,
    std::vector<vvr::Point3D *> const &road_pts, int road_seg);

  void update(float dt, std::vector<vvr::Point3D *> const &road_pts);
  void move(C2DVector dir);
  void turn(float rad, C2DPoint const &pivot);
  void place(C2DPoint const &ground, C2DVector normal);
  void setSpeed(double speed) { this->speed = speed; }
  void setRadius(double radius);
  void draw() const override;

  auto getHub() const { return tire.GetCentre(); }
  auto getRadius() const { return tire.GetRadius(); }
  auto getCircumference() const { return circumference; }

private:
  int road_seg;
  float speed;
  float angle;
  float circumference;
  vvr::Circle2D tire;
};

Wheel::Wheel(float radius, double wheel_speed,
  std::vector<vvr::Point3D *> const &road_pts, int road_seg)
  : road_seg(road_seg)
{
  angle = 0;
  speed = wheel_speed;
  circumference = 2 * M_PI * radius;
  C2DPoint ground{road_pts[road_seg]->x, road_pts[road_seg]->y};
  C2DPoint center = ground + C2DVector(0, radius);

  tire = vvr::Circle2D({center, radius}, randomColour());

  const C2DPoint p1{road_pts[road_seg + 0]->x, road_pts[road_seg + 0]->y};
  const C2DPoint p2{road_pts[road_seg + 1]->x, road_pts[road_seg + 1]->y};
  const C2DLine seg(p1, p2);
  C2DVector normal = seg.vector;
  normal.TurnLeft(M_PI_2);
  place(seg.point, normal);
}

void
Wheel::update(float dt, std::vector<vvr::Point3D *> const &road_pts)
{
  auto &p1 = road_pts[road_seg + 0];
  auto &p2 = road_pts[road_seg + 1];

  if (road_seg == road_pts.size() - 2) {
    math::float3 dv = ((*p2) - (*p1)).ScaledToLength(dt * speed);
    move(C2DVector{dv.x, dv.y});

    const C2DLine seg1(C2DPoint{p1->x, p1->y}, C2DPoint{p2->x, p2->y});
    const double proj1 = this->getHub().Project(seg1) / seg1.GetLength();

    if (proj1 > 1) {
      road_seg = 0;
      const C2DPoint p1{road_pts[road_seg + 0]->x, road_pts[road_seg + 0]->y};
      const C2DPoint p2{road_pts[road_seg + 1]->x, road_pts[road_seg + 1]->y};
      const C2DLine seg(p1, p2);
      C2DVector normal = seg.vector;
      normal.TurnLeft(M_PI_2);
      place(seg.point, normal);
    }
  } else if (road_seg < road_pts.size() - 1) {
    auto &p3 = road_pts[road_seg + 2];

    const C2DLine seg1(C2DPoint{p1->x, p1->y}, C2DPoint{p2->x, p2->y});
    const C2DLine seg2(C2DPoint{p2->x, p2->y}, C2DPoint{p3->x, p3->y});
    const float angleL = seg1.vector.AngleToLeft(seg2.vector);
    const float angleR = seg1.vector.AngleToRight(seg2.vector);

    if (angleR < angleL) {
      // Here we handle the case where the wheel is on a hill
      // and the next segment is relatively downhill.

      const double proj1 = this->getHub().Project(seg1) / seg1.GetLength();
      const double proj2 = this->getHub().Project(seg2) / seg2.GetLength();

      if (proj1 > 1 && proj2 >= 0) {
        C2DVector normal = seg2.vector;
        normal.TurnLeft(M_PI_2);
        this->place(seg2.point, normal);
        road_seg++;
      } else if (proj1 > 1) {
        const auto rot_speed = speed / this->getCircumference() * 2 * M_PI;
        this->turn(dt * rot_speed, seg2.point);
      } else {
        math::float3 dv = ((*p2) - (*p1)).ScaledToLength(dt * speed);
        this->move(C2DVector{dv.x, dv.y});
      }

    } else {
      // Here we handle the case where the wheel is on a hill
      // and the next segment is relatively uphill.

      const float dist1 = seg1.Distance(this->getHub());
      const float dist2 = seg2.Distance(this->getHub());

      if (dist2 <= dist1) {
        road_seg++;
      } else {
        math::float3 dv = ((*p2) - (*p1)).ScaledToLength(dt * speed);
        this->move(C2DVector{dv.x, dv.y});
      }
    }
  }
}

void
Wheel::move(C2DVector dir)
{
  tire.Move(dir);
  angle += dir.GetLength() / circumference * 2 * M_PI;
}

void
Wheel::turn(float rad, C2DPoint const &pivot)
{
  tire.RotateToRight(rad, pivot);
  angle += rad;
}

void
Wheel::place(C2DPoint const &ground, C2DVector normal)
{
  normal.SetLength(tire.GetRadius());
  C2DPoint center = ground + normal;
  tire.SetCentre(center);
}

void
Wheel::setRadius(double radius)
{
  this->tire.SetRadius(radius);
  this->circumference = 2 * M_PI * radius;
}

void
Wheel::draw() const
{
  C2DVector spoke = C2DVector(0, -tire.GetRadius());
  spoke.TurnRight(angle);

  vvr::Canvas tmp;
  constexpr int num_spokes = 8;
  for (int i = 0; i < num_spokes; ++i) {
    C2DPoint spoke_end = tire.GetCentre() + spoke;
    C2DLine spoke_line(tire.GetCentre(), spoke_end);
    tmp.add(spoke_line, tire.colour)->draw();
    spoke.TurnRight((M_PI * 2) / num_spokes);
  }

  tmp.add(tire.GetCentre())->draw();
  tire.draw();
}

/*---[Functions]-------------------------------------------------------------*/

auto
track_zigzag()
{
  std::vector<vvr::Point3D *> pts;
  constexpr int dx = 400;
  constexpr int dy = dx * 0.1;
  for (int i = 0; i < 28; ++i) {
    pts.push_back(new vvr::Point3D((i * dx), 0, 0));
    pts.push_back(new vvr::Point3D((i * dx) + (dx / 2.), dy, 0));
  }
  return pts;
}

auto
track_circle(float radius = 200)
{
  std::vector<vvr::Point3D *> pts;
  constexpr int segs = 32;
  for (int i = 0; i < segs; ++i) {
    pts.push_back(new vvr::Point3D(
      radius * sin(i * M_PI / segs * 2), radius * cos(i * M_PI / segs * 2), 0));
  }
  return pts;
}

auto
track_spiral(float radius = 200)
{
  std::vector<vvr::Point3D *> pts;
  constexpr int segs = 32;
  for (int i = 0; i < segs; ++i) {
    pts.push_back(new vvr::Point3D(
      radius * sin(i * M_PI / segs * 2), radius * cos(i * M_PI / segs * 2), 0));
    radius += 10;
  }
  return pts;
}

auto
track1()
{
  return std::vector<vvr::Point3D *>{
    new vvr::Point3D(0, 0, 0),
    new vvr::Point3D(130, 0, 0),
    new vvr::Point3D(300, 10, 0),
    new vvr::Point3D(400, 180, 0),
    new vvr::Point3D(600, 150, 0),
    new vvr::Point3D(800, 40, 0),
    new vvr::Point3D(1000, 0, 0),
    new vvr::Point3D(1000, -200, 0),
    new vvr::Point3D(800, -300, 0),
    new vvr::Point3D(400, -300, 0),
    new vvr::Point3D(0, -500, 0),
    new vvr::Point3D(-200, -500, 0),
    new vvr::Point3D(0, 0, 0),
  };
}

auto
track2()
{
  return std::vector<vvr::Point3D *>{
    new vvr::Point3D(0, 50, 0),
    new vvr::Point3D(200, 10, 0),
    new vvr::Point3D(400, 20, 0),
    new vvr::Point3D(600, 30, 0),
    new vvr::Point3D(900, 0, 0),
    new vvr::Point3D(1200, 40, 0),
    new vvr::Point3D(0, -400, 0),
  };
}

auto
translateTrack(std::vector<vvr::Point3D *> &&road_pts, math::float3 const &dv)
{
  for (auto &pt : road_pts) {
    (*pt) += dv;
  }
  return road_pts;
}

auto
tracks()
{
  typedef std::function<std::vector<vvr::Point3D *>()> TrackFn;
  return std::vector<TrackFn>{
    []() {
      return translateTrack(track1(), {-500, 0, 0});
    },
    []() {
      return translateTrack(track2(), {-500, 0, 0});
    },
    []() {
      return translateTrack(track_circle(), {0, 0, 0});
    },
    []() {
      return translateTrack(track_spiral(), {0, 0, 0});
    },
    []() {
      return translateTrack(track_zigzag(), {-500, 0, 0});
    },
  };
}

void
createRoadFromPts(
  std::vector<vvr::Point3D *> const &road_pts, vvr::Canvas &road)
{
  road.setDelOnClear(false);
  road.clear();

  for (int x = 0; x < road_pts.size() - 1; x++) {
    const auto &p0 = road_pts[x + 0];
    const auto &p1 = road_pts[x + 1];
    auto l = new vvr::CompositeLine({p0, p1}, vvr::DarkGray);
    l->collect(road);
  }
}

/*---[RollingDisks]----------------------------------------------------------*/

class RollingCarScene : public vvr::Scene
{
public:
  const char *getName() const override;
  RollingCarScene();

private:
  void reset() override;
  bool idle() override;
  void draw() override;

  void arrowEvent(vvr::ArrowDir dir, int modif) override;
  void keyEvent(unsigned char, bool, int) override;
  void mouseHovered(int x, int y, int modif) override;
  void mousePressed(int x, int y, int modif) override;
  void mouseMoved(int x, int y, int modif) override;
  void mouseReleased(int x, int y, int modif) override;

private:
  bool autoCenter = true;
  int num_wheels = 2;
  double wheel_speed;
  float wheel_radius;
  vvr::Canvas road;
  vvr::Canvas canvas;
  vvr::Animation anim;
  std::vector<Wheel::Ptr> wheels;
  std::vector<vvr::Point3D *> road_pts;

  typedef vvr::PriorityPicker2D<vvr::MousePicker2D<vvr::Point3D>> PickerT;
  PickerT::Ptr picker;
};

const char *
RollingCarScene::getName() const
{
  return "Rolling Car!";
}

RollingCarScene::RollingCarScene()
{
  vvr::Shape::SetPointSize(12);
  vvr::Shape::SetLineWidth(5);

  m_bg_col = vvr::grey;
  m_fullscreen = false;
  m_show_log = true;

  wheel_speed = 200;
  wheel_radius = 50;

  picker = PickerT::Make(road);

  road_pts = tracks()[4]();
  createRoadFromPts(road_pts, road);

  reset();
}

void
RollingCarScene::reset()
{
  randomColourIndex = 0;
  vvr::Scene::reset();
  anim.reset();
  canvas.clear();

  wheels.clear();
  for (int i = 0; i < num_wheels; ++i) {
    auto wheel = Wheel::Make(wheel_radius, wheel_speed, road_pts, i);
    wheels.push_back(wheel);
  }

  // anim.toggle();
}

bool
RollingCarScene::idle()
{
#if BENCHMARKS_ON
  ScopedCounter counter{"idle"};
#endif

  double dt = anim.update();

  wheel_speed = std::clamp(wheel_speed, 0.0, 2000.0);
  wheel_radius = std::clamp(wheel_radius, 10.0f, 1000.0f);

  for (auto &wheel : wheels) {
    wheel->setSpeed(wheel_speed);
    wheel->setRadius(wheel_radius);
  }

  for (auto &wheel : wheels) {
    wheel->update(dt, road_pts);
  }

  return !anim.paused();
}

void
RollingCarScene::draw()
{
#if BENCHMARKS_ON
  ScopedCounter counter{"drawing"};
#endif

  if (autoCenter) {
    const auto &center = wheels.front()->getHub();
    m_viewcenter_x = math::FloorInt(-center.x);
    m_viewcenter_y = math::FloorInt(-center.y);
  }
  enterPixelMode();

  {
    road.draw();
    canvas.draw();

    vvr::Canvas tmp;
    for (int i = 0; i < wheels.size() - 1; ++i) {
      tmp.add(C2DLine(wheels[i]->getHub(), wheels[i + 1]->getHub()))->draw();
    }

    for (auto &wheel : wheels) {
      wheel->draw();
    }

    drawAxes();
  }

  exitPixelMode();
}

void
RollingCarScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
  constexpr auto dSpeed = 50;
  constexpr auto dRadius = 10;

  switch (dir) {
  case vvr::UP:
    wheel_speed += dSpeed;
    break;
  case vvr::DOWN:
    wheel_speed -= dSpeed;
    break;
  case vvr::LEFT:
    wheel_radius -= dRadius;
    break;
  case vvr::RIGHT:
    wheel_radius += dRadius;
    break;
  }

  vvr_echo(wheel_speed);
}

void
RollingCarScene::keyEvent(unsigned char key, bool up, int modif)
{
  Scene::keyEvent(key, up, modif);

  key = tolower(key);

  switch (key) {
  case ' ':
    anim.toggle();
    break;
  case 'r':
    reset();
    break;
  case 'c':
    autoCenter = !autoCenter;
    break;
  }

  if (key >= '0' && key <= '9') {
    auto track_no = (key - '0') % tracks().size();
    road_pts = tracks()[track_no]();
    createRoadFromPts(road_pts, road);
    reset();
  }
}

constexpr auto hugeint = std::numeric_limits<int>::max();
static vvr::Mousepos dragAnchor{hugeint, hugeint};

void
RollingCarScene::mouseHovered(int x, int y, int modif)
{
  picker->do_pick(vvr::Mousepos{x, y}, modif);
}

void
RollingCarScene::mousePressed(int x, int y, int modif)
{
  if (vvr::Scene::ctrlDown(modif)) {
    if (!picker->do_pick(vvr::Mousepos{x, y}, modif)) {
      dragAnchor = {x, y};
    }
  }
}

void
RollingCarScene::mouseMoved(int x, int y, int modif)
{
  picker->do_drag(vvr::Mousepos{x, y}, modif);

  if (!picker->get_picked()) {
    if (dragAnchor.x != hugeint && dragAnchor.y != hugeint) {
      m_viewcenter_x += x - dragAnchor.x;
      m_viewcenter_y += y - dragAnchor.y;
    }
  }
}

void
RollingCarScene::mouseReleased(int x, int y, int modif)
{
  picker->do_drop();
  dragAnchor = {hugeint, hugeint};
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(RollingCarScene)
#endif


#include "Math/MathFunc.h"
#include "vvr/dragging.h"
#include <C2DLine.h>
#include <C2DPoint.h>
#include <C2DVector.h>
#include <MathGeoLibFwd.h>
#include <algorithm>
#include <box2d/box2d.h>
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
  return floor(value / step) * step;
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
  } else {
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
  tire.SetCentre(ground + normal);
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
track_simple()
{
  return std::vector<vvr::Point3D *>{
    new vvr::Point3D(0., 0.5, 0.),
    new vvr::Point3D(2., 0.1, 0.),
    new vvr::Point3D(4., 0.2, 0.),
    new vvr::Point3D(6., 0.3, 0.),
    new vvr::Point3D(9., 0., 0.),
    new vvr::Point3D(12., 0.4, 0.),
  };
}

auto
track_polygon()
{
  return std::vector<vvr::Point3D *>{
    new vvr::Point3D(-2.0, 0.0, 0.0),
    new vvr::Point3D(-1.0, 0.4, 0.0),
    new vvr::Point3D(0.0, 0.5, 0.0),
    new vvr::Point3D(1.0, 0.2, 0.0),
    new vvr::Point3D(3.0, 0.1, 0.0),
    new vvr::Point3D(2.0, -1.5, 0.0),
    new vvr::Point3D(0.0, -2.5, 0.0),
    new vvr::Point3D(0.5, -1.5, 0.0),
    new vvr::Point3D(-2.0, 0.0, 0.0),
  };
}

auto
track_circle(float radius = 2.0)
{
  std::vector<vvr::Point3D *> pts;
  constexpr int segs = 32;
  for (int i = 0; i < segs; ++i) {
    constexpr auto fullcircle = M_PI * 2;
    const auto x = radius * sin(fullcircle * i / segs);
    const auto y = radius * cos(fullcircle * i / segs);
    pts.push_back(new vvr::Point3D(x, y, 0));
  }
  return pts;
}

auto
track_spiral(float radius = 0.2)
{
  std::vector<vvr::Point3D *> pts;
  constexpr int segs = 32;
  for (int i = 0; i < segs; ++i) {
    constexpr auto fullcircle = M_PI * 2;
    const auto x = radius * sin(fullcircle * i / segs);
    const auto y = radius * cos(fullcircle * i / segs);
    pts.push_back(new vvr::Point3D(x, y, 0));
    radius += 0.1;
  }
  return pts;
}

auto
track_zigzag()
{
  std::vector<vvr::Point3D *> pts;
  constexpr float dx = 2.0;
  constexpr float dy = dx * 0.1;
  for (int i = 0; i < 20; ++i) {
    pts.push_back(new vvr::Point3D((i * dx), 0, 0));
    pts.push_back(new vvr::Point3D((i * dx) + (dx * 0.5), dy, 0));
  }
  return pts;
}

auto
tracks()
{
  typedef std::function<std::vector<vvr::Point3D *>()> TrackFn;
  return std::vector<TrackFn>{
    []() { return track_simple(); },
    []() { return track_polygon(); },
    []() { return track_circle(); },
    []() { return track_spiral(); },
    []() { return track_zigzag(); },
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

constexpr auto hugei = std::numeric_limits<int>::max();
constexpr auto hugef = std::numeric_limits<float>::max();

class RollingCarScene : public vvr::Scene
{
public:
  const char *getName() const override;
  RollingCarScene();

private:
  void reset() override;
  bool idle() override;
  void draw() override;
  void resize() override;
  void arrowEvent(vvr::ArrowDir dir, int modif) override;
  void mouseWheel(int dir, int modif) override;
  void keyEvent(unsigned char, bool, int) override;
  void mouseHovered(int x, int y, int modif) override;
  void mousePressed(int x, int y, int modif) override;
  void mouseMoved(int x, int y, int modif) override;
  void mouseReleased(int x, int y, int modif) override;

private:
  void setupPhysics();

private:
  int numWheels;
  float wheelSpeed;
  float wheelRadius;
  vvr::Canvas road;
  vvr::Canvas canvas;
  vvr::Animation anim;
  std::vector<Wheel::Ptr> wheels;
  std::vector<vvr::Point3D *> roadPts;
  vvr::TargetAnimation<math::float2> worldCenter;
  math::float2 dragAnchor{hugef, hugef};
  math::float2 worldCenterMlstn;
  math::float2 worldSize;
  bool keepWheelCentered = false;
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

  vvr::Scene::m_bg_col = vvr::grey;
  vvr::Scene::m_fullscreen = false;
  vvr::Scene::m_show_log = true;

  roadPts = tracks()[4]();
  createRoadFromPts(roadPts, road);
  setupPhysics();
  reset();
}

void
RollingCarScene::setupPhysics()
{
  b2Vec2 gravity(0.0f, 0.0f);
  b2World world(gravity);
}

void
RollingCarScene::reset()
{
  worldCenter = {{0.f, 0.f}, 10.0f};
  worldSize = {10., 0.}; // Define only the width of the world
  numWheels = 2;
  wheelSpeed = 0;
  wheelRadius = 0.25;
  keepWheelCentered = false;
  randomColourIndex = 0;
  vvr::Scene::reset();
  anim.reset();
  canvas.clear();

  wheels.clear();
  for (int i = 0; i < numWheels; ++i) {
    auto wheel = Wheel::Make(wheelRadius, wheelSpeed, roadPts, i);
    wheels.push_back(wheel);
  }

  anim.toggle();
  resize();
}

bool
RollingCarScene::idle()
{
#if BENCHMARKS_ON
  ScopedCounter counter{"idle"};
#endif

  double dt = anim.update();

  wheelSpeed = std::clamp(wheelSpeed, 0.0f, 16.0f);
  wheelRadius = std::clamp(wheelRadius, 0.01f, 10.0f);

  for (auto &wheel : wheels) {
    wheel->setSpeed(wheelSpeed);
    wheel->setRadius(wheelRadius);
  }

  for (auto &wheel : wheels) {
    wheel->update(dt, roadPts);
  }

  // Center view on hub of first wheel
  if (keepWheelCentered) {
    using math::FloorInt;
    const auto &hub = wheels.front()->getHub();
    const math::float2 newCenter(hub.x, step(hub.y, worldSize.y / 4.0));
    worldCenter.setTarget(newCenter);
  } else {
    worldCenter.snapToTarget();
  }

  return !anim.paused();
}

void
RollingCarScene::draw()
{
#if BENCHMARKS_ON
  ScopedCounter counter{"drawing"};
#endif

  enter2dMode(worldCenter.get(), worldSize);

  vvr::Canvas tmp;

  road.draw();
  canvas.draw();

  for (int i = 0; i < wheels.size() - 1; ++i) {
    tmp.add(C2DLine(wheels[i]->getHub(), wheels[i + 1]->getHub()))->draw();
  }

  for (auto &wheel : wheels) {
    wheel->draw();
  }

  {
    vvr::BackupAndRestore tmp2{vvr::Shape::LineWidth, 2.0f};
    drawAxes();
  }

  exitPixelMode();
}

void
RollingCarScene::resize()
{
  Scene::resize();

  if (m_first_resize) {
    std::cout << "Press 'r' to reset." << std::endl;
    std::cout << "Press 'c' to keep wheel centered." << std::endl;
    std::cout << "Press '0'-'9' to change track." << std::endl;
    std::cout << "Press 'space' to pause." << std::endl;
    std::cout << "Press 'up'/'down' to change speed." << std::endl;
    std::cout << "Press 'left'/'right' to change radius." << std::endl;
  }

  worldSize.y = worldSize.x * getViewportHeight() / getViewportWidth();
}

void
RollingCarScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
  constexpr auto dSpeed = 0.4f;

  switch (dir) {
  case vvr::UP:
    wheelSpeed += dSpeed;
    break;
  case vvr::DOWN:
    wheelSpeed -= dSpeed;
    break;
  case vvr::LEFT:
    wheelSpeed -= dSpeed;
    resize();
    break;
  case vvr::RIGHT:
    wheelSpeed += dSpeed;
    resize();
    break;
  }
}

void
RollingCarScene::mouseWheel(int dir, int modif)
{
  if (dir > 0) {
    worldSize.x /= 1.5;
  } else {
    worldSize.x *= 1.5;
  }
  resize();
}

void
RollingCarScene::keyEvent(unsigned char key, bool up, int modif)
{
  Scene::keyEvent(key, up, modif);
  key = tolower(key);

  std::cout << "Key pressed: '" << key << "'" << std::endl;

  switch (key) {
  case ' ':
    anim.toggle();
    if (anim.paused()) {
      worldCenter.pause();
    } else {
      worldCenter.update(true);
    }
    break;
  case 'r':
    reset();
    break;
  case 'c':
    if (keepWheelCentered == false) {
      keepWheelCentered = true;
      worldCenter.update(true);
    } else {
      keepWheelCentered = false;
    }
    break;
  }

  if (key >= '0' && key <= '9') {
    auto track_no = (key - '0') % tracks().size();
    roadPts = tracks()[track_no]();
    createRoadFromPts(roadPts, road);
    reset();
  }
}

void
RollingCarScene::mouseHovered(int x, int y, int modif)
{}

void
RollingCarScene::mousePressed(int x, int y, int modif)
{
  const float vpw = getViewportWidth();
  const float vph = getViewportHeight();
  const float xf = (((float)x / vpw)) * worldSize.x - worldCenter.get().x;
  const float yf = (((float)y / vph)) * worldSize.y - worldCenter.get().y;

  if (!keepWheelCentered || anim.paused()) {
    dragAnchor = {xf, yf};
    worldCenterMlstn = worldCenter.get();
  }
}

void
RollingCarScene::mouseMoved(int x, int y, int modif)
{
  if (dragAnchor.x != hugef && dragAnchor.y != hugef) {
    const float vpw = getViewportWidth();
    const float vph = getViewportHeight();
    const float xf = (((float)x / vpw)) * worldSize.x - worldCenterMlstn.x;
    const float yf = (((float)y / vph)) * worldSize.y - worldCenterMlstn.y;
    const math::float2 t{xf - dragAnchor.x, yf - dragAnchor.y};
    const math::float2 v{worldCenterMlstn - t};
    worldCenter.setValue(v);
  }
}

void
RollingCarScene::mouseReleased(int x, int y, int modif)
{
  dragAnchor = {hugef, hugef};
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(RollingCarScene)
#endif

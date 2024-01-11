#include "C2DLineSet.h"
#include "box2d/b2_body.h"
#include "box2d/b2_polygon_shape.h"
#include <C2DLine.h>
#include <C2DPoint.h>
#include <C2DVector.h>
#include <MathGeoLibFwd.h>
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

typedef std::vector<vvr::Point3D *> PointVector;

constexpr auto hugei = std::numeric_limits<int>::max();
constexpr auto hugef = std::numeric_limits<float>::max();

namespace {

template <typename T>
T
step(T value, T step)
{
  return floor(value / step) * step;
}

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

vvr::Colour
randomColour()
{
  return colours[randomColourIndex++ % (sizeof(colours) / sizeof(colours[0]))];
}

/*---[Wheel]-----------------------------------------------------------------*/

struct Wheel : vvr::Drawable
{
  vvr_decl_shared_ptr(Wheel);

  Wheel(float radius, C2DPoint const &pos, float angle);

  Wheel(float radius, double wheel_speed, PointVector const &road_pts,
    int road_seg);

  void update(float dt, PointVector const &road_pts);
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

Wheel::Wheel(float radius, C2DPoint const &pos, float angle) : angle(angle)
{
  speed = 0;
  circumference = 2 * M_PI * radius;
  tire = vvr::Circle2D({pos, radius}, vvr::black);
}

Wheel::Wheel(
  float radius, double wheel_speed, PointVector const &road_pts, int road_seg)
  : road_seg(road_seg)
{
  angle = 0;
  speed = wheel_speed;
  circumference = 2 * M_PI * radius;
  C2DPoint ground{road_pts[road_seg]->x, road_pts[road_seg]->y};
  C2DPoint center = ground + C2DVector(0, radius);
  // vvr::Colour colour = randomColour();
  vvr::Colour colour = vvr::red;
  tire = vvr::Circle2D({center, radius}, colour);

  const C2DPoint p1{road_pts[road_seg + 0]->x, road_pts[road_seg + 0]->y};
  const C2DPoint p2{road_pts[road_seg + 1]->x, road_pts[road_seg + 1]->y};
  const C2DLine seg(p1, p2);
  C2DVector normal = seg.vector;
  normal.TurnLeft(M_PI_2);
  place(seg.point, normal);
}

void
Wheel::update(float dt, PointVector const &road_pts)
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
  return PointVector{
    new vvr::Point3D(-5, 0., 0.),
    new vvr::Point3D(0, 0., 0.),
    new vvr::Point3D(5, 0., 0.),
  };
}

auto
track_polygon()
{
  return PointVector{
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
  PointVector pts;
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
  PointVector pts;
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
  PointVector pts;
  constexpr float dx = 3.0;
  constexpr float dy = dx * 0.21;
  for (int i = 0; i < 10; ++i) {
    pts.push_back(new vvr::Point3D((i * dx), 0, 0));
    pts.push_back(new vvr::Point3D((i * dx) + (dx * 0.5), dy, 0));
  }
  return pts;
}

auto
track_sinusoid()
{
  PointVector pts;
  constexpr float dx = 1.0;
  constexpr float dy = dx * 3.0;
  for (int i = 0; i < 1000; ++i) {
    float x = i * dx;
    float y = sin(x * 0.2 - 1.0) * dy + dy;
    pts.push_back(new vvr::Point3D(x, y, 0));
  }
  return pts;
}

auto
tracks()
{
  typedef std::function<PointVector()> TrackFn;
  return std::vector<TrackFn>{
    // []() { return track_simple(); },
    []() { return track_polygon(); },
    []() { return track_circle(); },
    []() { return track_spiral(); },
    []() { return track_zigzag(); },
    []() { return track_sinusoid(); },
  };
}

void
createRoadFromPts(PointVector const &road_pts, vvr::Canvas &road)
{
  road.setDelOnClear(false);
  road.clear();

  for (int x = 0; x < road_pts.size() - 1; x++) {
    const auto &p0 = road_pts[x + 0];
    const auto &p1 = road_pts[x + 1];
    auto l = new vvr::CompositeLine({p0, p1}, vvr::Aquamarine);
    // l->collect(road);
    road.add(l);
  }
}

/*---[Box2D Drawing]---------------------------------------------------------*/

void
drawWheel(b2Body *bodyBall)
{
  auto bodyPos = bodyBall->GetPosition();
  const auto r = bodyBall->GetFixtureList()->GetShape()->m_radius;
  Wheel w(r, {bodyPos.x, bodyPos.y}, -bodyBall->GetAngle());
  w.draw();
}

void
drawBox(b2Body *bodyBox)
{
  vvr::Canvas c;
  auto bodyPos = bodyBox->GetPosition();
  auto bodyAngle = bodyBox->GetAngle();
  auto shape = bodyBox->GetFixtureList()->GetShape();
  auto polyg = dynamic_cast<b2PolygonShape *>(shape);
  C2DLineSet ls;
  C2DPoint boxPts[] = {
    C2DPoint(polyg->m_vertices[0].x, polyg->m_vertices[0].y),
    C2DPoint(polyg->m_vertices[1].x, polyg->m_vertices[1].y),
    C2DPoint(polyg->m_vertices[2].x, polyg->m_vertices[2].y),
    C2DPoint(polyg->m_vertices[3].x, polyg->m_vertices[3].y),
  };
  ls.AddCopy({boxPts[0], boxPts[1]});
  ls.AddCopy({boxPts[1], boxPts[2]});
  ls.AddCopy({boxPts[2], boxPts[3]});
  ls.AddCopy({boxPts[3], boxPts[0]});
  ls.RotateToRight(-bodyAngle, {0, 0});
  ls.Move({bodyPos.x, bodyPos.y});
  for (int i = 0; i < ls.size(); ++i) {
    c.add(*ls.GetAt(i))->draw();
  }
}

} // namespace

/*---[RollingDisks]----------------------------------------------------------*/

class PlayboxScene : public vvr::Scene
{
public:
  const char *getName() const override { return "Playbox"; }
  PlayboxScene();

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
  void setCenterTarget();
  void setupPhysics();
  void simulatePhysics();
  void drawPhysics() const;

private:
  float gbW, gbH;
  float simulationTime;
  vvr::Canvas road;
  vvr::Canvas canvas;
  vvr::Animation anim;
  PointVector roadPts;
  vvr::TargetAnimation<math::float2> worldCenter;
  math::float2 worldCenterAnchor;
  math::float2 dragAnchor{hugef, hugef};
  math::float2 worldSize;
  bool keepCentered;

  // Physics:
  b2World *world = nullptr;
  b2Body *groundBody = nullptr;
  b2Body *ballBody = nullptr;
  std::vector<b2Body *> boxBodies;
};

PlayboxScene::PlayboxScene()
{
  vvr::Shape::SetPointSize(12);
  vvr::Shape::SetLineWidth(5);

  vvr::Scene::m_bg_col = vvr::grey;
  vvr::Scene::m_fullscreen = false;
  vvr::Scene::m_show_log = true;

  worldSize = {100., 0.}; // Define only the width of the world
  gbW = 100.0f;
  gbH = 0.25f;
  roadPts = track_sinusoid();
  createRoadFromPts(roadPts, road);
  reset();
}

void
PlayboxScene::reset()
{
  vvr::Scene::reset();

  randomColourIndex = 0;
  keepCentered = true;
  worldCenter = {
    {0.f, 3.0f},
    150.0f
  };
  worldCenter.update(keepCentered);
  anim.reset();
  canvas.clear();

  setupPhysics();

  resize();
  anim.toggle();
}

void
PlayboxScene::setCenterTarget()
{
  const auto &pos = ballBody->GetPosition();
  auto newx = pos.x;
  auto newy = step(pos.y, worldSize.y) + worldSize.y * 0.48;
  const math::float2 newCenter(newx, newy);
  worldCenter.setTarget(newCenter);
}

void
PlayboxScene::setupPhysics()
{
  simulationTime = {0};

  if (world) {
    delete world;
    world = nullptr;
    groundBody = nullptr;
    ballBody = nullptr;
    boxBodies.clear();
  }

  {
    // Create world:
    b2Vec2 gravity(0.0f, -10.0f);
    world = new b2World(gravity);
  }

  {
    // Create ground:
    b2BodyDef groundBodyDef;
    b2PolygonShape groundBox;
    b2FixtureDef fixtureDef;
    groundBodyDef.position.Set(0.0f, -gbH);
    groundBox.SetAsBox(gbW, gbH);
    fixtureDef.shape = &groundBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 2.0f;
    world->CreateBody(&groundBodyDef)->CreateFixture(&fixtureDef);
  }

  {
    // Create track:
    b2BodyDef trackBodyDef;
    b2FixtureDef fixtureDef;
    b2ChainShape chain;
    std::vector<b2Vec2> vs;
    trackBodyDef.type = b2_staticBody;
    trackBodyDef.position.Set(0.0f, 0.0f);
    // Reverse iterator due to need for CCW order in Box2D:
    for (auto it = roadPts.rbegin(); it != roadPts.rend(); ++it) {
      vs.push_back({(*it)->x, (*it)->y});
    }
    fixtureDef.shape = &chain;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 2.0f;
    chain.CreateLoop(vs.data(), roadPts.size());
    groundBody = world->CreateBody(&trackBodyDef);
    groundBody->CreateFixture(&fixtureDef);
  }

  {
    // Create ball:
    b2BodyDef bodyDef;
    b2CircleShape dynamicCircle;
    b2FixtureDef fixtureDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 8.0f);
    bodyDef.angularVelocity = -8.0f * M_PI;
    dynamicCircle.m_radius = 2.0f;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 0.040f;
    fixtureDef.friction = 1.0f;
    ballBody = world->CreateBody(&bodyDef);
    ballBody->CreateFixture(&fixtureDef);
    ballBody->SetAngularDamping(2);
  }

  for (int i = 0; i < 10; ++i) {
    // Create boxes:
    b2BodyDef bodyDef;
    b2PolygonShape dynamicBox;
    b2FixtureDef fixtureDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(7.0f * i, 10.0f);
    bodyDef.angularVelocity = 0.25f * M_PI;
    dynamicBox.SetAsBox(3.0f, 1.50f);
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 0.2f;
    fixtureDef.friction = 1.0f;
    boxBodies.push_back(world->CreateBody(&bodyDef));
    boxBodies.back()->CreateFixture(&fixtureDef);
  }
}

bool
PlayboxScene::idle()
{
#if BENCHMARKS_ON
  ScopedCounter counter{"idle"};
#endif

  double dt = anim.update();

  if (keepCentered) {
    setCenterTarget();
  } else {
    worldCenter.snapToTarget();
  }

  return !anim.paused();
}

void
PlayboxScene::simulatePhysics()
{
  float timeStep = 1.0f / 60.0f;
  int32 velocityIterations = 6;
  int32 positionIterations = 2;
  world->Step(timeStep, velocityIterations, positionIterations);
  simulationTime += timeStep;
}

struct GameControls
{
  bool brake = false;
  bool gas = false;
  bool downforce = false;
  bool upforce = false;
};

void
PlayboxScene::draw()
{
#if BENCHMARKS_ON
  ScopedCounter counter{"drawing"};
#endif

  static GameControls prevCon;

  const GameControls con{
    .brake = isArrowDown(vvr::LEFT),
    .gas = isArrowDown(vvr::RIGHT),
    .downforce = isArrowDown(vvr::DOWN),
    .upforce = isArrowDown(vvr::UP),
  };

  if (!anim.paused()) {
    constexpr auto torque = 100.0f;
    constexpr auto force = 15.0f;
    constexpr auto impulse = 15.0f;

    if (con.brake) {
      ballBody->ApplyTorque(torque, true);
      ballBody->ApplyForceToCenter({-force, 0}, true);
    } else if (con.gas) {
      ballBody->ApplyTorque(-torque, true);
      ballBody->ApplyForceToCenter({+force, 0}, true);
    }

    if (con.downforce && prevCon.downforce == false) {
      ballBody->ApplyLinearImpulseToCenter({0, -impulse * 2}, true);
    } else if (con.upforce && prevCon.upforce == false) {
      ballBody->ApplyLinearImpulseToCenter({0, +impulse}, true);
    }

    simulatePhysics();
  }

  enterPixelMode();

  {
    vvr::BackupAndRestore<float> tmp[] = {
      {vvr::Shape::PointSize, 10.0f},
      {vvr::Shape::LineWidth,  3.0f}
    };
    const float sec = simulationTime;
    const float vpw = this->getViewportWidth();
    const float vph = this->getViewportHeight();
    const float r = 50; // clock radius
    const float p = 15; // padding
    const C2DPoint c(-vpw / 2. + r + p, vph / 2. - r - p);
    float a = sec * M_PI;

    // Draw a clock
    vvr::Circle2D ring({c, r}, vvr::white);
    vvr::Point2D h1(c.x + r * sin(a), c.y + r * cos(a), vvr::Cornsilk);
    vvr::Point2D h2(c.x + r * sin(a / 60), c.y + r * cos(a / 60), vvr::white);
    ring.draw();
    h1.draw();
    h2.draw();

    // Draw gas and brake indicators
    ring.SetRadius(r / 2);
    ring.Move({(r * 2 + p), r / 2});
    ring.filled = con.brake;
    ring.colour = vvr::red;
    ring.draw();

    ring.Move({(r + p), 0});
    ring.filled = con.gas;
    ring.colour = vvr::green;
    ring.draw();
  }

  exitPixelMode();

  enter2dMode(worldCenter.get(), worldSize);

  {
    vvr::BackupAndRestore<float> tmp[] = {
      {vvr::Shape::PointSize, 25.0f},
      {vvr::Shape::LineWidth,  4.0f}
    };
    drawPhysics();
    road.draw();
  }

  {
    vvr::BackupAndRestore<float> tmp[] = {
      {vvr::Shape::PointSize, 5.0f},
      {vvr::Shape::LineWidth, 2.0f}
    };
  }

  canvas.draw();

  exitPixelMode();

  prevCon = con;
}

void
PlayboxScene::drawPhysics() const
{
  // Draw ground:
  const auto col = vvr::Aquamarine;
  vvr::LineSeg2D(-gbW, 0, gbW, 0, col).draw();
  vvr::LineSeg2D(gbW, 0, gbW, -gbH * 2, col).draw();
  vvr::LineSeg2D(gbW, -gbH * 2, -gbW, -gbH * 2, col).draw();
  vvr::LineSeg2D(-gbW, -gbH * 2, -gbW, 0, col).draw();

  drawWheel(ballBody);
  for (auto body : boxBodies) {
    drawBox(body);
  }
}

void
PlayboxScene::resize()
{
  Scene::resize();

  if (m_first_resize) {
    std::cout << "Press 'r' to reset." << std::endl;
    std::cout << "Press 'c' to keep centered." << std::endl;
    std::cout << "Press '0'-'9' to change track." << std::endl;
    std::cout << "Press 'space' to pause." << std::endl;
    std::cout << "Press '-' to zoom out." << std::endl;
    std::cout << "Press '+' to zoom in." << std::endl;
    std::cout << "Press 'left'/'right' to move." << std::endl;
  }

  worldSize.y = worldSize.x * getViewportHeight() / getViewportWidth();
}

void
PlayboxScene::arrowEvent(vvr::ArrowDir dir, int modif)
{}

void
PlayboxScene::mouseWheel(int dir, int modif)
{
  if (dir > 0) {
    worldSize.x /= 1.5;
  } else {
    worldSize.x *= 1.5;
  }
  resize();
}

void
PlayboxScene::keyEvent(unsigned char key, bool up, int modif)
{
  Scene::keyEvent(key, up, modif);
  key = tolower(key);

  if (up) {
    return;
  }

  switch (key) {
  case '-':
    worldSize.x *= 1.5;
    resize();
    setCenterTarget();
    worldCenter.snapToTarget();
    break;
  case '+':
  case '=':
    worldSize.x /= 1.5;
    resize();
    setCenterTarget();
    worldCenter.snapToTarget();
    break;
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
    keepCentered = !keepCentered;
    if (keepCentered) {
      worldCenter.update(true);
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
PlayboxScene::mouseHovered(int x, int y, int modif)
{}

void
PlayboxScene::mousePressed(int x, int y, int modif)
{
  const float vpw = getViewportWidth();
  const float vph = getViewportHeight();
  const float xf = (float)x / vpw * worldSize.x + worldCenter.get().x;
  const float yf = (float)y / vph * worldSize.y + worldCenter.get().y;

  if (!keepCentered || anim.paused()) {
    dragAnchor = {xf, yf};
    worldCenterAnchor = worldCenter.get();
  }

  canvas.add(new vvr::Point2D(xf, yf, vvr::orange));
}

void
PlayboxScene::mouseMoved(int x, int y, int modif)
{
  if (dragAnchor.x != hugef && dragAnchor.y != hugef) {
    const float vpw = getViewportWidth();
    const float vph = getViewportHeight();
    const float xf = (float)x / vpw * worldSize.x + worldCenterAnchor.x;
    const float yf = (float)y / vph * worldSize.y + worldCenterAnchor.y;
    const math::float2 t{xf - dragAnchor.x, yf - dragAnchor.y};
    const math::float2 v{worldCenterAnchor - t};
    worldCenter.setValue(v);
  }
}

void
PlayboxScene::mouseReleased(int x, int y, int modif)
{
  dragAnchor = {hugef, hugef};
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(PlayboxScene)
#endif

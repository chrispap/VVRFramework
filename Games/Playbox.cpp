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

  Wheel(float radius, C2DPoint const &pos, float angle, vvr::Colour col);

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

Wheel::Wheel(float radius, C2DPoint const &pos, float angle, vvr::Colour col)
  : angle(angle)
{
  speed = 0;
  circumference = 2 * M_PI * radius;
  tire = vvr::Circle2D({pos, radius}, col);
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
drawWheel(b2Body *bodyBall, vvr::Colour col)
{
  auto bodyPos = bodyBall->GetPosition();
  const auto r = bodyBall->GetFixtureList()->GetShape()->m_radius;
  Wheel w(r, {bodyPos.x, bodyPos.y}, -bodyBall->GetAngle(), col);
  w.draw();
}

void
drawBox(b2Body *bodyBox, vvr::Colour colour = vvr::black)
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
    c.add(*ls.GetAt(i), colour)->draw();
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
  b2Body *ballBody1 = nullptr;
  b2Body *ballBody2 = nullptr;
  std::vector<b2Body *> boxBodies;
};

PlayboxScene::PlayboxScene()
{
  vvr::Shape::SetPointSize(12);
  vvr::Shape::SetLineWidth(5);

  vvr::Scene::m_bg_col = vvr::grey;
  vvr::Scene::m_fullscreen = false;
  vvr::Scene::m_show_log = false;

  worldSize = {30., 0.}; // Define only the width of the world
  gbW = 100.0f;
  gbH = 0.25f;
  roadPts = track_zigzag();
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
    350.0f
  };
  worldCenter.update(keepCentered);
  anim.reset();
  canvas.clear();

  setupPhysics();

  resize();
  // anim.toggle();
}

void
PlayboxScene::setCenterTarget()
{
  const auto &pos = ballBody1->GetPosition();
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
    ballBody1 = nullptr;
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
    fixtureDef.density = 2.0f;
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
    fixtureDef.density = 2.0f;
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
    bodyDef.position.Set(-3.0f, 10.0f);
    bodyDef.angularVelocity = -8.0f * M_PI;
    dynamicCircle.m_radius = 0.6f;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 0.05f;
    fixtureDef.friction = 1.0f;
    ballBody1 = world->CreateBody(&bodyDef);
    ballBody1->CreateFixture(&fixtureDef);
    ballBody1->SetAngularDamping(2);
  }

  {
    // Create ball 2:
    b2BodyDef bodyDef;
    b2CircleShape dynamicCircle;
    b2FixtureDef fixtureDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(-1.0f, 10.0f);
    bodyDef.angularVelocity = 0.0f * M_PI;
    dynamicCircle.m_radius = 0.6f;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 0.05f;
    fixtureDef.friction = 1.0f;
    ballBody2 = world->CreateBody(&bodyDef);
    ballBody2->CreateFixture(&fixtureDef);
    ballBody2->SetAngularDamping(2);
  }

  {
    // Join ball and ball2 with distance joint:
    b2DistanceJointDef jointDef;
    jointDef.Initialize(ballBody1,
      ballBody2,
      ballBody1->GetWorldCenter(),
      ballBody2->GetWorldCenter());
    jointDef.collideConnected = true;
    world->CreateJoint(&jointDef);
  }

  for (int i = 0; i < 100; ++i) {
    // Create boxes:
    b2BodyDef bodyDef;
    b2PolygonShape dynamicBox;
    b2FixtureDef fixtureDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(4.0f * i, 8.0f);
    bodyDef.angularVelocity = 0.25f * M_PI;
    dynamicBox.SetAsBox(1.0f, 0.3f);
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
    constexpr auto torque = 0.4f;
    constexpr auto force = 0.2f;
    constexpr auto impulse = 0.2f;

    if (con.brake) {
      ballBody1->ApplyTorque(torque, true);
      ballBody2->ApplyTorque(torque, true);
      ballBody1->ApplyForceToCenter({-force, 0}, true);
      ballBody2->ApplyForceToCenter({-force, 0}, true);
    } else if (con.gas) {
      ballBody1->ApplyTorque(-torque, true);
      ballBody2->ApplyTorque(-torque, true);
      ballBody1->ApplyForceToCenter({+force, 0}, true);
      ballBody2->ApplyForceToCenter({+force, 0}, true);
    }

    if (con.downforce && prevCon.downforce == false) {
      ballBody1->ApplyLinearImpulseToCenter({0, -impulse * 2}, true);
      ballBody2->ApplyLinearImpulseToCenter({0, -impulse * 2}, true);
    } else if (con.upforce && prevCon.upforce == false) {
      ballBody1->ApplyLinearImpulseToCenter({0, +impulse}, true);
      ballBody2->ApplyLinearImpulseToCenter({0, +impulse}, true);
    }

    prevCon = con;

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

  enter2dMode(worldCenter.get(), worldSize);

  {
    vvr::BackupAndRestore<float> tmp[] = {
      {vvr::Shape::PointSize, 15.0f},
      {vvr::Shape::LineWidth,  5.0f}
    };

    drawPhysics();
    road.draw();
  }

  canvas.draw();

  exitPixelMode();
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

  // Draw balls:
  drawWheel(ballBody1, vvr::red);
  drawWheel(ballBody2, vvr::green);
  vvr::LineSeg2D(ballBody1->GetPosition().x,
    ballBody1->GetPosition().y,
    ballBody2->GetPosition().x,
    ballBody2->GetPosition().y,
    vvr::black)
    .draw();

  // Draw boxes:
  for (auto body : boxBodies) {
    drawBox(body);
  }

  // Draw contact points:
  for (b2Contact *con = world->GetContactList(); con; con = con->GetNext()) {
    auto bodyA = con->GetFixtureA()->GetBody();
    auto bodyB = con->GetFixtureB()->GetBody();

    b2Body *other = nullptr;
    if (bodyA == ballBody1)
      other = bodyB;
    else if (bodyB == ballBody1)
      other = bodyA;

    auto polygon = other ? dynamic_cast<b2PolygonShape *>(
                             other->GetFixtureList()[0].GetShape())
                         : nullptr;

    if (polygon && polygon->m_count == 4) {
      drawBox(bodyA, vvr::red);
    }

    b2WorldManifold worldManifold;
    con->GetWorldManifold(&worldManifold);
    for (int i = 0; i < con->GetManifold()->pointCount; ++i) {
      b2Vec2 pos = worldManifold.points[i];
      vvr::Point2D(pos.x, pos.y, vvr::yellow).draw();
    }
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
  case 'i': {
    const auto &pos = ballBody1->GetPosition();
    std::cout << "Ball pos: [" << pos.x << ", " << pos.y << "]" << std::endl;
  } break;
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

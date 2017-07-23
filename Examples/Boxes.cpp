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

#define FLAG_SHOW_AXES      1
#define FLAG_SHOW_AABB      2
#define FLAG_SHOW_WIRE      4
#define FLAG_SHOW_SOLID     8
#define FLAG_SHOW_NORMALS   16
#define FLAG_SHOW_OBB       32

#define objName "cube.obj"
#define objName "bunny_low.obj"
#define objName "unicorn_low.obj"
#define objName "dragon_low_low.obj"
#define objName "large/unicorn.obj"
#define objName "ironman.obj"

using namespace vvr;
using namespace std;
using namespace math;

/*--- struct Obb ----------------------------------------------------------------------*/

struct Obb : public vvr::IDrawable, private math::OBB
{
    vvr_decl_shared_ptr(Obb)

    void draw() const override;
    void set(const AABB& aabb, const float4x4& transform);

private:
    Obb();
    ~Obb();
    Obb(const Obb&) = delete;
    const size_t num_verts;
    vec *verts;
    vec *norms;
    vvr::Point3D *cornerpts;
    static const vvr::Colour colfill, coledge;
};

Obb::Obb() : num_verts(NumVerticesInTriangulation(1, 1, 1))
{
    verts = new vec[num_verts];
    norms = new vec[num_verts];
    cornerpts = new vvr::Point3D[NumVertices()];
    SetFrom(AABB{ {0, 0, 0 }, { 0, 0, 0 }}, float4x4::identity);
}

Obb::~Obb()
{
    delete[] verts;
    delete[] norms;
    delete[] cornerpts;
}

void Obb::set(const AABB& aabb, const float4x4& transform) 
{
    SetFrom(aabb, transform);
    Triangulate(1, 1, 1, verts, norms, nullptr, true);
    for (size_t i = 0; i < NumVertices(); ++i) {
        cornerpts[i] = math2vvr(CornerPoint(i), coledge);
    }
}

void Obb::draw() const
{
    for (size_t i = 0; i < num_verts; i += 3) {
        math::Triangle t(verts[i + 0], verts[i + 1], verts[i + 2]);
        math2vvr(t, colfill).draw();
    }

    for (size_t i = 0; i < NumEdges(); ++i) 
        math2vvr(Edge(i), coledge).draw();

    auto ptsz_old = vvr::Shape::PointSize;
    vvr::Shape::PointSize = 12;
    for (size_t i = 0; i < NumVertices(); ++i) {
        cornerpts[i].draw();
    }
    vvr::Shape::PointSize = ptsz_old;
}

const vvr::Colour Obb::colfill = vvr::Colour("dd4311");

const vvr::Colour Obb::coledge = vvr::Colour("111111");

/*--- class Scene ---------------------------------------------------------------------*/

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
    void makeObbFromCurrentView();
    int m_disp_flag;
    Obb::Ptr mObb;
};

BoxesScene::BoxesScene()
{
    m_bg_col = Colour("768E77");
    m_perspective_proj = false;
	m_fullscreen = false;
	m_hide_log = false;
    m_disp_flag = 0;
    m_disp_flag |= FLAG_SHOW_SOLID;
    m_disp_flag |= FLAG_SHOW_WIRE;
    m_disp_flag |= FLAG_SHOW_AXES;
    m_disp_flag |= FLAG_SHOW_OBB;
    mObb = Obb::Make();
}

void BoxesScene::reset()
{
    Scene::reset();
    auto pos = getFrustum().Pos();
    pos.y += 10;
    pos.z -= 40;
    setCameraPos(pos);
    makeObbFromCurrentView();
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

    if (m_disp_flag & FLAG_SHOW_AABB) 
    {
        vvr::Box3D b1(0, 0, 0, 10, 10,10, Colour::red);
        vvr::Box3D b2(-10, -10, -10, 5,5,5, Colour::green);
        b1.setTransparency(.5);
        b2.setTransparency(.5);
        b1.draw();
        b2.draw();
    }

    if (m_disp_flag & FLAG_SHOW_OBB) 
    {
        mObb->draw();
    }
}

void BoxesScene::makeObbFromCurrentView()
{
    mObb = Obb::Make();
    vec lo(0, 0, 0);
    vec hi(2, 4, 8);
    AABB aabb(lo, hi);
    float4x4 transform;
    transform = float4x4::RotateFromTo(vec(0, 0, 1), getFrustum().Front());
    mObb->set(aabb, transform);
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
    case 'm': makeObbFromCurrentView(); break;
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

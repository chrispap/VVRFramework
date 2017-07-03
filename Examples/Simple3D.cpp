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

struct CuttingPlane : public math::Plane, vvr::IDrawable
{
    vvr_decl_shared_ptr(CuttingPlane);

    math::vec pos;
    math::vec X, Y, Z;
    vvr::Colour col;
    vvr::Triangle3D t1, t2;
    float halfside;

    void draw() const override
    {
        t1.draw();
        t2.draw();
    }

    CuttingPlane(const math::vec &pos, const math::vec &normal, float halfside, const vvr::Colour &col, bool wire = false)
        : math::Plane(pos, normal.Normalized())
        , pos(pos)
        , halfside(halfside)
        , col(col)
    {
        Z = this->normal;
        Z.PerpendicularBasis(X, Y);
        math::vec v1 = pos + (+X + Y) * halfside;
        math::vec v2 = pos + (+X - Y) * halfside;
        math::vec v3 = pos + (-X - Y) * halfside;
        math::vec v4 = pos + (-X + Y) * halfside;
        t1 = math2vvr(math::Triangle(v1, v2, v3), col);
        t2 = math2vvr(math::Triangle(v3, v4, v1), col);
        t1.setSolidRender(!wire);
        t2.setSolidRender(!wire);
    }

private:
    ~CuttingPlane() {}
};

class Simple3DScene : public vvr::Scene
{
public:
    Simple3DScene();
    const char* getName() const { return "Simple 3D Scene"; }

protected:
    void draw() override;
    void resize() override;
    void reset() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;

private:
    void selectTri(int x, int y);
    void load3DModels();
    void playWithMathGeoLibPolygon();
    void defineCuttingPlane(const math::vec &pos, const vec &normal);

private:
    vvr::Mesh::Ptr m_mesh_1;
    vvr::Mesh::Ptr m_mesh_2;
    vvr::Mesh::Ptr m_mesh_3;
    vvr::Colour m_obj_col;
    vvr::Canvas2D m_canvas;
    std::vector<math::Triangle> m_floor_tris;
    CuttingPlane::Ptr m_plane, m_plane_clipped;
    int m_style_flag;
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

const float L1 = 25;
const float L2 = 5;
const float h1 = -7;
const float h2 = 13;

const vec A(-L1, h1, -L2);
const vec B(+L1, h1, -L2);
const vec C(+L1, h1, +L2);
const vec D(-L1, h1, +L2);
const vec E(-L1, h2, -L2);
const vec F(+L1, h2, -L2);

Simple3DScene::Simple3DScene()
{
    m_bg_col = Colour("768E77");
    m_obj_col = Colour("454545");
    m_perspective_proj = true;
    
    m_style_flag = 0;
    m_style_flag |= FLAG_SHOW_SOLID;
    m_style_flag |= FLAG_SHOW_WIRE;

    m_floor_tris.push_back(math::Triangle(B, A, D));
    m_floor_tris.push_back(math::Triangle(B, D, C));
    m_floor_tris.push_back(math::Triangle(F, E, A));
    m_floor_tris.push_back(math::Triangle(F, A, B));
}

void Simple3DScene::reset()
{
    Scene::reset();
    auto pos = getFrustum().Pos();
    pos.y += 10;
    pos.z -= 40;
    setCameraPos(pos);
    m_canvas.clear();
}

void Simple3DScene::resize()
{
    static bool first_pass = true;

    if (first_pass)
    {
        reset();
        load3DModels();
        defineCuttingPlane({10, 10, 0}, { 1, 1, 1 });
    }

    first_pass = false;
}

void Simple3DScene::defineCuttingPlane(const math::vec &pos, const math::vec &normal)
{
    m_plane = CuttingPlane::Make(pos, normal, 10, Colour::red);

    std::vector<vec> vertices;
    for (auto mesh : { m_mesh_1, m_mesh_2, m_mesh_3 })
    {
        std::set<int> vecs;
        for (auto t : mesh->getTriangles()) {
            if (math::Triangle(t.v1(), t.v2(), t.v3()).Intersects(*m_plane)) {
                vecs.insert(t.vi1);
                vecs.insert(t.vi2);
                vecs.insert(t.vi3);
            }
        }
        for (const int &vi : vecs) {
            vertices.push_back(mesh->getVertices().at(vi));
        }
    }

    float minx, miny, maxx, maxy;
    minx = miny = +std::numeric_limits<float>::max();
    maxx = maxy = -std::numeric_limits<float>::max();
    for (const vec &v : vertices) {
        float x = (v - m_plane->pos).Dot(m_plane->X);
        float y = (v - m_plane->pos).Dot(m_plane->Y);
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y < miny) miny = y;
        if (y > maxy) maxy = y;
    }

    vec newpos =
        ((maxx + minx) / 2) * m_plane->X +
        ((maxy + miny) / 2) * m_plane->Y + m_plane->pos;

    float newside = sqrt(2.0f) * std::max(maxx - minx, maxy - miny);

    m_plane = CuttingPlane::Make(newpos, m_plane->normal, newside / 2, vvr::Colour::red, false);
}

void Simple3DScene::load3DModels()
{
    // Load 3D models.
    const string objFile = getBasePath() + "resources/obj/" + objName;

    m_mesh_1 = Mesh::Make(objFile);
    m_mesh_1->cornerAlign();
    m_mesh_1->setBigSize(getSceneWidth() / 8);
    m_mesh_1->update();

    m_mesh_2 = Mesh::Make(*m_mesh_1);
    m_mesh_3 = Mesh::Make(*m_mesh_1);
    
    m_mesh_2->setBigSize(m_mesh_1->getMaxSize() * 1.5);
    m_mesh_3->setBigSize(m_mesh_1->getMaxSize() / 1.5);

    m_mesh_1->move(vec(m_mesh_1->getAABB().Size().x * -0.5, h1, 0));
    m_mesh_2->move(vec(m_mesh_1->getAABB().Size().x * +0.5, h1, 0));
    m_mesh_3->move(vec(m_mesh_1->getAABB().Size().x * -0.5 + m_mesh_3->getAABB().Size().x * -1, h1, 0));

    m_mesh_1->update();
    m_mesh_2->update();
    m_mesh_3->update();
}

void Simple3DScene::playWithMathGeoLibPolygon()
{

    math::Polyhedron poly;
    vvr::Mesh::Ptr mesh = m_mesh_1;

    for (unsigned i = 0; i < mesh->getVertices().size(); ++i) {
        vec &v = mesh->getVertices()[i];
        poly.v.push_back({ float3(v.x, v.y, v.z) });
    }

    for (unsigned i = 0; i < mesh->getTriangles().size(); ++i) {
        vvr::Triangle &t = mesh->getTriangles()[i];
        Polyhedron::Face f;
        f.v = { t.vi1, t.vi2, t.vi3 };
        f.FlipWindingOrder();
        poly.f.push_back(f);
    }

    //echo(poly.IsConvex());
}

void Simple3DScene::draw()
{
    //! Draw floor
    for (const auto &tri : m_floor_tris)
    {
        auto floor_tri = math2vvr(tri, Colour(23, 35, 56));
        floor_tri.setSolidRender(true);
        floor_tri.draw();
    }

    //! Draw meshes
    for (auto mesh : { m_mesh_1, m_mesh_2, m_mesh_3 })
    {
        if (m_style_flag & FLAG_SHOW_SOLID) mesh->draw(m_obj_col, SOLID);
        if (m_style_flag & FLAG_SHOW_WIRE) mesh->draw(Colour::black, WIRE);
        if (m_style_flag & FLAG_SHOW_NORMALS) mesh->draw(Colour::black, NORMALS);
        if (m_style_flag & FLAG_SHOW_AXES) mesh->draw(Colour::black, AXES);
        if (m_style_flag & FLAG_SHOW_AABB) mesh->draw(Colour::black, BOUND);
    }

    //! Draw selected triangles
    m_canvas.draw();

    //! Draw planes
    if (m_plane) m_plane->draw();
    if (m_plane_clipped) m_plane_clipped->draw();
}

void Simple3DScene::mousePressed(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);

    if (!shift_down) {
        return Scene::mousePressed(x, y, modif);
    }
    else {
        selectTri(x, y);
    }
}

void Simple3DScene::mouseMoved(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);

    if (!shift_down)
    {
        return Scene::mouseMoved(x, y, modif);
    }
    else
    {
        selectTri(x, y);
    }
}

void Simple3DScene::mouseReleased(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);

    if (!shift_down)
    {
        return Scene::mouseReleased(x, y, modif);
    }
    else
    {

    }
}

void Simple3DScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case 'a': m_style_flag ^= FLAG_SHOW_AXES; break;
    case 'w': m_style_flag ^= FLAG_SHOW_WIRE; break;
    case 's': m_style_flag ^= FLAG_SHOW_SOLID; break;
    case 'n': m_style_flag ^= FLAG_SHOW_NORMALS; break;
    case 'b': m_style_flag ^= FLAG_SHOW_AABB; break;
    case 'd': m_mesh_1->getTriangles().erase(m_mesh_1->getTriangles().begin()); break;
    case 'f': m_mesh_2->getTriangles().erase(m_mesh_2->getTriangles().begin()); break;
    }
}

void Simple3DScene::selectTri(int x, int y)
{
    Ray ray = unproject(x, y);


    for (vvr::Mesh::Ptr mesh_ptr : { m_mesh_1, m_mesh_2, m_mesh_3, })
    {
        vector<vvr::Triangle3D*> tris_sel;
        int tri_min_index = -1, tri_counter = 0;
        float tri_min_dist = FLT_MAX;
        math::Triangle tri_sel;

        for (unsigned i = 0; i < mesh_ptr->getTriangles().size(); ++i)
        {
            vvr::Triangle &t = mesh_ptr->getTriangles()[i];

            math::Triangle tri(
                vec(t.v2().x, t.v2().y, t.v2().z),
                vec(t.v1().x, t.v1().y, t.v1().z),
                vec(t.v3().x, t.v3().y, t.v3().z)
                );

            if (tri.Intersects(ray)) {
                tris_sel.push_back(new Triangle3D(math2vvr(tri, Colour::magenta)));
                float d;
                if ((d = tri.DistanceSq(ray.pos)) < tri_min_dist) {
                    tri_min_dist = d;
                    tri_min_index = tri_counter;
                    tri_sel = tri;
                }
                tri_counter++;
            }

        }

        if (tri_min_index != -1) {
            m_canvas.add(tris_sel.at(tri_min_index));
            defineCuttingPlane(tri_sel.CenterPoint(), tri_sel.NormalCW());
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new Simple3DScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}

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

#define FLAG_SHOW_AXES 1
#define FLAG_SHOW_AABB 2
#define FLAG_SHOW_WIRE 4
#define FLAG_SHOW_SOLID 8
#define FLAG_SHOW_NORMALS 16

//#define objName "tavli.obj"
//#define objName "cube.obj"
#define objName "bunny_low.obj"
//#define objName "large/unicorn.obj"
//#define objName "ironman.obj"
//#define objName "unicorn_low.obj"
//#define objName "dragon_low_low.obj"

class Simple3DScene : public vvr::Scene
{
public:
    Simple3DScene();
    const char* getName() const override { return "Simple 3D Scene"; }

protected:
    void draw() override;
    void resize() override;
    void reset() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;

private:
    void load3DModels();
    void pick(int x, int y);
    void defineCuttingPlane(const math::vec &pos, const vec &normal);

private:
    int m_style_flag;
    int m_click_counter;
    vvr::Mesh::Ptr m_mesh_1;
    vvr::Mesh::Ptr m_mesh_2;
    vvr::Mesh::Ptr m_mesh_3;
    vvr::Colour m_obj_col;
    vvr::Canvas m_canvas;
    vvr::Ground* m_floor;
    vvr::Aabb3D* m_box;
    vvr::Quad3D::Ptr m_plane;
    vvr::Quad3D::Ptr m_plane_clipped;
};

Simple3DScene::Simple3DScene()
{
    m_bg_col = vvr::Colour("768E77");
    m_obj_col = vvr::Colour("454545");
    m_perspective_proj = true;
    m_style_flag = 0;
    m_style_flag |= FLAG_SHOW_SOLID;
    m_style_flag |= FLAG_SHOW_WIRE;
    m_floor = new vvr::Ground(25, 5, -5, 5, vvr::Colour(23, 35, 56));
}

void Simple3DScene::reset()
{
    Scene::reset();
    auto pos = getFrustum().Pos();
    pos.y += 10;
    pos.z -= 40;
    setCameraPos(pos);
    m_canvas.clear();
    m_box = new vvr::Aabb3D();
    m_box->colour = vvr::Colour("#459823");
    m_canvas.add(m_box);
    m_click_counter = 0;
}

void Simple3DScene::resize()
{
    static bool first_pass = true;
    if (first_pass) {
        reset();
        load3DModels();
        defineCuttingPlane({10, 10, 0}, { 1, 1, 1 });
    }
    first_pass = false;
}

void Simple3DScene::defineCuttingPlane(const math::vec &pos, const math::vec &normal)
{
    m_plane = vvr::Quad3D::Make(pos, normal, 10, vvr::red);

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

    vec newpos(vec::zero);
    newpos += ((maxx + minx) / 2) * m_plane->X;
    newpos += ((maxy + miny) / 2) * m_plane->Y + m_plane->pos;

    float newside = sqrt(2.0f) * std::max(maxx - minx, maxy - miny);

    m_plane = vvr::Quad3D::Make(newpos, m_plane->normal, newside / 2, vvr::red);
}

void Simple3DScene::load3DModels()
{
    // Load 3D models.
    const std::string objFile = vvr::get_base_path() + "resources/obj/" + objName;

    m_mesh_1 = vvr::Mesh::Make(objFile);
    m_mesh_1->cornerAlign();
    m_mesh_1->setBigSize(getSceneWidth() / 8);
    m_mesh_1->update();

    m_mesh_2 = vvr::Mesh::Make(*m_mesh_1);
    m_mesh_3 = vvr::Mesh::Make(*m_mesh_1);

    m_mesh_2->setBigSize(m_mesh_1->getMaxSize() * 1.5);
    m_mesh_3->setBigSize(m_mesh_1->getMaxSize() / 1.5);

    m_mesh_1->move(vec(m_mesh_1->getAABB().Size().x * -0.5, -5, 0));
    m_mesh_2->move(vec(m_mesh_1->getAABB().Size().x * +0.5, -5, 0));
    m_mesh_3->move(vec(m_mesh_1->getAABB().Size().x * -0.5 + m_mesh_3->getAABB().Size().x * -1, -5, 0));

    m_mesh_1->update();
    m_mesh_2->update();
    m_mesh_3->update();
}

void Simple3DScene::draw()
{
    m_floor->draw();

    //! Draw meshes
    for (auto mesh : { m_mesh_1, m_mesh_2, m_mesh_3 })
    {
        if (m_style_flag & FLAG_SHOW_SOLID) mesh->draw(m_obj_col, vvr::SOLID);
        if (m_style_flag & FLAG_SHOW_WIRE) mesh->draw(vvr::black, vvr::WIRE);
        if (m_style_flag & FLAG_SHOW_NORMALS) mesh->draw(vvr::black, vvr::NORMALS);
        if (m_style_flag & FLAG_SHOW_AXES) mesh->draw(vvr::black, vvr::AXES);
        if (m_style_flag & FLAG_SHOW_AABB) mesh->draw(vvr::black, vvr::BOUND);
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
        pick(x, y);
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
        pick(x, y);
    }
}

void Simple3DScene::mouseReleased(int x, int y, int modif)
{
    ++m_click_counter;
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

void Simple3DScene::pick(int x, int y)
{
    Ray ray = unproject(x, y);
    math::Triangle tri_sel;
    vec intr;

    for (vvr::Mesh::Ptr mesh_ptr : { m_mesh_1, m_mesh_2, m_mesh_3, })
    {
        std::vector<vvr::Triangle3D*> tris_sel;
        int tri_min_index = -1, tri_counter = 0;
        float tri_min_dist = FLT_MAX;

        for (unsigned i = 0; i < mesh_ptr->getTriangles().size(); ++i)
        {
            vvr::Triangle &t = mesh_ptr->getTriangles()[i];

            math::Triangle tri(
                vec(t.v2().x, t.v2().y, t.v2().z),
                vec(t.v1().x, t.v1().y, t.v1().z),
                vec(t.v3().x, t.v3().y, t.v3().z)
                );

            if (tri.Intersects(ray, NULL, &intr)) {
                tris_sel.push_back(new vvr::Triangle3D(tri, vvr::magenta));
                float d;
                if ((d = tri.DistanceSq(ray.pos)) < tri_min_dist) {
                    tri_min_dist = d;
                    tri_min_index = tri_counter;
                    tri_sel = tri;
                }
                tri_counter++;
            }
        }

        if (tri_min_index != -1)
        {
#if 0
            m_canvas.add(tris_sel.at(tri_min_index));
            defineCuttingPlane(tri_sel.CenterPoint(), tri_sel.NormalCW());
#endif
            if (m_click_counter % 2 == 0) {
                m_box->minPoint = intr;
            }
            else {
                m_box->maxPoint = intr;
            }
        }
    }
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(Simple3DScene)
#endif

#include <vvr/settings.h>
#include <vvr/utils.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vector>
#include <string>
#include <MathGeoLib.h>

struct CuttingPlane : public math::Plane, vvr::IRenderable
{
    DECL_SHARED_PTR(CuttingPlane);  

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
        std::cout << "Cutting Plane - Contructed [" << this << "]"<< std::endl;
    }

private: 
    ~CuttingPlane() 
    {
        std::cout << "Cutting Plane - Destructed [" << this << "]" << std::endl;
    }
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

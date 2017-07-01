#include "CGALSurfaceMesher.h"
#include <MathGeoLib.h>

#define NUM_PTS                 1000
#define TIME_INTERVAL_SEC       0.020
#define FLAG_SHOW_HULL          1
#define FLAG_SHOW_HULL_EDGES    2
#define FLAG_SHOW_AABB          4
#define FLAG_SHOW_POINTS        8
#define FLAG_SHOW_AXES          16

using namespace std;
using namespace vvr;

#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Implicit_surface_3.h>

typedef CGAL::Surface_mesh_default_triangulation_3 Tr;
typedef CGAL::Complex_2_in_triangulation_3<Tr> C2t3;
typedef Tr::Geom_traits GT;
typedef GT::Sphere_3 Sphere_3;
typedef GT::Point_3 Point_3;
typedef GT::FT FT;
typedef FT(*Function)(Point_3);
typedef CGAL::Implicit_surface_3<GT, Function> Surface_3;

//! Global parameters tuned with slider

static float SV[6] = {0.2};

//! Triangulation of surface

FT surface_function(Point_3 p) 
{
    const  FT x = p.x();
    const  FT y = p.y();
    const  FT z = p.z();

    const  FT x2 = SQUARE(x);
    const  FT y2 = SQUARE(y);
    const  FT z2 = SQUARE(z);

    const FT a = 1.2;
    const FT b = 1.3;
    const FT c = 2.5;

    const FT a2 = SQUARE(a);
    const FT b2 = SQUARE(b);
    const FT c2 = SQUARE(c);
 
    return x2 - y2 + z2/2 - 1;
}

void Triangulate(vvr::Mesh &mesh)
{
    //! Mesh object structs.
    std::vector<vec> &verts = mesh.getVertices();
    std::vector<vvr::Triangle> &tris = mesh.getTriangles();

    //! Triangulation using CGAL
    Tr tr;
    C2t3 c2t3(tr);
    Surface_3 surface(surface_function, Sphere_3(CGAL::ORIGIN, 25.0));
    CGAL::Surface_mesh_default_criteria_3<Tr> criteria(30, SV[0], SV[1]);
    CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag());

    //! Build a map from vertex pointers to vertex indices.
    std::map<const Tr::Vertex*, std::size_t, std::less<const Tr::Vertex*> > mapping;
    std::size_t vn = 0;

    for (Tr::Vertex_iterator vi = tr.vertices_begin(); 
        vi != tr.vertices_end(); 
        ++vi) 
    {
        if (tr.is_infinite(vi)) continue;

        mapping[&*vi] = vn++;
        verts.push_back(vec(
            CGAL::to_double(vi->point().x()),
            CGAL::to_double(vi->point().y()),
            CGAL::to_double(vi->point().z())
        ));
    }

    for (Tr::Cell_iterator ci = tr.cells_begin();
        ci != tr.cells_end();
        ++ci)
    {
        tris.push_back(vvr::Triangle(&verts,
            mapping[&*ci->vertex(0)],
            mapping[&*ci->vertex(1)],
            mapping[&*ci->vertex(3)]
        ));

        tris.push_back(vvr::Triangle(&verts,
            mapping[&*ci->vertex(1)],
            mapping[&*ci->vertex(2)],
            mapping[&*ci->vertex(3)]
        ));
    }

    mesh.update(true);

    echo(tr.number_of_vertices());
    echo(tr.number_of_facets());
    echo(tr.number_of_cells());
    echo(verts.size());
    echo(tris.size());
}

//! Scene

CGALSurfaceMesher::CGALSurfaceMesher()
{
    vvr::Shape::DEF_POINT_SIZE = 5;
    m_create_menus = true;
    m_fullscreen = false;
    m_perspective_proj = true;
    m_bg_col = vvr::Colour::white;
    m_hide_log = false;
    m_hide_sliders = false;
    m_flag = 0;
    m_flag |= VVR_FLAG(SHOW_SOLID);
    m_flag |= VVR_FLAG(SHOW_WIRE);
}

void CGALSurfaceMesher::resize()
{
    static bool first_pass = true;
    if (first_pass)
    {
        first_pass = false;
        Triangulate(m_mesh);
        m_mesh.setBigSize(getSceneWidth() * 0.2);
    }
}

void CGALSurfaceMesher::draw()
{
    if (VVR_FLAG_ON(m_flag, SHOW_SOLID)) m_mesh.draw(vvr::Colour::green, vvr::SOLID);
    
    if (VVR_FLAG_ON(m_flag, SHOW_WIRE)) m_mesh.draw(vvr::Colour::black, vvr::WIRE);
    
    if (VVR_FLAG_ON(m_flag, SHOW_NORMALS)) m_mesh.draw(vvr::Colour::red, vvr::NORMALS);
}

void CGALSurfaceMesher::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
        VVR_FLAG_CASE_TOGGLE(m_flag, 's', SHOW_SOLID);
        VVR_FLAG_CASE_TOGGLE(m_flag, 'w', SHOW_WIRE);
        VVR_FLAG_CASE_TOGGLE(m_flag, 'n', SHOW_NORMALS);
    }

}

void CGALSurfaceMesher::sliderChanged(int slider_id, float val)
{
    SV[slider_id] = val;

    if (slider_id >= 0 && slider_id <= 2) 
    {
        m_mesh = vvr::Mesh();
        Triangulate(m_mesh);
    }
    else if (slider_id == 3) 
    {
        m_mesh.setBigSize(getSceneWidth() * SV[2] * 2);
    }
}

//! Main

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new CGALSurfaceMesher);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}

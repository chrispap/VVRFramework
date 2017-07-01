#include "CGALTest.h"
#include <fstream>
#include <vvr/utils.h>
#include <MathGeoLib/MathGeoLib.h>

using namespace std;
using namespace vvr;

#define NUM_PTS                 1000
#define TIME_INTERVAL_SEC       0.020
#define FLAG_SHOW_HULL          1
#define FLAG_SHOW_HULL_EDGES    2
#define FLAG_SHOW_AABB          4
#define FLAG_SHOW_POINTS        8
#define FLAG_SHOW_AXES          16

CGALTestScene::CGALTestScene()
{
    vvr::Shape::DEF_POINT_SIZE = 5;
    m_create_menus = true;
    m_fullscreen = false;
    m_perspective_proj = true;
    m_bg_col = vvr::Colour::black;
    m_style_flag = 0;
    m_style_flag |= FLAG_SHOW_POINTS;
    m_style_flag |= FLAG_SHOW_HULL;
    m_style_flag |= FLAG_SHOW_HULL_EDGES;
    m_hide_log = false;
    m_pause = false;
}

void CGALTestScene::resize()
{
    static bool first_pass = true;
    if (first_pass)
    {
        first_pass = false;
        reset();
    }
}

void CGALTestScene::reset()
{
    const float mw = getSceneWidth() * 0.3;
    const float mh = getSceneHeight() * 0.3;
    const float mz = std::min(mw, mh);

    // Create random 3D points.
    m_pts.clear();
    for (int i = 0; i < NUM_PTS; ++i) {
        m_pts.push_back(vvr::Point3D(
            mw * (float)uf(gen) - mw / 2,
            mh * (float)uf(gen) - mh / 2,
            mz * (float)uf(gen) - mz / 2,
            vvr::Colour("660011"))
        );
    }

    computeHull();
}

bool CGALTestScene::idle()
{
    //! Read current anim time
    if (m_pause) return false;
    const float t = vvr::getSeconds();
    const float dt = t - m_time_last;
    if (dt < TIME_INTERVAL_SEC) return true;
    m_time_last = t;
    
    //! Move points
    const float mw = getSceneWidth() * 0.3;
    const float mh = getSceneHeight() * 0.3;
    const float mz = std::min(mw, mh);
    const float dv = mz / 100;

    /*std::for_each(m_pts.begin(), m_pts.end(),
        [&](vvr::Point3D &p) {*/
    
    //#pragma omp parallel for
    for (int i = 0; i < m_pts.size(); ++i)
    {
        vvr::Point3D &p(m_pts[i]);

        const float vel[3] = {
            //dv * ((float)uf(gen) - 0.5),
            dv * ((float)rand() / RAND_MAX - 0.5),
            dv * ((float)rand() / RAND_MAX - 0.5),
            dv * ((float)rand() / RAND_MAX - 0.5),
            
        };

        p.x += vel[0];
        p.y += vel[1];
        p.z += vel[2];
    };

    computeHull();

    //! Rotate camera
    // m_world_rot.y += vvr::normalizeAngle(dt * 5);

    // Continue 
    return !m_pause;
}

void CGALTestScene::draw()
{
    const float mw = getSceneWidth() * 0.3 / 2;
    const float mh = getSceneHeight() * 0.3 / 2;
    const float mz = std::min(mw, mh);
    
    //! Draw Axes in world center
    if (m_style_flag & FLAG_SHOW_AXES) {
        drawAxes();
    }

    //! Draw points
    if (m_style_flag & FLAG_SHOW_POINTS)
    {
        std::for_each(m_pts.begin(), m_pts.end(),
            [](vvr::Point3D &p) {
            p.draw();
        });
    }

    //! Draw Hull Polygon
    if (m_style_flag & FLAG_SHOW_HULL)
    {
        std::for_each(m_ch_poly.facets_begin(), m_ch_poly.facets_end(), [](Polyhedron_3::Facet &f) {
            Polyhedron_3::Halfedge_handle h = f.halfedge();
            K::Point_3 &p1 = h->prev()->vertex()->point();
            K::Point_3 &p2 = h->vertex()->point();
            K::Point_3 &p3 = h->next()->vertex()->point();
            vvr::Triangle3D(
                p1.x(), p1.y(), p1.z(),
                p3.x(), p3.y(), p3.z(),
                p2.x(), p2.y(), p2.z(),
                vvr::Colour("224422")).draw();
        });
    }

    //! Draw Hull edges
    if (m_style_flag & FLAG_SHOW_HULL_EDGES) {
        std::for_each(m_ch_poly.edges_begin(), m_ch_poly.edges_end(), [](Polyhedron_3::Halfedge &e) {
            K::Point_3 &p1 = e.vertex()->point();
            K::Point_3 &p2 = e.opposite()->vertex()->point();
            vvr::LineSeg3D(
                p1.x(), p1.y(), p1.z(),
                p2.x(), p2.y(), p2.z(),
                vvr::Colour::yellowGreen).draw();
        });
    }

    //! Draw Bounding Box
    if (m_style_flag & FLAG_SHOW_AABB)
    {
        vvr::Box3D aabb(-mw, -mh, -mz, mw, mh, mz);
        aabb.setColour(vvr::Colour::white);
        aabb.setSolidRender(false);
        aabb.draw();
    }

    //! Display FPS
    static float LAST_RENDER_SEC;
    static float LAST_FPS_PRINT;
    const float sec = vvr::getSeconds();
    if (sec - LAST_FPS_PRINT > 1) {
        const float fps_dt = sec - LAST_RENDER_SEC;
        cout << "FPS: " << (int)(1.0 / fps_dt) << endl;
        LAST_FPS_PRINT = sec;
    }
    LAST_RENDER_SEC = sec;
}

void CGALTestScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    key = tolower(key);

    switch (key)
    {
    case 'a': m_style_flag ^= FLAG_SHOW_AXES; break;
    case 'h': m_style_flag ^= FLAG_SHOW_HULL; break;
    case 'e': m_style_flag ^= FLAG_SHOW_HULL_EDGES; break;
    case 'b': m_style_flag ^= FLAG_SHOW_AABB; break;
    case 'p': m_style_flag ^= FLAG_SHOW_POINTS; break;
    case ' ': m_pause = !m_pause; break;
    }

}

void CGALTestScene::computeHull()
{
    //! Compute Convex Hull
    
    m_ch_poly.clear();
    std::vector<Point_3> points;
    for (auto &p : m_pts) points.push_back(Point_3(p.x, p.y, p.z));
    CGAL::convex_hull_3(points.begin(), points.end(), m_ch_poly);

    //! Store hull as .obj.
    
    std::string filename("hull.obj");
    std::ofstream os(filename);
    CGAL::print_polyhedron_wavefront(os, m_ch_poly);
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new CGALTestScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}

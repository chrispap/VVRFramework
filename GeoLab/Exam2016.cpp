#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/animation.h>
#include <algorithm>
#include <vector>
#include <MathGeoLib.h>

void Task_Calliper(const C2DPointSet &pts, C2DLine &line1, C2DLine &line2);

void Task_Edges(const vvr::Mesh &mesh, std::vector<vvr::LineSeg3D> &segments);

class Exam2016Scene : public vvr::Scene
{
public:
    Exam2016Scene();

    const char* getName() const override {
        return "GeoLab 2016 Exam";
    }

protected:
    void draw() override;
    void resize() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    std::vector<C2DTriangle> m_tris;
    C2DPointSet m_pts;
    C2DLine m_line1;
    C2DLine m_line2;
    vvr::Animation m_anim;
    vvr::Mesh::Ptr m_mesh;
    std::vector<vvr::LineSeg3D> m_edges;
    int m_style_flag;
};
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>

#define FLAG_SHOW_AXES 1
#define FLAG_SHOW_AABB 2
#define FLAG_SHOW_WIRE 4
#define FLAG_SHOW_SOLID 8
#define FLAG_SHOW_NORMALS 16
#define OBJ_FILENAME "bunny_low.obj"

using namespace std;
using namespace vvr;
using namespace math;

struct Edge
{
    int v1, v2; // indices of vertices
    int t1, t2; // indices of triangles

    Edge(int v1, int v2)
        : v1(min(v1, v2))
        , v2(max(v1, v2))
        , t1(-1)
        , t2(-1)
    {

    };

    bool operator< (const struct Edge &o) const {
        return v1 < o.v1 ? true : (v1 == o.v1 ? v2 < o.v2 : false);
    };

    bool operator== (const struct Edge &o) const {
        return v1 == o.v1 && v2 == o.v2;
    };

};

/**
* Return the index of an edge in a vector of edges IF it exists,
* or return -1 if it doesn't.
*/
int findEdge(const std::vector<Edge> &edges, const Edge &edge)
{
    std::vector<Edge>::const_iterator eit;
    eit = std::find(edges.begin(), edges.end(), edge);
    if (eit == edges.end()) return -1;
    else return eit - edges.begin();
}

void Task_Calliper(const C2DPointSet &pts, C2DLine &line1, C2DLine &line2)
{
    /////////////////////////////////////////////////////////////////////////////////////
    // DESCRIPTION:                                                                    //
    //      Breite tis paralliles eutheies elaxistis apostasis pou perikleioun         //
    //      ola ta simeia.                                                             //
    //                                                                                 //
    // DEDOMENA:                                                                       //
    //      C2DPointSet pts // ta simeia                                               //
    //                                                                                 //
    // RESULT:                                                                         //
    //      Tis grammes apothikeystese tis stis metavlites line1, line2                //
    //                                                                                 //
    // HINTS:                                                                          //
    //      double C2DLine::DistanceAsRay(const C2DPoint& TestPoint) const;            //
    //                                                                                 //
    /////////////////////////////////////////////////////////////////////////////////////

    float min_dist = FLT_MAX;

    for (int i = 0; i < pts.size(); i++)
    {
        for (int j = 0; j < pts.size(); j++)
        {
            if (j == i) continue;

            C2DLine l(pts[i], pts[j]);
            float max_dist = 0;
            int kk = 0;
            int cl = 0;
            int cr = 0;

            for (int k = 0; k < pts.size(); k++)
            {
                if (k == i) continue;
                if (k == j) continue;

                if (l.IsOnRight(pts[k])) cr++; else cl++;
                if (cr && cl) break;

                if (l.DistanceAsRay(pts[k]) > max_dist) {
                    max_dist = l.Distance(pts[k]);
                    kk = k;
                }
            }

            if (cr && cl) continue;

            if (max_dist < min_dist) {
                min_dist = max_dist;
                line1 = l;
                line2 = l;
                line2.Move(pts[kk] - pts[i]);
            }
        }
    }

    line1.GrowFromCentre(4);
    line2.GrowFromCentre(4);
}

void Task_Edges(const vvr::Mesh &mesh, std::vector<vvr::LineSeg3D> &segments)
{
    /////////////////////////////////////////////////////////////////////////////////////
    // DESCRIPTION:                                                                    //
    //    - Breite tis akmes (edges) tou trigwnikou mesh pou sas dinetai.              //
    //      Xrwmatiste kathe akmi analoga me tin gwnia pou sximatizoun metaksy tous    //
    //      ta 2 proskeimena trigwna tis.                                              //
    //                                                                                 //
    //    - Dwste idiaiteri prosoxi se akmes pou vriskontai sto orio tou antikeimenou  //
    //      me apotelesma na exoun MONO ENA proskeimeno trignwo.                       //
    //                                                                                 //
    //    - Ypothetontas oti kamia gwnia den kseperna ta pi/4 xrwmatiste ws eksis:     //
    //        0 rad    : Full Blue                                                     //
    //        pi/4 rad : Full Red                                                      //
    //        Endiamesa: Interpolation                                                 //
    //                                                                                 //
    //                                                                                 //
    // DEDOMENA:                                                                       //
    //      verts // Koryfes mesh                                                      //
    //      tris  // Trigwna mesh                                                      //
    //                                                                                 //
    // RESULT:                                                                         //
    //      Apothikeyste tis akmes pou tha vreite san antikeimena LineSeg3D sto        //
    //      vector segments. (Deite HINT).                                             //
    //                                                                                 //
    // HINTS:                                                                          //
    //    - Xrisimopoieiste to struct Edge gia na anaparastisete mia akmni             //
    //      san zeugos apo indices koryfwn.                                            //
    //                                                                                 //
    //    - To vector edges mporei na xrisimopoiithei gia tis sygkentrwsi twn akmwn    //
    //                                                                                 //
    //    - Gia na vreite to index enos segment mesa se ena vector apo edges:          //
    //          int ei = findEdge(edges, edge);                                        //
    //                                                                                 //
    //    - Gia na prosthesete mia akmi sto teoiko vector apo segments:                //
    //          segments.push_back(math2vvr(math::LineSegment(v1, v2), col));          //
    //                                                                                 //
    /////////////////////////////////////////////////////////////////////////////////////

    const vector<vec>& verts = mesh.getVertices();
    const vector<vvr::Triangle>& tris = mesh.getTriangles();

    //! Construct edge vec
    vector<Edge> edges;
    for (int ti = 0; ti < tris.size(); ti++)
    {
        for (int j = 0; j < 3; j++) {
            Edge edge(tris[ti].v[j], tris[ti].v[(j + 1) % 3]);
            Edge *eptr;
            int ei = findEdge(edges, edge);
            if (ei < 0) {
                edges.push_back(edge);
                eptr = &edges.back();
            }
            else eptr = &edges[ei];
            if (eptr->t1 == -1) eptr->t1 = ti;
            else eptr->t2 = ti;
        }
    }

    //! Colour each edge
    for (int i = 0; i < edges.size(); i++)
    {
        const Edge &e = edges[i];
        Colour col = Colour::black;

        if (e.t1 < 0 || e.t2 < 0)
        {
            col = Colour::green;
        }
        else
        {
            math::Triangle t1, t2;
            t1 = math::Triangle(tris[e.t1].v1(), tris[e.t1].v2(), tris[e.t1].v3());
            t2 = math::Triangle(tris[e.t2].v1(), tris[e.t2].v2(), tris[e.t2].v3());
            float a = ::abs(t1.NormalCW().AngleBetweenNorm(t2.NormalCW()) / (math::pi / 4));
            if (a > 1) a = 1;
            col.r = a * 255;
            col.b = (1.0 - a) * 255;
        }

        segments.push_back(math2vvr(math::LineSegment(verts[e.v1], verts[e.v2]), col));
    }

    /* Alternative */

    /*set<Edge> edge_set;
    vector<Edge> edge_vec;

    for (int ti = 0; ti < tris.size(); ti++)
    {
    edge_set.insert(Edge(tris[ti].vi1, tris[ti].vi2));
    edge_set.insert(Edge(tris[ti].vi2, tris[ti].vi3));
    edge_set.insert(Edge(tris[ti].vi3, tris[ti].vi1));
    }

    edge_vec = vector<Edge>(edge_set.begin(), edge_set.end());

    for (int ti = 0; ti < tris.size(); ti++)
    {
    for (int ei = 0; ei < 3; ei++) {
    Edge &e = *std::find(edge_vec.begin(), edge_vec.end(),
    Edge(tris[ti].v[ei], tris[ti].v[(ei + 1) % 3]));
    if (e.t1 == -1) e.t1 = ti; else e.t2 = ti;
    }
    }

    for (int i = 0; i < edge_vec.size(); i++)
    {
    const Edge &e = edge_vec[i];
    Colour col = Colour::black;

    if (e.t1 < 0 || e.t2 < 0)
    {
    col = Colour::green;
    }
    else
    {
    math::Triangle t1, t2;
    t1 = math::Triangle(tris[e.t1].v1(), tris[e.t1].v2(), tris[e.t1].v3());
    t2 = math::Triangle(tris[e.t2].v1(), tris[e.t2].v2(), tris[e.t2].v3());
    float a = ::abs(t1.NormalCW().AngleBetweenNorm(t2.NormalCW()) / (math::pi / 4));
    if (a > 1) a = 1;
    col.r = a * 255;
    col.b = (1.0 - a) * 255;
    }

    segments.push_back(math2vvr(math::LineSegment(verts[e.v1], verts[e.v2]), col));
    }*/

}

void RandomPts(C2DPointSet &ptset, int ptnum, int W0, int W1, int H0, int H1)
{
    ptset.DeleteAll();

    CRandomNumber rnw, rnh;
    rnw.SetMin(W0);
    rnw.SetMax(W1);
    rnh.SetMin(H0);
    rnh.SetMax(H1);

    for (int i = 0; i < ptnum; i++) {
        ptset.AddCopy(rnw.GetInt(), rnh.GetInt());
    }

    ptset.SortLeftToRight();
    for (int i = 0; i < ptset.size() - 1; i++) {
        if (ptset.GetAt(i)->Distance(*ptset.GetAt(i + 1)) == 0.0) {
            ptset.DeleteAt(i--);
        }
    }
}

Exam2016Scene::Exam2016Scene()
{
    //! Setup: Scene 
    m_perspective_proj = true;
    m_style_flag = FLAG_SHOW_SOLID;
    m_bg_col = Colour(0x76, 0x8E, 0x77);

    //! Setup Task: Triangle
    C2DPoint p0(110, 50);
    C2DPoint p1(-160, 100);
    C2DPoint p2(195, 195);

    //! Setup Task: Lines
    RandomPts(m_pts, 25, 150, 350, -200, 200);
    m_line1.Set(m_pts[0], m_pts[1]);
    m_line2.Set(m_pts[1], m_pts[2]);

    //! Setup Task: 3D
    const string objDir = getBasePath() + "resources/obj/";
    const string objFile = objDir + OBJ_FILENAME;
    m_mesh = Mesh::Make(objFile);

    //! Execute Tasks
    Task_Calliper(m_pts, m_line1, m_line2);
}

void Exam2016Scene::resize()
{
    static bool first_pass = true;

    if (first_pass)
    {
        m_mesh->setBigSize(getSceneWidth() / 3);
        m_mesh->centerAlign();
        float3x4 t = m_mesh->getTransform();
        t.SetTranslatePart(vec(-getSceneWidth() / 4, 0, 0));
        m_mesh->setTransform(t);
        m_mesh->update();
        Task_Edges(*m_mesh, m_edges);
    }

    first_pass = false;
}

void Exam2016Scene::draw()
{
    //! Draw mesh
    if (m_style_flag & FLAG_SHOW_SOLID) m_mesh->draw(Colour::grey, SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE) m_mesh->draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS) m_mesh->draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES) m_mesh->draw(Colour::black, AXES);
    if (m_style_flag & FLAG_SHOW_AABB) m_mesh->draw(Colour::black, BOUND);

    float w = vvr::Shape::LineWidth;
    vvr::Shape::LineWidth = 3;
    for (int i = 0; i < m_edges.size(); i++) {
        LineSeg3D seg = m_edges.at(i);
        vec v1(seg.x1, seg.y1, seg.z1);
        vec v2(seg.x2, seg.y2, seg.z2);
        v1 = m_mesh->getTransform().TransformPos(v1);
        v2 = m_mesh->getTransform().TransformPos(v2);
        seg.x1 = v1.x;
        seg.y1 = v1.y;
        seg.z1 = v1.z;
        seg.x2 = v2.x;
        seg.y2 = v2.y;
        seg.z2 = v2.z;
        seg.draw();
    }
    vvr::Shape::LineWidth = w;

    enterPixelMode();

    //! Split screen
    Line2D(0, 0, 0, 1).draw();
    Canvas tmp_canvas;

    //! Draw parallel lines and points
    tmp_canvas.add(m_line1, Colour::yellow);
    tmp_canvas.add(m_line2, Colour::yellowGreen);
    tmp_canvas.draw();
    vvr::draw(m_pts);

    returnFromPixelMode();
}

bool Exam2016Scene::idle()
{
    if (m_anim.paused()) return false;
    const float deg_per_sec = 30;
    m_anim.update();
    float3x4 t = m_mesh->getTransform();
    t.SetRotatePartY(math::DegToRad(vvr::normalizeAngle(m_anim.t * deg_per_sec)));
    m_mesh->setTransform(t);
    return true;
}

void Exam2016Scene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
}

void Exam2016Scene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
}

void Exam2016Scene::keyEvent(unsigned char key, bool up, int modif)
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
    case ' ': if (m_anim.paused()) m_anim.update(true); else m_anim.pause();
    }

}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new Exam2016Scene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

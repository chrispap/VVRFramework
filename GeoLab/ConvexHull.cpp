#ifndef SCENE_CONVEX_HULL_H
#define SCENE_CONVEX_HULL_H

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/utils.h>
#include <GeoLib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

class ConvexHullScene : public vvr::Scene
{
public:

    ConvexHullScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif);

private:
    void ConvexHull_Slow();
    void ConvexHull_Fast();

private:
    C2DPolygon m_convex_hull_polygon;
    C2DPointSet m_point_cloud;
    vvr::Canvas m_canvas, m_canvas_algo_steps;
    vvr::Point2D* m_mouse_pos;
};

#endif // SCENE_CONVEX_HULL_H

#define MAX_NUM_PTS 50
#define ConvexHull ConvexHull_Fast 
#define ConvexHull ConvexHull_Slow 

using namespace std;

ConvexHullScene::ConvexHullScene()
{
    m_bg_col = vvr::Colour(0x44, 0x44, 0x44);
    m_hide_log = false;
    m_hide_sliders = true;
    reset();
}

void ConvexHullScene::reset()
{
    Scene::reset();
    m_mouse_pos = new vvr::Point2D();
    m_canvas_algo_steps.clear();
    m_canvas.clear();
    m_canvas.add(m_mouse_pos);
    m_convex_hull_polygon.Clear();
    const int BW = 100;
    const int BH = 100;
    C2DRect bound(-BW, BH, BW, -BH);
    C2DPolygon rand_poly;
    rand_poly.CreateRandom(bound, MAX_NUM_PTS, MAX_NUM_PTS);
    m_point_cloud.DeleteAll();
    rand_poly.GetPointsCopy(m_point_cloud);

    cout << "Computing Convex Hull..." << endl;
    float duration = vvr::getSeconds();
    ConvexHull();
    duration = vvr::getSeconds() - duration;
    cout << "Duration: " << duration << " sec" << endl;
}

void ConvexHullScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    m_mouse_pos->colour = vvr::magenta;
    m_mouse_pos->x = x; m_mouse_pos->y = y;

    C2DPoint p(x, y);
    bool inside;

    // If we wanted to use GeoLib instead of doing it customly.
    // inside = m_convex_hull_polygon.Contains(p);

    inside = true;
    for (int i = 0; i < m_convex_hull_polygon.GetPointsCount(); i++) {
        C2DLine line(*m_convex_hull_polygon.GetLine(i));
        if (!line.IsOnRight(p)) {
            inside = false;
            break;
        }
    }

    m_canvas.clear();
    m_canvas.add(new vvr::Point2D(x, y, inside ? vvr::yellow : vvr::red));

    if (!inside)
    {
        vvr::LineSeg2D *l1 = new vvr::LineSeg2D(x, y, 0, 0, vvr::blue);
        vvr::LineSeg2D *l2 = new vvr::LineSeg2D(*l1);
        const C2DLine *poly_seg;
        int i = 0;

        // Find the first vissible segment
        while (!m_convex_hull_polygon.GetLine(i++)->IsOnRight(p));
        while (m_convex_hull_polygon.GetLine(i++)->IsOnRight(p));

        // Create the first line
        poly_seg = m_convex_hull_polygon.GetLine(--i);
        l1->x2 = poly_seg->GetPointFrom().x;
        l1->y2 = poly_seg->GetPointFrom().y;

        // vvr::Colour the vissisble segments
        do
        {
            m_canvas.add(new vvr::LineSeg2D(poly_seg->GetPointFrom().x,
                poly_seg->GetPointFrom().y,
                poly_seg->GetPointTo().x,
                poly_seg->GetPointTo().y, vvr::blue));
            poly_seg = m_convex_hull_polygon.GetLine(++i);
        } while (!poly_seg->IsOnRight(p));

        // Create the second line
        poly_seg = m_convex_hull_polygon.GetLine(--i);
        l2->x2 = poly_seg->GetPointTo().x;
        l2->y2 = poly_seg->GetPointTo().y;

        m_canvas.add(l1);
        m_canvas.add(l2);
    }

}

void ConvexHullScene::mouseReleased(int x, int y, int modif)
{
    m_mouse_pos->colour = vvr::white;
    m_mouse_pos->x = x; m_mouse_pos->y = y;
}

void ConvexHullScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    mousePressed(x, y, modif);
}

void ConvexHullScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::LEFT) {
        if (m_canvas_algo_steps.isAtStart()) m_canvas_algo_steps.ff();
        else m_canvas_algo_steps.prev();
    }
    else if (dir == vvr::RIGHT) {
        if (m_canvas_algo_steps.isAtEnd()) m_canvas_algo_steps.rew();
        else m_canvas_algo_steps.next();
    }
}

void ConvexHullScene::draw()
{
    enterPixelMode();

    // Do any drawing here.
    vvr::draw(m_convex_hull_polygon, vvr::green);
    vvr::draw(m_point_cloud, vvr::black);
    m_canvas_algo_steps.draw();
    m_canvas.draw();

    exitPixelMode();
}

void ConvexHullScene::ConvexHull_Slow()
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK: 
    //!
    //!  - Breite to kyrto polygwno twn simeinwn `m_point_cloud`.
    //!
    //! HINTS:
    //!
    //!  - An `p` einai C2DPoint* tote gia na parete to `N` simeio tou point cloud 
    //!    grafete to eksis:
    //!      p = m_point_cloud.GetAt(N);
    //! 
    //!  - Dimiourgia euthigrammou tmimatos apo 2 simeia:
    //!      C2DLine line(p1,  p2);
    //! 
    //!  - To idio, alla me pointers sta simeia:
    //!      C2DLine line(*p1, *p2); 
    //! 
    //!  - Stin metavliti `lineset` prepei na apothikeysete ta eythigramma tmimata
    //!    pou vriskete oti anikoun sto Convex Hull.
    //!
    //!  - An `pq` einai ena C2DLine, etsi to prosthetete sto lineset:
    //!      lineset.AddCopy(pq);
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    const int num_pts = m_point_cloud.size(); // To plithos twn simeiwn.
    C2DPoint *p, *q, *t;
    C2DLineSet lineset;

    for (int pi = 0; pi < num_pts; pi++)
    {
        p = m_point_cloud.GetAt(pi);

        for (int qi = 0; qi < num_pts; qi++)
        {
            if (pi == qi) continue;
            q = m_point_cloud.GetAt(qi);
            C2DLine line(*p, *q);

            m_canvas_algo_steps.newFrame(false);

            bool keep = true;
            for (int ti = 0; ti < num_pts; ti++)
            {
                if (ti == pi || ti == qi) continue;
                t = m_point_cloud.GetAt(ti);

                if (line.IsOnRight(*t)) {
                    keep = false;
                    m_canvas_algo_steps.add(new vvr::Point2D(t->x, t->y, vvr::red));
                }
                else {
                    m_canvas_algo_steps.add(new vvr::Point2D(t->x, t->y, vvr::green));
                }

            }

            m_canvas_algo_steps.add(new vvr::Point2D(p->x, p->y, vvr::yellow));
            m_canvas_algo_steps.add(new vvr::Point2D(q->x, q->y, vvr::blue));
            m_canvas_algo_steps.add(new vvr::Line2D(p->x, p->y, q->x, q->y, keep ? vvr::magenta : vvr::black));

            if (keep) {
                lineset.AddCopy(line);
            }
        }
    }

    m_canvas_algo_steps.rew();

    // Convert line-set to polygon
    C2DPointSet pointset;
    for (int i = 0; i < lineset.size(); i++) {
        pointset.AddCopy(lineset.GetAt(i)->GetPointFrom());
    }
    m_convex_hull_polygon.Create(pointset, true);
    m_convex_hull_polygon.RemoveNullLines();
}

void ConvexHullScene::ConvexHull_Fast()
{
    // Copy the points to a temporary array,
    // to be compatible with GeoLib's functions.
    C2DPoint *pts = new C2DPoint[m_point_cloud.size()];
    for (int i = 0; i < m_point_cloud.size(); i++)
        pts[i] = *m_point_cloud.GetAt(i);

    // Create polygon from points/
    C2DPolygon cloud_polygon;
    cloud_polygon.Create(pts, m_point_cloud.size());

    // Create Convex Hull
    m_convex_hull_polygon.CreateConvexHull(cloud_polygon);

    // Don't forget to delete the array.
    delete[] pts;
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new ConvexHullScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}

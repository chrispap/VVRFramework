#include "Example_ConvexHull.h"
#include <utils.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

#define ConvexHull    ConvexHull_Fast
#define NUM_PTS_MIN   10
#define NUM_PTS_MAX   400
#define NUM_PTS_STEP  400
#define BW            150
#define BH            100

/* Constructor - Loading */

ConvexHullScene::ConvexHullScene()
{
    m_bg_col = Colour(0x44,0x44,0x44);
    reset();
}

void ConvexHullScene::reset()
{
    Scene::reset();

    // Compute Convex Hull (for various point numbers)
    for (int num_pts=NUM_PTS_MIN; num_pts <= NUM_PTS_MAX; num_pts += NUM_PTS_STEP)
    {
        m_canvas.clear();
        m_canvas_algo_steps.clear();
        createRandomPoints(m_point_cloud, num_pts);

        cout << "Computing Convex Hull for " << num_pts << " points... "; fflush(0);
        float time = vvr::getSeconds();
        //for (int rep=0; rep < 1000; rep++)
        ConvexHull();
        time = vvr::getSeconds() - time;
        cout << "Duration: " << time << " sec" << endl;
    }

}

void ConvexHullScene::createRandomPoints(C2DPointSet &point_set, int num_pts)
{
    point_set.DeleteAll();

    CRandomNumber rnw, rnh;
    rnw.SetMax(BW); rnw.SetMin(-BW);
    rnh.SetMax(BH); rnh.SetMin(-BH);
    for (int i = 0; i < num_pts; i++)
        point_set.AddCopy(rnw.GetInt(), rnh.GetInt());

    // Discard duplicate points
    point_set.SortLeftToRight(); // After sorting, duplicate points will be consecutive.
    for (int i = 0; i < point_set.size() - 1; i++) {
        if (point_set.GetAt(i)->Distance(*point_set.GetAt(i + 1)) == 0.0) {
            point_set.DeleteAt(i--);
        }
    }
}

/* UI handling */

void ConvexHullScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);

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
    m_canvas.add(new Point2D(x, y, inside ? Colour::yellow : Colour::red));

    if (!inside)
    {
        LineSeg2D *l1 = new LineSeg2D(x,y,0,0 ,Colour::blue);
        LineSeg2D *l2 = new LineSeg2D(*l1);
        const C2DLine *poly_seg;
        int i=0;

        // Find the first vissible segment
        while(!m_convex_hull_polygon.GetLine(i++)->IsOnRight(p));
        while( m_convex_hull_polygon.GetLine(i++)->IsOnRight(p));

        // Create the first line
        poly_seg = m_convex_hull_polygon.GetLine(--i);
        l1->x2 = poly_seg->GetPointFrom().x;
        l1->y2 = poly_seg->GetPointFrom().y;

        // Colour the vissisble segments
        do
        {
            m_canvas.add(new LineSeg2D(poly_seg->GetPointFrom().x,
                                       poly_seg->GetPointFrom().y,
                                       poly_seg->GetPointTo().x,
                                       poly_seg->GetPointTo().y, Colour::blue));
            poly_seg = m_convex_hull_polygon.GetLine(++i);
        } while(!poly_seg->IsOnRight(p));

        // Create the second line
        poly_seg = m_convex_hull_polygon.GetLine(--i);
        l2->x2 = poly_seg->GetPointTo().x;
        l2->y2 = poly_seg->GetPointTo().y;

        m_canvas.add(l1);
        m_canvas.add(l2);
    }

}

void ConvexHullScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    mousePressed(x, y, modif);
}

void ConvexHullScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
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

/* Drawing */

void ConvexHullScene::draw()
{
    enterPixelMode();

    // Do any drawing here.
    draw(m_convex_hull_polygon, Colour::green);
    draw(m_point_cloud, Colour::black);
    m_canvas_algo_steps.draw();
    m_canvas.draw();

    returnFromPixelMode();
}

void ConvexHullScene::draw(C2DPointSet &point_set, Colour &col)
{
    /* Draw point cloud */
    for (int i = 0; i < point_set.size(); i++) {
        Point2D(
            point_set.GetAt(i)->x,
            point_set.GetAt(i)->y,
            col).draw();
    }
}

void ConvexHullScene::draw(C2DLineSet  &line_set, Colour &col)
{
    for (int i = 0; i < line_set.size(); i++) {
        LineSeg2D(
            line_set.GetAt(i)->GetPointFrom().x,
            line_set.GetAt(i)->GetPointFrom().y,
            line_set.GetAt(i)->GetPointTo().x,
            line_set.GetAt(i)->GetPointTo().y,
            col).draw();
    }
}

void ConvexHullScene::draw(C2DPolygon  &polygon, Colour &col)
{
    for (int i = 0; i < polygon.GetLines().size(); i++) {
        LineSeg2D(
            polygon.GetLines().GetAt(i)->GetPointFrom().x,
            polygon.GetLines().GetAt(i)->GetPointFrom().y,
            polygon.GetLines().GetAt(i)->GetPointTo().x,
            polygon.GetLines().GetAt(i)->GetPointTo().y,
            col).draw();
    }
}

/* Convex Hull - Implementations */

void ConvexHullScene::ConvexHull_Slow()
{
    // HINT:
    // An `p` einai C2DPoint* tote gia na parete to `N` simeio tou point cloud
    // grafete to eksis:
    //     p = m_point_cloud.GetAt(N);
    //
    // Dimiourgia euthigrammou tmimatos apo 2 simeia:
    //     C2DLine line( p1,  p2);
    //
    // To idio, alla me pointers sta simeia:
    //     C2DLine line(*p1, *p2);
    //
    // Stin metavliti `lineset` prepei na apothikeysete ta eythigramma tmimata
    // pou vriskete oti anikoun sto Convex Hull.
    //
    // An `pq` einai ena C2DLine, etsi to prosthetete sto lineset:
    //     lineset.AddCopy(pq);
    //

    const int num_pts = m_point_cloud.size(); // To plithos twn simeiwn.
    C2DPoint *p, *q, *t;
    C2DLineSet lineset;

    for (int pi = 0; pi < num_pts; pi++) {
        p = m_point_cloud.GetAt(pi);

        for (int qi = 0; qi < num_pts; qi++) {
            if (pi == qi) continue;
            q = m_point_cloud.GetAt(qi);
            C2DLine line(*p, *q);

            m_canvas_algo_steps.newFrame(false);

            bool keep = true;
            for (int ti = 0; ti < num_pts; ti++) {
                if (ti == pi || ti == qi) continue;
                t = m_point_cloud.GetAt(ti);

                if (line.IsOnRight(*t)) {
                    keep = false;
                    m_canvas_algo_steps.add(new Point2D(t->x, t->y, Colour::red));
                }
                else {
                    m_canvas_algo_steps.add(new Point2D(t->x, t->y, Colour::green));
                }

            }

            m_canvas_algo_steps.add(new Point2D(p->x, p->y, Colour::yellow));
            m_canvas_algo_steps.add(new Point2D(q->x, q->y, Colour::blue));
            m_canvas_algo_steps.add(new Line2D(p->x, p->y, q->x, q->y, Colour::black));

            if (keep) {
                lineset.AddCopy(line);
            }
        }
    }

    m_canvas_algo_steps.rew();

    // Convert line-set to polygon
    C2DPointSet pointset;
    for (int i = 0; i < lineset.size(); i++)
        pointset.AddCopy(lineset.GetAt(i)->GetPointFrom());
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
        std::cerr << exc << std::endl;
        return 1;
    }
}

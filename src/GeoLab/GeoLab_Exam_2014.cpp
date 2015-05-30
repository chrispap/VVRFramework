#include "GeoLab_Exam_2014.h"
#include <utils.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

#define NUMPOINTS   60

/* Task Function prototypes */
int  Task_1_TriangleMesh (const C2DTriangle&, const C2DPoint&, C2DTriangle[]);
void Task_2_CollinearPoints (const C2DPointSet&, C2DPointSet&);
bool Task_3_IsOnPath (const C2DPoint&, const C2DPoint&, const C2DPoint&, const C2DPoint&, double);

/* Construct - Load  - Setup */

Exam2014Scene::Exam2014Scene()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
    reset();
}

void Exam2014Scene::reset()
{
    Scene::reset();
    
    path_width = 80;
    A = C2DPoint (-270,   118);
    B = C2DPoint (  90,   250);
    C = C2DPoint ( 392,   110);

    // Divide window to Tasks
    boundary1.Set(C2DPoint(4000, 0), C2DPoint(-4000, 0));
    boundary2.Set(C2DPoint(0, -3000), C2DPoint(0,0));
    canvas0.add(boundary1, Colour::black);
    canvas0.add(boundary2, Colour::black);

    SetupTask1();
    SetupTask2();
    SetupTask3();

    // Execute Tasks
    C2DPointSet colPts;
    Task_2_CollinearPoints(points, colPts);

    double dist1, dist2, dist3;
    for (unsigned j=0; j<colPts.size(); j+=3) {
        dist1 = colPts[j].Distance(colPts[j+1]);
        dist2 = colPts[j+1].Distance(colPts[j+2]);
        dist3 = colPts[j+2].Distance(colPts[j]);

        if (dist1>dist2 && dist1>dist3) {
            canvas2.add(C2DLine(colPts[j],colPts[j+1]), Colour::darkRed);
        }
        else if (dist2 > dist3) {
            canvas2.add(C2DLine(colPts[j+1],colPts[j+2]), Colour::darkRed);
        }
        else {
            canvas2.add(C2DLine(colPts[j+2],colPts[j]), Colour::darkRed);
        }
    }
    for (unsigned i=0; i<colPts.size(); i++) {
        canvas2.add(colPts[i], Colour::red);
    }

}

void Exam2014Scene::SetupTask1()
{
    triangle.Set(C2DPoint(-350, -100), C2DPoint(-150,-100), C2DPoint(-250, -250));
    canvas1.add(triangle, Colour::orange);
}

void Exam2014Scene::SetupTask2()
{
    points.AddCopy(C2DPoint(125, -55));
    points.AddCopy(C2DPoint( 75, -55));
    points.AddCopy(C2DPoint( 25, -55));
    points.AddCopy(C2DPoint(125,-105));
    points.AddCopy(C2DPoint( 25,-105));
    points.AddCopy(C2DPoint(125,-155));
    points.AddCopy(C2DPoint( 75,-155));
    points.AddCopy(C2DPoint( 25,-155));

    points.AddCopy(C2DPoint(175, -40));
    points.AddCopy(C2DPoint(175, -90));
    points.AddCopy(C2DPoint(175,-140));
    points.AddCopy(C2DPoint(205, -40));
    points.AddCopy(C2DPoint(205, -90));
    points.AddCopy(C2DPoint(205,-140));
    points.AddCopy(C2DPoint(235, -40));
    points.AddCopy(C2DPoint(235,-140));

    canvas2.add(C2DPoint(125, -55), Colour::green);
    canvas2.add(C2DPoint( 75, -55), Colour::green);
    canvas2.add(C2DPoint( 25, -55), Colour::green);
    canvas2.add(C2DPoint(125,-105), Colour::green);
    canvas2.add(C2DPoint( 25,-105), Colour::green);
    canvas2.add(C2DPoint(125,-155), Colour::green);
    canvas2.add(C2DPoint( 75,-155), Colour::green);
    canvas2.add(C2DPoint( 25,-155), Colour::green);

    canvas2.add(C2DPoint(175, -40), Colour::green);
    canvas2.add(C2DPoint(175, -90), Colour::green);
    canvas2.add(C2DPoint(175,-140), Colour::green);
    canvas2.add(C2DPoint(205, -40), Colour::green);
    canvas2.add(C2DPoint(205, -90), Colour::green);
    canvas2.add(C2DPoint(205,-140), Colour::green);
    canvas2.add(C2DPoint(235, -40), Colour::green);
    canvas2.add(C2DPoint(235,-140), Colour::green);

    C2DPoint pt;
    CRandomNumber rnX(  5, 500);
    CRandomNumber rnY(-350, -10);

    for (int i=0; i<NUMPOINTS; i++) {
        pt.Set(rnX.Get(), rnY.Get());
        points.AddCopy(pt);
        canvas2.add(pt, Colour::green);
    }

    canvas2.newFrame(true);

    C2DPoint mousePt = C2DPoint(37, -10);
    points.AddCopy(mousePt);
    canvas2.add(mousePt, Colour::yellow);
}

void Exam2014Scene::SetupTask3()
{
    // Make line segments
    C2DLine AB(A, B);
    C2DLine BC(B, C);

    // Make path edges
    C2DVector vw1(A, B);
    vw1.SetLength(path_width);
    vw1.TurnRight();
    C2DVector vw2(B, C);
    vw2.SetLength(path_width);
    vw2.TurnRight();
    C2DLine E2(A+vw1, B+vw1);
    C2DLine E1(A+vw1*-1, B+vw1*-1);
    C2DLine E4(B+vw2, C+vw2);
    C2DLine E3(B+vw2*-1, C+vw2*-1);
    E1.GrowFromCentre(100);
    E2.GrowFromCentre(100);
    E3.GrowFromCentre(100);
    E4.GrowFromCentre(100);

    // Find edge intersections
    C2DPointSet S;
    E1.Crosses(boundary1, &S);
    E2.Crosses(boundary1, &S);
    E1.Crosses(E3, &S);
    E2.Crosses(E4, &S);
    E4.Crosses(boundary1, &S);
    E3.Crosses(boundary1, &S);
    S[0] = A+vw1*-1;
    S[1] = A+vw1;
    S[4] = C+vw2;
    S[5] = C+vw2*-1;

    // Make dashed line helper vars
    C2DVector l1(A,B);
    C2DVector g1(l1);
    l1.SetLength(20);
    g1.SetLength(-5);
    C2DVector l2(B,C);
    C2DVector g2 = l2;
    l2.SetLength(22);
    g2.SetLength(-5);

    if (S.size()!=6) return;

    /* Add shapes */
    canvas3.clear();
    canvas3.add(C2DLine(S[0], S[2]), Colour::white);
    canvas3.add(C2DLine(S[1], S[3]), Colour::white);
    canvas3.add(C2DLine(S[2], S[5]), Colour::white);
    canvas3.add(C2DLine(S[3], S[4]), Colour::white);
    canvas3.add(C2DCircle(A,path_width), Colour::white, 0);
    canvas3.add(C2DCircle(C,path_width), Colour::white, 0);
    canvas3.add(C2DTriangle(S[0], S[2], S[1]), Colour::grey, 1);
    canvas3.add(C2DTriangle(S[1], S[2], S[3]), Colour::grey, 1);
    canvas3.add(C2DTriangle(S[2], S[5], S[4]), Colour::grey, 1);
    canvas3.add(C2DTriangle(S[3], S[2], S[4]), Colour::grey, 1);
    canvas3.add(C2DCircle(A,path_width), Colour::grey, true);
    canvas3.add(C2DCircle(C,path_width), Colour::grey, true);
    for (int i=0;i<19; i++) canvas3.add(C2DLine(A+l1*i, A+l1*(i+1)+g1), Colour::white);
    for (int i=0;i<15; i++) canvas3.add(C2DLine(B+l2*i, B+l2*(i+1)+g2), Colour::white);
    canvas3.add(C2DLine(AB.GetMidPoint(), AB.GetMidPoint()+vw1), Colour::blue);

    canvas3.add(A, Colour::cyan);
    canvas3.add(C, Colour::cyan);
    canvas3.add(B, Colour::cyan);
}

/* UI Handling */

void Exam2014Scene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    HandlePoint(C2DPoint(x,y));
}

void Exam2014Scene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    mousePressed(x,y,modif);
}

void Exam2014Scene::HandlePoint(const C2DPoint &p)
{
    if (boundary1.IsOnRight(p))
        HandleTask3(p);
    else if (boundary2.IsOnRight(p))
        HandleTask2(p);
    else
        HandleTask1(p);
}

void Exam2014Scene::HandleTask1(const C2DPoint &p)
{
    C2DTriangle tr[3];
    int count = Task_1_TriangleMesh(triangle, p, tr);
    canvas1.resize(1);
    canvas1.newFrame(true);

    for (int i=0; i<count; i++) {
        canvas1.add(tr[i], Colour::darkOrange);
    }

    canvas1.add(p, Colour::cyan);
}

void Exam2014Scene::HandleTask2(const C2DPoint &p)
{
    canvas2.resize(1);
    canvas2.newFrame(true);

    points[points.size()-1].Set(p.x, p.y);
    canvas2.add(p, Colour::yellow);

    C2DPointSet colPts;
    Task_2_CollinearPoints(points, colPts);

    double dist1, dist2, dist3;
    for (unsigned j=0; j<colPts.size(); j+=3) {
        dist1 = colPts[j].Distance(colPts[j+1]);
        dist2 = colPts[j+1].Distance(colPts[j+2]);
        dist3 = colPts[j+2].Distance(colPts[j]);

        if (dist1>dist2 && dist1>dist3) {
            canvas2.add(C2DLine(colPts[j],colPts[j+1]), Colour::darkRed);
        }
        else if (dist2 > dist3) {
            canvas2.add(C2DLine(colPts[j+1],colPts[j+2]), Colour::darkRed);
        }
        else {
            canvas2.add(C2DLine(colPts[j+2],colPts[j]), Colour::darkRed);
        }
    }

    for (unsigned i=0; i<colPts.size(); i++) {
        canvas2.add(colPts[i], Colour::red);
    }

}

void Exam2014Scene::HandleTask3(const C2DPoint &p)
{
    canvas3.resize(1);
    canvas3.newFrame(true);
    canvas3.add(p, Task_3_IsOnPath(A, B, C, p, path_width) ? Colour::green : Colour::red);
}

/* Drawing */

void Exam2014Scene::draw()
{
    enterPixelMode();

    canvas0.draw();
    canvas1.draw();
    canvas2.draw();
    canvas3.draw();
}

/* Application Entry Point */

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new Exam2014Scene);
}

/*! Dosmenou enos trigwnou kai enos shmeiou dhmiourghste ta trigwna pou exoun koryfh
 *  to shmeio auto kai 2 apo ta shmeia tou arxikou trigwnou.
 *  Oi pleures twn trigwn pou tha ftia3ete den tha prepei na diastauronontai me to
 *  tis pleures tou arxikou trigwnou
 * @param[in]  tr To trigwno pou dinetai einai to portokali pou vlepetai me clockwise
 *  fora shmeiwn 1,2,3
 * @param[in]  pt To shmeio pou dinetai einai to shmeio pou kanate aristero click
 * @param[out] newTriangles[] Pinakas poy meta thn klish ths synarthshs tha prepei na
 *  periexei ta trigwna pou dhmiourgountai apo to shmeio pt kai to trigwno tr kai dn
 *  temnontai meta3i tous. Ta trigwna tha prepei na briskontai sthn mikroterh adeia
 *  thesh tou pinaka (to poly tria).
 * @return Epistrefei ton arithmo twn trigwnwn pou dhmiourgh8hkan
 */
int Task_1_TriangleMesh(const C2DTriangle &tr, const C2DPoint &pt, C2DTriangle newTriangles[]) 
{
    C2DLine ln1, ln2, ln3;
    int counter=0;

    ln1 = C2DLine(tr.GetPoint1(), tr.GetPoint2());
    ln2 = C2DLine(tr.GetPoint2(), tr.GetPoint3());
    ln3 = C2DLine(tr.GetPoint3(), tr.GetPoint1());

    if (ln1.IsOnRight(pt) && ln3.IsOnRight(pt) && ln2.IsOnRight(pt)) {
        newTriangles[0].Set(tr.GetPoint1(), tr.GetPoint2(), pt);
        newTriangles[1].Set(tr.GetPoint2(), tr.GetPoint3(), pt);
        newTriangles[2].Set(tr.GetPoint3(), tr.GetPoint1(), pt);
        return 3;
    }

    if (!ln1.IsOnRight(pt))
        newTriangles[counter++].Set(tr.GetPoint1(), tr.GetPoint2(), pt);

    if (!ln2.IsOnRight(pt))
        newTriangles[counter++].Set(tr.GetPoint2(), tr.GetPoint3(), pt);

    if (!ln3.IsOnRight(pt))
        newTriangles[counter++].Set(tr.GetPoint3(), tr.GetPoint1(), pt);

    return counter;
}

/*! Dosmenwn triwn shmeiwn epistrefei an auta einai syneftheiaka
 * @param[in]  p1 To prwto shmeio
 * @param[in]  p2 To deutero shmeio
 * @param[in]  p3 To trito shmeio
 * @return Epistrefei an ta tria auta shmeia einai shneftheiaka me akribeia klishs
 *  4 dekadikwn psifiwn
 */
bool isCollinear(const C2DPoint &p1, const C2DPoint &p2, const C2DPoint &p3)
{
    if ((p1.x==p2.x) && (p1.x==p3.x)) return true;
    
    double a1 = (p1.y - p2.y)/(p1.x - p2.x);
    double a2 = (p1.y - p3.y)/(p1.x - p3.x);
    return (abs(a1 - a2) < 0.0001);
}

/*! Dosmenou enos nefous shmeiwn ypologizei poia apo afta einai suneftheiaka
 * @param[in]  cloudPts C2DPointSet apo shmeia
 * @param[out] collinearPts C2DPointSet ta shmeia pou einai shneftheiaka ana triades
 *  ena shmeio mporei na einai panw apo mia fora sto Set.
 */
void Task_2_CollinearPoints(const C2DPointSet &cloudPts, C2DPointSet &collinearPts) 
{
    for (unsigned i=0, num=cloudPts.size(); i<num; i++) {
        for (unsigned j=i+1; j<num; j++) {
            for (unsigned k=j+1; k<num; k++) {
                if (isCollinear(cloudPts[i], cloudPts[j], cloudPts[k])) {
                    collinearPts.AddCopy(cloudPts[i]);
                    collinearPts.AddCopy(cloudPts[j]);
                    collinearPts.AddCopy(cloudPts[k]);
                }
            }
        }
    }
}

/*! Dosmenou enos dromou pou orizetai apo ta euthigrama tmhmata(?) AB,BC, kai me platos
 *  path_width apo tis eutheies, opws fainetai sto sxhma kai enos shmeiou p ypologiste
 *  kai epistrepste true h false an to shmeio afto brisketai entos h ektos tou dromou.
 * @param[in]  A To shmeio A tou dromou opws fainetai sto sxhma
 * @param[in]  B To shmeio B tou dromou opws fainetai sto sxhma
 * @param[in]  C To shmeio C tou dromou opws fainetai sto sxhma
 * @param[in]  p To shmeio pou kaneis click me to pontiki
 * @param[in]  path_width To paxos tou dromou opws fainetai me thn mple gramh sto sxhma
 * @return Epistrefei true an to shmeio p einai mesa entos tou dromou alliws false
 */
bool Task_3_IsOnPath(const C2DPoint &A, const C2DPoint &B, const C2DPoint &C, const C2DPoint &p, double path_width)
{
    // Make line segments
    C2DLine AB(B, A);
    C2DLine BC(B, C);

    double d1, d2, d1_, d2_;
    C2DPoint np1, np2;
    d1 = AB.Distance(p);
    d2 = BC.Distance(p);
    d1_ = AB.DistanceAsRay(p);
    d2_ = BC.DistanceAsRay(p);

    if (d1==d2) return (d1_<path_width && d2_<path_width);
    else return (min(d1,d2)<path_width);
}

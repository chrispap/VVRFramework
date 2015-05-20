#include <scene.h>
#include <mesh.h>
#include <vector>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

using namespace std;
using namespace vvr;
using namespace math;

bool Task_3_IsOnPath(const C2DPoint&, const C2DPoint&, const C2DPoint&, const C2DPoint&, double);

class GeoLabExam2015Scene : public vvr::Scene
{
public:
    GeoLabExam2015Scene();
    const char* getName() const { return "GeoLab 2015 Exam"; }
    void keyEvent(unsigned char key, bool up, int modif) override;

protected:
    void draw() override;
    void resize() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

private:
    void SetupTask3();
    void HandleTask1(const C2DPoint &p);
    void HandleTask2(const C2DPoint &p);
    void HandleTask3(const C2DPoint &p);

private:

    // Task 2
private:
    vvr::Mesh::Ptr  m_mesh;
    vvr::Colour     m_obj_col;
    int             m_style_flag;
    int             m_mouselastX, m_mouselastY;

    // Task 3
private:
    C2DLine     boundary1;
    Canvas2D    canvas3;
    C2DPoint    A, B, C;
    double      path_width;

};

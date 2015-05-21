#include <scene.h>
#include <mesh.h>
#include <vector>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

using namespace std;
using namespace vvr;
using namespace math;

class Exam2015Scene : public vvr::Scene
{
public:
    Exam2015Scene();
    const char* getName() const { return "GeoLab 2015 Exam"; }
    void keyEvent(unsigned char key, bool up, int modif) override;

protected:
    void draw() override;
    void resize() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

private:
    // Task methods
    bool Task_Path(const C2DPoint &p);
    void Task_ParallelLines();
    void Task_3D();

    // Task Lines
    C2DPointSet     m_point_set, m_convex_hull;
    C2DLine         m_line1;
    C2DLine         m_line2;

    // Task Path
    float           m_path_width;
    Canvas2D        m_path_canvas;
    C2DPoint        A, B, C;

    // Task 3D
    Mesh::Ptr       m_mesh;
    Colour          m_obj_col;
    int             m_style_flag;
    int             m_mouse_last_x;
    int             m_mouse_last_y;
};

#include <vvr/scene.h>
#include <vvr/mesh.h>
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

#include <vvr/scene.h>
#include <vvr/mesh.h>

class CGALSurfaceMesher : public vvr::Scene
{
    enum {
        SHOW_SOLID, 
        SHOW_WIRE, 
        SHOW_NORMALS,
    };

public:
    CGALSurfaceMesher();

private:
    void keyEvent(unsigned char key, bool up, int modif) override;
    void sliderChanged(int slider_id, float val) override;
    void resize() override;
    void draw() override;

private:
    vvr::Mesh m_mesh;
    unsigned m_flag;
};

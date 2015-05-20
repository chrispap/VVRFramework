#include <scene.h>
#include <mesh.h>
#include <vector>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

using namespace std;
using namespace vvr;
using namespace math;

class TemplateScene : public vvr::Scene
{
public:
    TemplateScene();
    const char* getName() const { return "Template Scene"; }
    void keyEvent(unsigned char key, bool up, int modif) override;

protected:
    void draw() override;
    void resize() override;

private:
    vvr::Mesh::Ptr  m_mesh;
    vvr::Colour     m_obj_col;
    int             m_style_flag;
};

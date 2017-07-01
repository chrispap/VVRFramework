#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vector>
#include <string>
#include <MathGeoLib.h>

class HelixScene : public vvr::Scene
{
public:
    HelixScene();
    const char* getName() const { return "Simple 3D Scene"; }
    void keyEvent(unsigned char key, bool up, int modif) override;

protected:
    void draw() override;
    void resize() override;
    bool idle() override;

private:
    float m_r, m_c;
    double m_sec;
    int m_style_flag;
    vvr::Animation m_anim;
};

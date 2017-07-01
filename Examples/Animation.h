#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vector>
#include <string>
#include <MathGeoLib.h>

class AnimationScene : public vvr::Scene 
{
public:
    AnimationScene();
    const char* getName() const { return "Animation Scene";}

protected:
    void draw() override;
    void resize() override;
    void reset() override;
    bool idle() override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    float m_sphere_rad;
    bool m_pause;
};

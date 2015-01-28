#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class TemplateScene : public vvr::Scene
{
public: 
    const char* getName() const { return "Template VVR Scene";}
    TemplateScene();
    bool idle();
    void onNewOrientationData(string &data);

protected: //methods
    void draw();
    void load();

private: //methods
    void loadData(string, vector<vvr::Vec3d> &rots, vector<double> &times);

private: //data
    vvr::Settings m_settings;
    double m_bone_width;
    double m_anim_start_time, tsf;
    bool m_anim_on;

private: // Bone 3D model and motion data
    vvr::Mesh *ulna, *humerus;
    vector<double> m_times_ulna, m_times_humerus;
    vector<vvr::Vec3d> m_rots_ulna, m_rots_humerus;
};

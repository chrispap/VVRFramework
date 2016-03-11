#include "scene.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <string>
#include <algorithm>
#include <locale>
#include <cmath>
#include <QtOpenGL>
#include <MathGeoLib.h>

using namespace vvr;
using namespace std;
using namespace math;

static void GL_Info ()
{
    /* PRINT OpenGL INFO */
    printf("\n=== VVR Framework ================\n");
    printf(" %s\n", glGetString(GL_VERSION ));
    printf(" %s\n", glGetString(GL_VENDOR  ));
    printf(" %s\n", glGetString(GL_RENDERER));
    printf("==================================\n\n");
    fflush(0);
}

#define VVR_FOV_MAX 120
#define VVR_FOV_MIN 10

Scene::Scene()
{
    m_world_rot_def = Vec3d(0,0,0);
    m_world_rot = m_world_rot_def;
    m_perspective_proj = false;
    m_fullscreen = false;
    m_create_menus = true;
    m_hide_log = true;
    m_hide_sliders = true;
    m_camera_dist = 100;
    m_fov = 30;
}

const char* Scene::getName() const
{
    return "VVR Framework Scene";
}

void Scene::drawAxes()
{
    GLfloat len = 2.0 * getSceneWidth();

    glBegin(GL_LINES);
    
    // X
    glColor3ub(0xFF, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(len, 0, 0);
    
    // Y
    glColor3f(0, 0xFF, 0);
    glVertex3f(0,0,0);
    glVertex3f(0, len, 0);
    
    // Z
    glColor3f(0, 0, 0xFF);
    glVertex3f(0,0,0);
    glVertex3f(0, 0, len);

    glEnd();
}

void Scene::GL_Init()
{
    // Light setup
    float lz = m_camera_dist * 3;
    static GLfloat light_position[] = {   0,    0,   lz,   1};
    static GLfloat ambientLight[]   = { .75,  .75,  .75,   1};
    static GLfloat diffuseLight[]   = { .75,  .75,  .75,   1};
    static GLfloat specularLight[]  = { .85,  .85,  .85,   1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GL_Info();
}

void Scene::GL_Resize(int w, int h)
{
    const float ar = (float)w / h;
    m_screen_width = w;
    m_screen_height = h;

    const vec pos(0, 0, m_camera_dist);
    const vec front(0, 0, -1);
    const vec up(0, 1, 0);
    m_frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    m_frustum.SetFrame(pos, front, up);

    if (m_perspective_proj)
    {
        const float n = m_camera_dist * 0.01;
        const float f = m_camera_dist * 100;
        m_scene_width = 1.5f * (m_camera_dist * 2 * tanf(DegToRad(m_fov / 2)));
        m_scene_height = m_scene_width / ar;
        m_frustum.SetVerticalFovAndAspectRatio(DegToRad(m_fov), ar);
        m_frustum.SetViewPlaneDistances(n, f);
    }
    else
    {
        const float n = -m_camera_dist * 2;
        const float f = m_camera_dist * 2;
        m_scene_width = m_camera_dist / 2;
        m_scene_height = m_scene_width / ar;
        m_frustum.SetOrthographic(m_scene_width, m_scene_height);
        m_frustum.SetViewPlaneDistances(n, f);
    }

    //! Set OpenGL Projection Matrix
    glViewport(0, 0, m_screen_width, m_screen_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    m_proj_mat = m_frustum.ProjectionMatrix();
    m_proj_mat.Transpose(); // Covert to colunm major for OpenGL
    glMultMatrixf(m_proj_mat.ptr());
    resize();
}

void Scene::GL_Render()
{
    glClearColor(m_bg_col.r/255.0, m_bg_col.g/255.0, m_bg_col.b/255.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float4x4 mvm = modelViewMatrix();
    mvm.Transpose(); // Covert to colunm major for OpenGL
    glMultMatrixf((GLfloat*)mvm.v);
    draw();
}

void Scene::keyEvent (unsigned char key,  bool up, int modif)
{
    if (up) return;

    int ctrl  =  modif & 0x01;
    int shift =  modif & 0x02;

    switch (isprint(key)? tolower(key): key) {
    case '0' : m_world_rot = m_world_rot_def; break;
    case 'r' : this->reset();
    }

}

void Scene::arrowEvent (ArrowDir dir, int modif)
{
    int ctrl  =  modif & 0x01;
    int shift =  modif & 0x02;

}

void Scene::mousePressed(int x, int y, int modif)
{
    m_mouselastX = x;
    m_mouselastY = y;
}

void Scene::mouseReleased(int x, int y, int modif)
{

}

void Scene::mouseMoved(int x, int y, int modif)
{
    int dx = x - m_mouselastX;
    int dy = y - m_mouselastY;

    m_world_rot.x -= dy;
    m_world_rot.y += dx;

    m_world_rot.x = fmod(m_world_rot.x, 360.0);
    while (m_world_rot.x < 0) m_world_rot.x += 360;

    m_world_rot.y = fmod(m_world_rot.y, 360.0);
    while (m_world_rot.y < 0) m_world_rot.y += 360;

    m_mouselastX = x;
    m_mouselastY = y;
}

void Scene::mouseWheel(int dir, int modif)
{
    m_fov += -4.0 * dir;
    if (m_fov < VVR_FOV_MIN) m_fov = VVR_FOV_MIN;
    else if (m_fov > VVR_FOV_MAX) m_fov = VVR_FOV_MAX;
    GL_Resize(m_screen_width, m_screen_height);
}

void Scene::sliderChanged(int slider_id, float val) 
{

}

void Scene::setSliderVal(int slider_id, float val)
{
    if (slider_id > 5 || slider_id < 0) return;
    if (val > 1) val = 1;
    else if (val < 0) val = 0;
    //TODO: Connect scene object backwards to window in order to be able to change 
    //      slider values from the scene.
}

void Scene::reset()
{
    m_world_rot = m_world_rot_def;
}

void Scene::enterPixelMode()
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-m_screen_width/2, m_screen_width/2, -m_screen_height/2, m_screen_height/2, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void Scene::returnFromPixelMode()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Scene::mouse2pix(int &x, int &y)
{
    x -= m_screen_width/2;
    y -= m_screen_height/2;
    // Reverse the default window coordinate system so that y grows upwards.
    y = -y; 
}

float4x4 Scene::modelViewMatrix()
{
    float4x4 M = float4x4::Translate(vec(0, 0, -m_camera_dist)) *
        float4x4::RotateX(math::DegToRad(m_world_rot.x)) *
        float4x4::RotateY(math::DegToRad(m_world_rot.y)) *
        float4x4::RotateZ(math::DegToRad(m_world_rot.z));
    return M;
}

Ray Scene::unproject(float x, float y)
{
    Ray ray = m_frustum.UnProject(x, y);
    return ray;
}

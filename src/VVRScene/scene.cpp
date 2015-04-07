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
#include <MathGeoLib\MathGeoLib.h>

using namespace vvr;
using namespace std;
using namespace math;

Scene::Scene()
{
    m_globRot_def = Vec3d(0,0,0);
    m_globRot = m_globRot_def;
    m_perspective_proj = false;
}

const char* Scene::getName() const
{
    return "VVR Framework Scene";
}

void Scene::drawAxes()
{
    GLfloat len = 2.0 * getScreenWidth();
    glLineWidth(1);
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

/* OpenGL Callbacks */
void Scene::GL_Init()
{
    // Light setup
    static GLfloat light_position[] = { 0, 0, m_camera_dist*3,   1};
    static GLfloat ambientLight[]   = { .75,  .75,  .75,   1};
    static GLfloat diffuseLight[]   = { .75,  .75,  .75,   1};
    static GLfloat specularLight[]  = { .85,  .85,  .85,   1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    
    // glEnable(...)
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

    /* PRINT OpenGL INFO */
    printf("=== VVR Framework ================\n");
    printf(" %s\n", glGetString(GL_VERSION ));
    printf(" %s\n", glGetString(GL_VENDOR  ));
    printf(" %s\n", glGetString(GL_RENDERER));
    printf("==================================\n");
}

void Scene::GL_Resize(int w, int h)
{
    m_screen_width = w;
    m_screen_height = h;

    glViewport(0,0, m_screen_width, m_screen_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    m_camera_dist = 800;
    const auto d = m_camera_dist;
    float4x4 proj_mat;

    if (m_perspective_proj) 
        proj_mat = float4x4::OpenGLPerspProjRH(d/2, d*2, 1., 1. * (float)h/w);
    else 
        proj_mat = float4x4::OpenGLOrthoProjRH(-d*2, d*2, w, h);
    
    glMultMatrixf(proj_mat.ptr());
    resize();
}

void Scene::GL_Render()
{
    glClearColor(m_bg_col.r/255.0, m_bg_col.g/255.0, m_bg_col.b/255.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Push the scene to the back (far from camera)
    glTranslatef(0,0, -m_camera_dist);

    // Apply global rotation & draw
    glRotatef(m_globRot.x, 1, 0, 0);
    glRotatef(m_globRot.y, 0, 1, 0);
    glRotatef(m_globRot.z, 0, 0, 1);
    draw();
}

/* UI Callbacks */
void Scene::keyEvent (unsigned char key,  bool up, int modif)
{
    if (up) return;

    int ctrl  =  modif & 0x01;
    int shift =  modif & 0x02;

    switch (isprint(key)? tolower(key): key) {
    case '0' : m_globRot = m_globRot_def; break;
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

void Scene::mouseMoved(int x, int y, int modif)
{
    int dx = x - m_mouselastX;
    int dy = y - m_mouselastY;

    m_globRot.x += dy;
    m_globRot.y += dx;

    m_globRot.x = fmod(m_globRot.x, 360.0);
    while (m_globRot.x < 0) m_globRot.x += 360;

    m_globRot.y = fmod(m_globRot.y, 360.0);
    while (m_globRot.y < 0) m_globRot.y += 360;

    m_mouselastX = x;
    m_mouselastY = y;
}

void Scene::mouseWheel(int dir, int modif)
{
    m_camera_dist += -20.0 * dir;
    if (m_camera_dist < 0.01) m_camera_dist = 0.01;
}

void Scene::reset()
{
    m_globRot = m_globRot_def;
}

/* Utils */
void Scene::enterPixelMode()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-m_screen_width/2, m_screen_width/2, m_screen_height/2, -m_screen_height/2, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void Scene::returnFromPixelMode()
{
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Scene::mouse2pix(int &x, int &y)
{
    x -= m_screen_width/2;
    y -= m_screen_height/2;
}

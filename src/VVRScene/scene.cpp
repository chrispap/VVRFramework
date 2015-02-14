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

//light
#define L_POS_X 20.0
#define L_POS_Y 50.0
#define L_POS_Z 100.0
#define L_POS_W 0.0

using namespace vvr;
using namespace std;

Scene::Scene()
{
    m_globRotDef = Vec3d(0,0,0);
    m_globRot = m_globRotDef;
    m_perspective_proj = false;
}

const char* Scene::getName() const
{
    return "VVR Framework Scene";
}

void Scene::drawAxes()
{
    GLfloat len = 2.0 * m_scene_width;
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
    GLfloat light_position[] = { 0, m_scene_height, m_scene_dist,   1};
    GLfloat ambientLight[]   = { .5,  .5,  .5,   1};
    GLfloat diffuseLight[]   = { .9,  .9,  .9,   1};
    GLfloat specularLight[]  = { .9,  .9,  .9,   1};
    
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
    //glEnable(GL_BLEND);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::GL_Resize(int w, int h)
{
    m_screen_width = w;
    m_screen_height = h;

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(m_perspective_proj)
    {
//        gluPerspective(
//            10.0,
//            (float)w/h,
//            m_scene_dist * 0.002,
//            m_scene_dist * 20
//        );
    }
    else
    {
        // Keep constant aspect ratio
        m_scene_height = m_scene_width * h/w;

        glOrtho(
            -m_scene_width  / 2,
             m_scene_width  / 2,
            -m_scene_height / 2,
             m_scene_height / 2,
             m_scene_dist*-2,       // -Z look to the user! Near should be negative!
             m_scene_dist* 2        // Far should be positive!
        );
    }

}

void Scene::GL_Render()
{
    glClearColor(m_bgCol.r/255.0, m_bgCol.g/255.0, m_bgCol.b/255.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Push the scene to the back (far from camera)
    glTranslatef(0,0, m_scene_dist);

    // Apply global rotation & draw
    glRotatef(m_globRot.x, 1, 0, 0);
    glRotatef(m_globRot.y, 0, 1, 0);
    glRotatef(m_globRot.z, 0, 0, 1);
    draw();
}

/* UI Callbacks */
void Scene::keyEvent (unsigned char key,  bool up, int x, int y, int modif)
{
    if (up) return;

    int ctrl  =  modif & 0x01;
    int shift =  modif & 0x02;

    switch (isprint(key)? tolower(key): key) {
    case '0' : m_globRot = m_globRotDef; break;
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
    m_scene_dist += 0.2*dir;

    if (m_scene_dist < 0.01) m_scene_dist = 0.01;
}

void Scene::reset()
{
    m_globRot = m_globRotDef;
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

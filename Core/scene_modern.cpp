#include <vvr/scene_modern.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>
#include <QtGui>
#include <qopenglext.h>

namespace vvr
{
    struct SceneModern::Impl : QOpenGLExtraFunctions
    {
        SceneModern *scene;
        Impl(SceneModern *scene) : scene(scene) { }
        void setupGL();
        void draw();

    private:
        GLuint shader;
        GLuint vs, fs;
        GLuint vbo = 0;
        GLuint vao = 0;
    };
}

const char * vvr::SceneModern::name = "Modern OpenGL scene";

vvr::SceneModern::SceneModern()
{
    m_bg_col = vvr::grey;
    m_show_log = true;
    impl = new Impl(this);
}

vvr::SceneModern::~SceneModern()
{
    delete impl;
}

void vvr::SceneModern::resize()
{
    if (m_first_resize) {
        impl->setupGL();
    }
}

void vvr::SceneModern::draw()
{
    impl->draw();
}

void vvr::SceneModern::Impl::setupGL()
{
    char infoLog[512];
    int  ok;

    //---[Paths]---
    const std::string shader_path = vvr::get_base_path() + "resources/shaders/";
    const std::string vert_shader = vvr::read_file(shader_path + "basic.vert");
    const std::string frag_shader = vvr::read_file(shader_path + "basic.frag");
    const char* vert_shader_ptr = vert_shader.c_str();
    const char* frag_shader_ptr = frag_shader.c_str();

    //---[Geom data]---
    float a = scene->getSceneWidth() / 4;
    vvr::Triangle3D tri({
        { 0.0f,  0.0f, 0.0f },
        {    a,  0.0f, 0.0f },
        {    a,     a, 0.0f }
    });

    initializeOpenGLFunctions();

    //---[Buffers]---
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(math::Triangle), static_cast<math::Triangle*>(&tri), GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //---[Shaders]---
    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vert_shader_ptr, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(vs, 512, NULL, infoLog); vvr_msg(infoLog); }
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &frag_shader_ptr, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(fs, 512, NULL, infoLog); vvr_msg(infoLog); }
    shader = glCreateProgram();
    glAttachShader(shader, fs);
    glAttachShader(shader, vs);
    glLinkProgram(shader);
}

void vvr::SceneModern::Impl::draw()
{
    glUseProgram(shader);
    
    //---[Setup matrices in shaders]---
    auto frustum = scene->getFrustum();
    float4x4 mvp = frustum.ViewProjMatrix();
    mvp.Transpose(); // Colunm major for OpenGL.
    GLuint mvp_id = glGetUniformLocation(shader, "mvp");
    glUniformMatrix4fv(mvp_id, 1, GL_FALSE, mvp.ptr());
    
    //---[Render]---
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

vvr_invoke_main_with_scene(vvr::SceneModern)

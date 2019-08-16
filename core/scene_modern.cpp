#include <vvr/scene_modern.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>
#include <QtGui>
#include <qopenglext.h>

//---
namespace vvr { struct SceneModern::Impl : QOpenGLExtraFunctions { }; }

//---
const char * vvr::SceneModern::name = "Modern OpenGL scene";

//---
GLuint shader_program;
GLuint vs, fs;
GLuint vbo = 0;
GLuint vao = 0;

//---
vvr::SceneModern::SceneModern()
{
    m_bg_col = vvr::grey;
    m_show_log = true;
    gl = new Impl;
}

vvr::SceneModern::~SceneModern()
{
    delete gl;
}

void vvr::SceneModern::resize()
{
    if (m_first_resize) {
        setupGL();
    }
}

void vvr::SceneModern::setupGL()
{
    char infoLog[512];
    int  ok;

    //---[Paths]---
    const std::string shader_path = vvr::get_base_path() + "resources/shaders/";
    std::string src_vert_shader = vvr::read_file(shader_path + "basic.vert");
    const char* src_vert_shader_ptr = src_vert_shader.c_str();
    std::string src_frag_shader = vvr::read_file(shader_path + "basic.frag");
    const char* src_frag_shader_ptr = src_frag_shader.c_str();

    //---[Geom data]---
    vvr::Triangle3D tri({
        { 0.0f,  0.5f, 0.0f },
        { 0.5f, -0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f }
    });

    gl->initializeOpenGLFunctions();

    //---[Buffers]---
    gl->glGenBuffers(1, &vbo);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl->glBufferData(GL_ARRAY_BUFFER, sizeof(math::Triangle), &static_cast<math::Triangle>(tri), GL_STATIC_DRAW);
    gl->glGenVertexArrays(1, &vao);
    gl->glBindVertexArray(vao);
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //---[Vertex shader]---
    vs = gl->glCreateShader(GL_VERTEX_SHADER);
    gl->glShaderSource(vs, 1, &src_vert_shader_ptr, NULL);
    gl->glCompileShader(vs);
    gl->glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { gl->glGetShaderInfoLog(vs, 512, NULL, infoLog); vvr_msg(infoLog); }

    //---[Fragment shader]---

    fs = gl->glCreateShader(GL_FRAGMENT_SHADER);
    gl->glShaderSource(fs, 1, &src_frag_shader_ptr, NULL);
    gl->glCompileShader(fs);
    gl->glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { gl->glGetShaderInfoLog(fs, 512, NULL, infoLog); vvr_msg(infoLog); }

    //---[Compile / Link shader]---
    shader_program = gl->glCreateProgram();
    gl->glAttachShader(shader_program, fs);
    gl->glAttachShader(shader_program, vs);
    gl->glLinkProgram(shader_program);
}

void vvr::SceneModern::draw()
{
    gl->glUseProgram(shader_program);
    gl->glBindVertexArray(vao);
    gl->glDrawArrays(GL_TRIANGLES, 0, 3);
}
//---

vvr_invoke_main_with_scene(vvr::SceneModern)
